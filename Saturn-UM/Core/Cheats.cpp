//______                            ______                  
//|  _  \                           | ___ \                 
//| | | |_ __ __ _  __ _  ___  _ __ | |_/ /_   _ _ __ _ __  
//| | | | '__/ _` |/ _` |/ _ \| '_ \| ___ \ | | | '__| '_ \ 
//| |/ /| | | (_| | (_| | (_) | | | | |_/ / |_| | |  | | | |
//|___/ |_|  \__,_|\__, |\___/|_| |_\____/ \__,_|_|  |_| |_|
//                  __/ |                                   
//                 |___/                                    
//
//https://discord.gg/5WcvdzFybD
//https://github.com/ByteCorum/DragonBurn

#include <string>
#include <thread>
#include <future>
#include <iostream>
#include <memory>

#include "Cheats.h"
#include "Render.h"
#include "../Core/Config.h"
#include "../Core/Interfaces.h"
#include "../Core/FrameContext.h"
#include "../Core/DI.h"

#include "../Core/Init.h"

#include "../Features/ESP.h"
#include "../Core/GUI.h"
#include "../Features/RCS.H"
#include "../Features/BombTimer.h"
#include "../Features/SpectatorList.h"
#include "../Helpers/Logger.h"


int PreviousTotalHits = 0;

void Menu();
void Visual(const CEntity&);
void Radar(Base_Radar, const CEntity&);
void Trigger(const CEntity&, const int&);
void AIM(const CEntity&, std::vector<Vec3>);
void MiscFuncs(CEntity&);
void RenderCrosshair(ImDrawList*, const CEntity&);
void RadarSetting(Base_Radar&);

struct MiscFeatureAdapter : Core::IFeature {
    const CEntity* local;
    void OnFrame(const Core::FrameContext& ctx) override { local = ctx.local; }
    void OnRender() override { if (local) { CEntity copy = *local; MiscFuncs(copy); } }
};

struct VisualFeatureAdapter : Core::IFeature {
    const CEntity* local;
    void OnFrame(const Core::FrameContext& ctx) override { local = ctx.local; }
    void OnRender() override { if (local) Visual(*local); }
};

struct RadarFeatureAdapter : Core::IFeature {
    Base_Radar* radar;
    const CEntity* local;
    void OnFrame(const Core::FrameContext& ctx) override { radar = ctx.radar; local = ctx.local; }
    void OnRender() override { if (radar && local) Radar(*radar, *local); }
};

struct ESPFeatureAdapter : Core::IFeature {
    const std::vector<EntityResult>* entities;
    const CEntity* local;
    int localIndex;
    void OnFrame(const Core::FrameContext& ctx) override { entities = ctx.entities; local = ctx.local; localIndex = ctx.localControllerIndex; }
    void OnRender() override {
        if (!entities || !local) return;
        auto mm = Core::Container::Get<MemoryMgr>();
        for (const auto& result : *entities) {
            if (!result.isValid) continue;
            if (!ESPConfig::ESPenabled) continue;
            if (ESPConfig::FlashCheck && local->Pawn.FlashDuration >= 0.1f) continue;
            const ImVec4& Rect = result.espRect;
            const int distance = result.distance;
            if (MenuConfig::RenderDistance == 0 || (distance <= MenuConfig::RenderDistance && MenuConfig::RenderDistance > 0)) {
                ESP::RenderPlayerESP(*local, result.entity, Rect, localIndex, result.entityIndex);
                Render::DrawDistance(*local, result.entity, Rect);
                if (ESPConfig::ShowHealthBar) {
                    ImVec2 HealthBarPos = { Rect.x - 6.f, Rect.y };
                    ImVec2 HealthBarSize = { 4, Rect.w };
                    Render::DrawHealthBar(result.entity.Controller.Address, result.entity.Pawn.Health > 100 ? result.entity.Pawn.Health : 100, result.entity.Pawn.Health, HealthBarPos, HealthBarSize);
                }
                if (ESPConfig::ArmorBar && result.entity.Pawn.Armor > 0) {
                    bool HasHelmet;
                    ImVec2 ArmorBarPos;
                    if (mm) mm->ReadMemory(result.entity.Controller.Address + Offset.PlayerController.HasHelmet, HasHelmet);
                    if (ESPConfig::ShowHealthBar)
                        ArmorBarPos = { Rect.x - 10.f, Rect.y };
                    else
                        ArmorBarPos = { Rect.x - 6.f, Rect.y };
                    ImVec2 ArmorBarSize = { 4.f, Rect.w };
                    Render::DrawArmorBar(result.entity.Controller.Address, result.entity.Pawn.Armor > 100 ? result.entity.Pawn.Armor : 100, result.entity.Pawn.Armor, HasHelmet, ArmorBarPos, ArmorBarSize);
                }
            }
        }
    }
};

struct TriggerAimFeatureAdapter : Core::IFeature {
    const CEntity* local;
    int localIndex;
    std::vector<Vec3>* aimList;
    void OnFrame(const Core::FrameContext& ctx) override {
        local = ctx.local; localIndex = ctx.localControllerIndex; aimList = ctx.aimPosList;
        if (local) Trigger(*local, localIndex);
        if (local && aimList) AIM(*local, *aimList);
    }
    void OnRender() override {}
};

static bool g_featuresInitialized = false;
static std::vector<std::unique_ptr<Core::IFeature>> g_features_tick;
static std::vector<std::unique_ptr<Core::IFeature>> g_features_render;

void Cheats::Run()
{	
	Menu();

	Misc::AutoAccept::UpdateAutoAccept();

#ifndef DBDEBUG
	if (!Init::Client::isGameWindowActive() && !MenuConfig::ShowMenu) {
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		return;
	}
#endif

	// Update matrix
	if (!memoryManager.ReadMemory(gGame.GetMatrixAddress(), gGame.View.Matrix,64))
		return;

	// Update EntityList Entry
	gGame.UpdateEntityListEntry();

	DWORD64 LocalControllerAddress = 0;
	DWORD64 LocalPawnAddress = 0;

	if (!memoryManager.ReadMemory(gGame.GetLocalControllerAddress(), LocalControllerAddress))
		return;
	if (!memoryManager.ReadMemory(gGame.GetLocalPawnAddress(), LocalPawnAddress))
		return;

	if (LocalPawnAddress == 0 || LocalControllerAddress == 0) {
        g_globalVars->UpdateGlobalvars();
        cachedResults.clear();
        return;
    }

	// LocalEntity
	CEntity LocalEntity;
	int LocalPlayerControllerIndex = 0;
	LocalEntity.UpdateClientData();
	if (!LocalEntity.UpdateController(LocalControllerAddress))
		return;
	if (!LocalEntity.UpdatePawn(LocalPawnAddress) && !MenuConfig::WorkInSpec)
		return;

	// Update m_currentTick
	bool success = memoryManager.ReadMemory<DWORD>(LocalEntity.Controller.Address + Offset.PlayerController.m_nTickBase, m_currentTick);
	if (!success) {
		m_currentTick = 0;
	}

	// aimbot data
	std::vector<Vec3> AimPosList;

	// radar data
	Base_Radar GameRadar;
	if ((RadarCFG::ShowRadar && LocalEntity.Controller.TeamID != 0) || (RadarCFG::ShowRadar && MenuConfig::ShowMenu))
		RadarSetting(GameRadar);

	// process entities
	auto entityResults = ProcessEntities(LocalEntity, LocalPlayerControllerIndex);
	
	// render, collect aim data
	HandleEnts(entityResults, LocalEntity, LocalPlayerControllerIndex, GameRadar, AimPosList);

    if (!g_featuresInitialized) {
        g_features_tick.emplace_back(std::make_unique<TriggerAimFeatureAdapter>());
        g_features_render.emplace_back(std::make_unique<VisualFeatureAdapter>());
        g_features_render.emplace_back(std::make_unique<RadarFeatureAdapter>());
        g_features_render.emplace_back(std::make_unique<ESPFeatureAdapter>());
        g_features_render.emplace_back(std::make_unique<MiscFeatureAdapter>());
        g_featuresInitialized = true;
    }

    Core::FrameContext ctx{ &LocalEntity, LocalPlayerControllerIndex, &entityResults, &AimPosList, &GameRadar };
    for (auto& f : g_features_render) {
        f->OnFrame(ctx);
    }

	int currentFPS = static_cast<int>(ImGui::GetIO().Framerate);
	if (currentFPS > MenuConfig::RenderFPS)
	{
		int FrameWait = round(1000.0f / MenuConfig::RenderFPS);
		std::this_thread::sleep_for(std::chrono::milliseconds(FrameWait));
	}

	for (auto& f : g_features_render) {
		f->OnRender();
	}
	
	// run trigger & aim every new tick
	if (m_currentTick != m_previousTick)
	{
        for (auto& f : g_features_tick) {
            f->OnFrame(ctx);
        }
		
		std::vector<CEntity> allEntities;
		for (const auto& pair : cachedResults) {
			allEntities.push_back(pair.second);
		}
		SpecList::GetSpectatorList(allEntities, LocalEntity);
		m_previousTick = m_currentTick;
	}
}

// collect entity data
std::vector<std::pair<int, CEntity>> Cheats::CollectEntityData(CEntity& localEntity, int& localPlayerControllerIndex)
{
	// update only on new tick
	//if (m_currentTick == m_previousTick)
	//{
	//	return cachedResults;
	//}

	std::vector<EntityBatchData> batchData;
	batchData.reserve(64);

	// collect entity addresses
	for (int entityIndex = 0; entityIndex < 64; ++entityIndex)
	{
		DWORD64 entityAddress = 0;
		if (!memoryManager.ReadMemory<DWORD64>(gGame.GetEntityListEntry() + (entityIndex + 1) * 0x70, entityAddress))
		{
			continue;
		}

		// skip local player
		if (entityAddress == localEntity.Controller.Address)
		{
			localPlayerControllerIndex = entityIndex;
			continue;
		}

		// get pawn address
		CEntity tempEntity;
		tempEntity.Controller.Address = entityAddress;
		DWORD64 pawnAddress = tempEntity.Controller.GetPlayerPawnAddress();
		
		if (pawnAddress != 0)
		{
			batchData.emplace_back(entityIndex, entityAddress, pawnAddress);
		}
	}

	if (batchData.empty())
	{
		return {};
	}

	// process all entities in batch
	std::vector<std::pair<int, CEntity>> entities;
	EntityBatchProcessor processor;
	if (!processor.ProcessAllEntities(entities, batchData))
	{
		return {};
	}

	// update cache
	cachedResults = entities;

	return cachedResults;
}

// process, prepare results
std::vector<EntityResult> Cheats::ProcessEntities(CEntity& localEntity, int& localPlayerControllerIndex)
{
	// get batch-processed entities
	auto entities = CollectEntityData(localEntity, localPlayerControllerIndex);
	std::vector<EntityResult> results;
	results.reserve(entities.size());

	// process each entity
	for (auto& [entityIndex, entity] : entities)
	{
		EntityResult result;
		result.entityIndex = entityIndex;
		result.entity = entity;

		if (!entity.IsAlive())
			continue;

		// skip teammates if team check enabled
		if (MenuConfig::TeamCheck && entity.Controller.TeamID == localEntity.Controller.TeamID)
			continue;

		// check if in screen
		result.isInScreen = entity.IsInScreen();

		// calculate distance
		result.distance = static_cast<int>(entity.Pawn.Pos.DistanceTo(localEntity.Pawn.Pos) / 100);

		// calculate esp box rect
		if (ESPConfig::ESPenabled && result.isInScreen)
			result.espRect = ESP::GetBoxRect(entity, ESPConfig::BoxType);

        

		result.isValid = true;
		results.push_back(result);
	}
	
	return results;
}

// render, collect aim data
void Cheats::HandleEnts(const std::vector<EntityResult>& entities, CEntity& localEntity, 
	int localPlayerControllerIndex, Base_Radar& gameRadar, std::vector<Vec3>& aimPosList)
{
	// healthbar map (static)
	static std::map<DWORD64, Render::HealthBar> HealthBarMap;

	// aimbot data
	float MaxAimDistance = 100000;

	for (const auto& result : entities)
	{
		if (!result.isValid)
		{
			if (HealthBarMap.count(result.entity.Controller.Address))
				HealthBarMap.erase(result.entity.Controller.Address);
			continue;
		}

		const auto& entity = result.entity;
		const int entityIndex = result.entityIndex;

		// add entity to radar
		if (RadarCFG::ShowRadar && localEntity.Controller.TeamID != 0)
		{
			gameRadar.AddPoint(localEntity.Pawn.Pos, localEntity.Pawn.ViewAngle.y, 
				entity.Pawn.Pos, ImColor(237, 85, 106, 200), RadarCFG::RadarType, entity.Pawn.ViewAngle.y);
		}

		// Out-of-FOV arrow
		if (localEntity.IsAlive()) {
			ESP::RenderOutOfFOVArrow(localEntity, result.entity);
		}

        // skip not in screen
		if (!result.isInScreen)
		{
			continue;
		}

		// process aimbot data
		if (!AimControl::HitboxList.empty()) {
			float minDistance = FLT_MAX;
			Vec3 bestAimPos = { 0, 0, 0 };

			ImVec2 screenCenter{ Gui.Window.Size.x / 2, Gui.Window.Size.y / 2 };

			constexpr float DEG_TO_RAD = M_PI / 180.f;
			constexpr float STATIC_FOV = 90.0f;
			float halfWindowSize = Gui.Window.Size.x / 2.f;
			float staticFovTan = tan(STATIC_FOV * DEG_TO_RAD / 2.f);
			float aimFovTan = tan(AimControl::AimFov * DEG_TO_RAD / 2.f);
			float aimFovRadius = (aimFovTan / staticFovTan) * halfWindowSize;

            const auto& bones = entity.GetBone().BonePosList;
            if (!bones.empty()) {
                for (size_t i = 0; i < AimControl::HitboxList.size(); ++i) {
                    int hitboxID = AimControl::HitboxList[i];
                    if (hitboxID < 0 || static_cast<size_t>(hitboxID) >= bones.size()) {
                        continue;
                    }

                    float distanceToSight = bones[hitboxID].ScreenPos.DistanceTo(
                        { screenCenter.x, screenCenter.y });

                    if (distanceToSight < minDistance && distanceToSight <= aimFovRadius) {
                        minDistance = distanceToSight;

                        if (!LegitBotConfig::VisibleCheck ||
                            entity.Pawn.bSpottedByMask & (DWORD64(1) << (localPlayerControllerIndex)) ||
                            localEntity.Pawn.bSpottedByMask & (DWORD64(1) << (entityIndex))) {
                            Vec3 tempPos = bones[hitboxID].Pos;

                            bestAimPos = tempPos;
                            aimPosList.push_back(bestAimPos);
                            MaxAimDistance = distanceToSight;
                        }
                    }
                }
            }
		}

		// render esp
        if (false)
		{
			
		}
	}
}

void Menu() 
{
	if (MenuConfig::ShowMenu)
		GUI::DrawGui();

	GUI::InitHitboxList();
}

void Visual(const CEntity& LocalEntity)
{
	// Fov circle
	if (LocalEntity.Controller.TeamID != 0 && !MenuConfig::ShowMenu)
		Render::DrawFovCircle(ImGui::GetBackgroundDrawList(), LocalEntity);

	// Fov line
	Render::DrawFov(LocalEntity, LegitBotConfig::FovLineSize, LegitBotConfig::FovLineColor, 1);


        
}

void Radar(Base_Radar Radar, const CEntity& LocalEntity)
{
	// Radar render
	if ((RadarCFG::ShowRadar && LocalEntity.Controller.TeamID != 0) || (RadarCFG::ShowRadar && MenuConfig::ShowMenu))
	{
		Radar.Render();

		MenuConfig::RadarWinPos = ImGui::GetWindowPos();
		ImGui::End();
	}
}

void Trigger(const CEntity& LocalEntity, const int& LocalPlayerControllerIndex)
{
	// TriggerBot
	if (LegitBotConfig::TriggerBot && (GetAsyncKeyState(TriggerBot::HotKey) || LegitBotConfig::TriggerAlways))
		TriggerBot::Run(LocalEntity, LocalPlayerControllerIndex);
}

void AIM(const CEntity& LocalEntity, std::vector<Vec3> AimPosList)
{
	DWORD lastTick = 0;
	DWORD currentTick = GetTickCount64();

	if (!LegitBotConfig::AimBot) {
		RCS::RecoilControl(LocalEntity);
		return;
	}

	bool shouldAim = LegitBotConfig::AimAlways || GetAsyncKeyState(AimControl::HotKey);
	if (shouldAim && !AimPosList.empty())
		AimControl::AimBot(LocalEntity, LocalEntity.Pawn.CameraPos, AimPosList);

	if (LegitBotConfig::AimToggleMode && (GetAsyncKeyState(AimControl::HotKey) & 0x8000) &&
		currentTick - lastTick >= 200) {
		AimControl::switchToggle();
		lastTick = currentTick;
	}
}

void MiscFuncs(CEntity& LocalEntity)
{
    SpecList::SpectatorWindowList(LocalEntity);
    bmb::RenderWindow(LocalEntity.Controller.TeamID);
    
    

    Misc::HitManager(LocalEntity, PreviousTotalHits);
    Misc::BunnyHop(LocalEntity);
    Misc::Watermark(LocalEntity);
    Misc::AntiAFKKickUpdate();
    
}

void RadarSetting(Base_Radar& Radar)
{
	// Radar window
	ImGui::SetNextWindowBgAlpha(RadarCFG::RadarBgAlpha);
	ImGui::Begin("Radar", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
	ImGui::SetWindowSize({ RadarCFG::RadarRange * 2,RadarCFG::RadarRange * 2 });
	ImGui::SetWindowPos(MenuConfig::RadarWinPos, ImGuiCond_Once);

	if (MenuConfig::RadarWinChengePos)
	{
		ImGui::SetWindowPos("Radar", MenuConfig::RadarWinPos);
		MenuConfig::RadarWinChengePos = false;
	}

	if (!RadarCFG::customRadar)
	{
		RadarCFG::ShowRadarCrossLine = false;
		RadarCFG::Proportion = 2700.f;
		RadarCFG::RadarPointSizeProportion = 1.f;
		RadarCFG::RadarRange = 125.f;
		RadarCFG::RadarBgAlpha = 0.1f;
	}


	// Radar.SetPos({ Gui.Window.Size.x / 2,Gui.Window.Size.y / 2 });
	Radar.SetDrawList(ImGui::GetWindowDrawList());
	Radar.SetPos({ ImGui::GetWindowPos().x + RadarCFG::RadarRange, ImGui::GetWindowPos().y + RadarCFG::RadarRange });
	Radar.SetProportion(RadarCFG::Proportion);
	Radar.SetRange(RadarCFG::RadarRange);
	Radar.SetSize(RadarCFG::RadarRange * 2);
	Radar.SetCrossColor(RadarCFG::RadarCrossLineColor);

	Radar.ArcArrowSize *= RadarCFG::RadarPointSizeProportion;
	Radar.ArrowSize *= RadarCFG::RadarPointSizeProportion;
	Radar.CircleSize *= RadarCFG::RadarPointSizeProportion;

	Radar.ShowCrossLine = RadarCFG::ShowRadarCrossLine;
	Radar.Opened = true;
}

void RenderCrosshair(ImDrawList* drawList, const CEntity& LocalEntity)
{
	if (!MiscCFG::SniperCrosshair || LocalEntity.Controller.TeamID == 0 || MenuConfig::ShowMenu)
		return;

	bool isScoped;
	memoryManager.ReadMemory<bool>(LocalEntity.Pawn.Address + Offset.Pawn.isScoped, isScoped);
	std::string curWeapon = TriggerBot::GetWeapon(LocalEntity);

	if (!TriggerBot::CheckScopeWeapon(curWeapon) || isScoped)
		return;

	Render::DrawCrossHair(drawList, ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), MiscCFG::SniperCrosshairColor);
}

std::string Cheats::GetCurrentMapName() {
    if (!g_globalVars || !g_globalVars->g_cCurrentMap) {
        return "";
    }

    char currentMap[256] = { 0 };
    if (!memoryManager.ReadMemory(reinterpret_cast<DWORD64>(g_globalVars->g_cCurrentMap),
        currentMap, sizeof(currentMap) - 1)) {
        return "";
    }

    currentMap[255] = '\0';
    return std::string(currentMap);
}
