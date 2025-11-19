#include "TriggerBot.h"
#include "Aimbot.h"
#include "Hitbox.h"
#include "../Game/Bone.h"
#include "../Core/EntityResult.h"
#include "../OS-ImGui/imgui/imgui.h"
#include <chrono>
#include <random>
#include <thread>
#include "../Geo/MapGeo.h"

void TriggerBot::Run(const CEntity& LocalEntity, const int& LocalPlayerControllerIndex)
{
    

    if (LocalEntity.Controller.AliveStatus == 0) { return; }

    // Get the entity under the crosshair
    DWORD uHandle = 0;
    if (!memoryManager.ReadMemory<DWORD>(LocalEntity.Pawn.Address + Offset.Pawn.iIDEntIndex, uHandle))
    {
        g_HasValidTarget = false;
        g_TargetFoundTime = std::chrono::system_clock::now();
        
        return;
    }

    if (uHandle == -1)
    {
        g_HasValidTarget = false;
        g_TargetFoundTime = std::chrono::system_clock::now();
        
        return;
    }

    DWORD64 PawnAddress = CEntity::ResolveEntityHandle(uHandle);
    if (PawnAddress == 0)
    {
        g_HasValidTarget = false;
        g_TargetFoundTime = std::chrono::system_clock::now();
        
        return;
    }

    CEntity targetEntity;
    if (!targetEntity.UpdatePawn(PawnAddress))
    {
        g_HasValidTarget = false;
        g_TargetFoundTime = std::chrono::system_clock::now();
        
        return;
    }

    // Validate the targeted entity
    if (!CanTrigger(LocalEntity, targetEntity, LocalPlayerControllerIndex))
    {
        g_HasValidTarget = false;
        g_TargetFoundTime = std::chrono::system_clock::now();
        
        return;
    }

    if (!g_HasValidTarget)
    {
        g_TargetFoundTime = std::chrono::system_clock::now();
    }
    g_HasValidTarget = true;

    auto now = std::chrono::system_clock::now();

    // calculate elapsed time
    long long timeSinceLastShot = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - g_LastShotTime).count();
    long long timeSinceTargetFound = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - g_TargetFoundTime).count();

    // check conditions to shoot using ActivationMode
    bool pressed = (GetAsyncKeyState(TriggerBot::HotKey) & 0x8000) != 0;
    bool shouldShoot = false;
    if (TriggerBot::ActivationMode == 2) {
        shouldShoot = true;
    } else if (TriggerBot::ActivationMode == 1) {
        shouldShoot = TriggerBot::ToggledActive;
    } else {
        shouldShoot = pressed;
    }
    
    if (shouldShoot &&
        timeSinceLastShot >= ShotDuration &&
        timeSinceTargetFound >= TriggerDelay)
    { ExecuteShot(); }
    
}

bool TriggerBot::CanTrigger(const CEntity& LocalEntity, const CEntity& TargetEntity, const int& LocalPlayerControllerIndex)
{
    // Check if target is in a valid state
    if (TargetEntity.Pawn.Address == 0) { return false; }

    // Check team
    if (MenuConfig::TeamCheck && LocalEntity.Pawn.TeamID == TargetEntity.Pawn.TeamID) { return false; }

    // Check if weapon is ready
    bool waitForNoAttack = false;
    if (!memoryManager.ReadMemory<bool>(LocalEntity.Pawn.Address + Offset.Pawn.m_bWaitForNoAttack, waitForNoAttack)) { return false; }

    if (waitForNoAttack) { return false; }

    // Check weapon type
    std::string currentWeapon = GetWeapon(LocalEntity);
    if (!CheckWeapon(currentWeapon)) { return false; }

    //check is velocity == 0
    if(StopedOnly && LocalEntity.Pawn.Speed != 0) { return false; }

    

    // Check TTD timout
    DWORD64 playerMask = (DWORD64(1) << LocalPlayerControllerIndex);
    bool bIsVisible;
    if (LegitBotConfig::VisibleCheck && gMapGeo.IsReady()) {
        bool los = gMapGeo.RaycastLOS(LocalEntity.Pawn.CameraPos, TargetEntity.Pawn.Pos);
        bool maskVis = (TargetEntity.Pawn.bSpottedByMask & playerMask) || (LocalEntity.Pawn.bSpottedByMask & playerMask);
        bIsVisible = TriggerBot::IgnoreSmoke ? los : (los && maskVis);
    } else {
        bIsVisible = (TargetEntity.Pawn.bSpottedByMask & playerMask) || (LocalEntity.Pawn.bSpottedByMask & playerMask);
    }
    if (TTDtimeout && !bIsVisible) { return false; }

    // Check scope requirement
    if (ScopeOnly && CheckScopeWeapon(currentWeapon))
    {
        bool isScoped = false;
        memoryManager.ReadMemory<bool>(LocalEntity.Pawn.Address + Offset.Pawn.isScoped, isScoped);
        if (!isScoped) { return false; }
    }

    return true;
}

void TriggerBot::ExecuteShot()
{
    // Check if already shooting to avoid double-click
    if (GetAsyncKeyState(VK_LBUTTON) < 0)
        return;

    // Update timing
    g_LastShotTime = std::chrono::system_clock::now();

    // Execute shot with random timing
    std::random_device RandomDevice;
    std::mt19937 RandomNumber(RandomDevice());
    std::uniform_int_distribution<> Range(1, 5);
    auto rand = std::chrono::microseconds(Range(RandomNumber));

    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    std::this_thread::sleep_for(std::chrono::microseconds(Range(RandomNumber)));
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

std::string TriggerBot::GetWeapon(const CEntity& LocalEntity)
{
    // Single memory read to get the weapon pointer
    DWORD64 CurrentWeapon;
    if (!memoryManager.ReadMemory(LocalEntity.Pawn.Address + Offset.Pawn.pClippingWeapon, CurrentWeapon) || CurrentWeapon == 0)
        return "";

    // Calculate the final address for weapon index directly
    DWORD64 weaponIndexAddress = CurrentWeapon + Offset.EconEntity.AttributeManager +
        Offset.WeaponBaseData.Item + Offset.WeaponBaseData.ItemDefinitionIndex;

    // Single memory read to get weapon index
    short weaponIndex;
    if (!memoryManager.ReadMemory(weaponIndexAddress, weaponIndex) || weaponIndex == -1)
        return "";

    // Inline weapon name lookup
    static const std::string defaultWeapon = "";
    auto it = CEntity::weaponNames.find(weaponIndex);
    return (it != CEntity::weaponNames.end()) ? it->second : defaultWeapon;
}

bool TriggerBot::CheckScopeWeapon(const std::string& WeaponName)
{
    return (WeaponName == "awp" || WeaponName == "g3Sg1" || WeaponName == "ssg08" || WeaponName == "scar20");
}

bool TriggerBot::CheckWeapon(const std::string& WeaponName)
{
    return !(WeaponName == "smokegrenade" || WeaponName == "flashbang" || WeaponName == "hegrenade" ||
        WeaponName == "molotov" || WeaponName == "decoy" || WeaponName == "incgrenade" ||
        WeaponName == "t_knife" || WeaponName == "ct_knife" || WeaponName == "c4");
}

static inline bool withinFovPixels(const ImVec2& center, const ImVec2& p, int radius)
{
    float dx = p.x - center.x;
    float dy = p.y - center.y;
    return (dx * dx + dy * dy) <= (float)(radius * radius);
}

static inline bool boneHitInFov(const CEntity& e, int radius, const std::vector<int>& hitboxes, ImVec2 center)
{
    return Hitbox::AnyInFov(e, radius, hitboxes, center);
}

void TriggerBot::RunEnhanced(const CEntity& LocalEntity, int LocalPlayerControllerIndex, const std::vector<EntityResult>& entities)
{
    if (LocalEntity.Controller.AliveStatus == 0) { return; }

    DWORD uHandle = 0;
    bool haveHandle = memoryManager.ReadMemory<DWORD>(LocalEntity.Pawn.Address + Offset.Pawn.iIDEntIndex, uHandle) && uHandle != (DWORD)-1;
    CEntity targetEntity;
    bool targetValid = false;
    if (haveHandle) {
        DWORD64 PawnAddress = CEntity::ResolveEntityHandle(uHandle);
        if (PawnAddress && targetEntity.UpdatePawn(PawnAddress)) {
            targetValid = true;
        }
    }

    ImVec2 center{ ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f };
    std::vector<int> hbSel = Hitbox::TriggerList();
    int radius = TriggerBot::FovPixels;

    auto canShoot = [&](const CEntity& tgt)->bool{
        if (tgt.Pawn.Address == 0) return false;
        if (MenuConfig::TeamCheck && LocalEntity.Pawn.TeamID == tgt.Pawn.TeamID) return false;
        bool waitForNoAttack = false;
        if (!memoryManager.ReadMemory<bool>(LocalEntity.Pawn.Address + Offset.Pawn.m_bWaitForNoAttack, waitForNoAttack)) return false;
        if (waitForNoAttack) return false;
        std::string currentWeapon = GetWeapon(LocalEntity);
        if (!CheckWeapon(currentWeapon)) return false;
        if (StopedOnly && LocalEntity.Pawn.Speed != 0) return false;
        DWORD64 playerMask = (DWORD64(1) << LocalPlayerControllerIndex);
        bool bIsVisible;
        if (LegitBotConfig::VisibleCheck && gMapGeo.IsReady()) {
            bool los = gMapGeo.RaycastLOS(LocalEntity.Pawn.CameraPos, tgt.Pawn.Pos);
            bool maskVis = (tgt.Pawn.bSpottedByMask & playerMask) || (LocalEntity.Pawn.bSpottedByMask & playerMask);
            bIsVisible = TriggerBot::IgnoreSmoke ? los : (los && maskVis);
        } else {
            bIsVisible = (tgt.Pawn.bSpottedByMask & playerMask) || (LocalEntity.Pawn.bSpottedByMask & playerMask);
        }
        if (TTDtimeout && !bIsVisible) return false;
        if (ScopeOnly && CheckScopeWeapon(currentWeapon)) {
            bool isScoped = false;
            memoryManager.ReadMemory<bool>(LocalEntity.Pawn.Address + Offset.Pawn.isScoped, isScoped);
            if (!isScoped) return false;
        }
        return true;
    };

    bool inFov = false;
    if (targetValid) {
        inFov = Hitbox::RayHits(LocalEntity, targetEntity, hbSel) && canShoot(targetEntity);
    }

    if (!inFov) {
        const CEntity* best = nullptr;
        float bestDist2 = (float)(radius * radius);
        for (const auto& r : entities) {
            if (!r.isValid) continue;
            const CEntity& e = r.entity;
            if (!canShoot(e)) continue;
            const auto& bones = e.GetBone().BonePosList;
            if (bones.empty()) continue;
            if (Hitbox::RayHits(LocalEntity, e, hbSel)) { best = &e; break; }
        }
        if (best) {
            targetEntity = *best;
            inFov = true;
        }
    }

    static bool g_isHolding = false;
    static ULONGLONG g_lastInFovTick = 0;
    ULONGLONG nowTick = GetTickCount64();
    if (!inFov) {
        bool continuous = (TriggerDelay == 0);
        if (continuous && g_isHolding) {
            if (nowTick - g_lastInFovTick <= 120) {
                return;
            }
        }
        if (g_isHolding) { mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0); g_isHolding = false; }
        g_HasValidTarget = false;
        g_TargetFoundTime = std::chrono::system_clock::now();
        return;
    }
    g_lastInFovTick = nowTick;

    if (!g_HasValidTarget) g_TargetFoundTime = std::chrono::system_clock::now();
    g_HasValidTarget = true;

    auto now = std::chrono::system_clock::now();
    long long timeSinceLastShot = std::chrono::duration_cast<std::chrono::milliseconds>(now - g_LastShotTime).count();
    long long timeSinceTargetFound = std::chrono::duration_cast<std::chrono::milliseconds>(now - g_TargetFoundTime).count();

    bool pressed = (GetAsyncKeyState(TriggerBot::HotKey) & 0x8000) != 0;
    bool shouldShoot = false;
    if (TriggerBot::ActivationMode == 2) shouldShoot = true;
    else if (TriggerBot::ActivationMode == 1) shouldShoot = TriggerBot::ToggledActive;
    else shouldShoot = pressed;
    bool continuous = (TriggerDelay == 0);
    if (shouldShoot) {
        if (continuous) {
            if (!g_isHolding) { mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0); g_isHolding = true; }
        } else {
            if (timeSinceLastShot >= ShotDuration && timeSinceTargetFound >= TriggerDelay) {
                ExecuteShot();
            }
        }
    } else {
        if (g_isHolding) { mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0); g_isHolding = false; }
    }
}
