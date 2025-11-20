#include "TriggerBot.h"
#include "Aimbot.h"
#include "Hitbox.h"
#include "../Game/Bone.h"
#include "../Core/EntityResult.h"
#include "../Core/Cheats.h"
#include "../OS-ImGui/imgui/imgui.h"
#include <algorithm>
#include <chrono>
#include <random>
#include <thread>
#include "../Geo/MapGeo.h"

void TriggerBot::Run(const CEntity& LocalEntity, const int& LocalPlayerControllerIndex)
{
    if (LocalEntity.Controller.AliveStatus == 0)
        return;

    DWORD uHandle = 0;
    if (!memoryManager.ReadMemory<DWORD>(LocalEntity.Pawn.Address + Offset.Pawn.iIDEntIndex, uHandle)) {
        g_HasValidTarget = false;
        g_TargetFoundTime = std::chrono::system_clock::now();
        return;
    }
    if (uHandle == -1) {
        g_HasValidTarget = false;
        g_TargetFoundTime = std::chrono::system_clock::now();
        return;
    }
    DWORD64 PawnAddress = CEntity::ResolveEntityHandle(uHandle);
    if (PawnAddress == 0) {
        g_HasValidTarget = false;
        g_TargetFoundTime = std::chrono::system_clock::now();
        return;
    }
    CEntity targetEntity;
    if (!targetEntity.UpdatePawn(PawnAddress)) {
        g_HasValidTarget = false;
        g_TargetFoundTime = std::chrono::system_clock::now();
        return;
    }
    if (!CanTrigger(LocalEntity, targetEntity, LocalPlayerControllerIndex)) {
        g_HasValidTarget = false;
        g_TargetFoundTime = std::chrono::system_clock::now();
        return;
    }
    if (!g_HasValidTarget) {
        g_TargetFoundTime = std::chrono::system_clock::now();
    }
    g_HasValidTarget = true;
    auto now = std::chrono::system_clock::now();

    long long timeSinceLastShot = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - g_LastShotTime).count();
    long long timeSinceTargetFound = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - g_TargetFoundTime).count();

    bool pressed = (GetAsyncKeyState(TriggerBot::HotKey) & 0x8000) != 0;
    bool shouldShoot = false;
    if (TriggerBot::ActivationMode == 2) shouldShoot = true;
    else if (TriggerBot::ActivationMode == 1) shouldShoot = TriggerBot::ToggledActive;
    else shouldShoot = pressed;

    if (shouldShoot &&
        timeSinceLastShot >= ShotDuration &&
        timeSinceTargetFound >= TriggerDelay)
    {
        ExecuteShot();
    }
}

bool TriggerBot::CanTrigger(const CEntity& LocalEntity, const CEntity& TargetEntity, const int& LocalPlayerControllerIndex)
{
    if (TargetEntity.Pawn.Address == 0) return false;
    if (!Cheats::IsFFA() && LocalEntity.Pawn.TeamID == TargetEntity.Pawn.TeamID) return false;

    bool waitForNoAttack = false;
    if (!memoryManager.ReadMemory<bool>(LocalEntity.Pawn.Address + Offset.Pawn.m_bWaitForNoAttack, waitForNoAttack)) return false;
    if (waitForNoAttack) return false;
    std::string currentWeapon = GetWeapon(LocalEntity);
    if (!CheckWeapon(currentWeapon)) return false;
    if (StopedOnly && LocalEntity.Pawn.Speed != 0) return false;

    // Visibilidade e local varável = true (raytrace + mask)
    DWORD64 playerMask = (DWORD64(1) << (LocalPlayerControllerIndex & 0x3F));
    bool maskVis = (TargetEntity.Pawn.bSpottedByMask & playerMask) || (LocalEntity.Pawn.bSpottedByMask & playerMask);
    bool los = false;
    if (gMapGeo.IsReady())
        los = gMapGeo.RaycastLOS(LocalEntity.Pawn.CameraPos, TargetEntity.Pawn.Pos);
    else
        los = true; // fallback: se não disponível, não bloqueia

    bool visFinal = (IgnoreSmoke) ? (los || maskVis) : (los && maskVis);
    if (!visFinal) return false;
    if (ScopeOnly && CheckScopeWeapon(currentWeapon)) {
        bool isScoped = false;
        memoryManager.ReadMemory<bool>(LocalEntity.Pawn.Address + Offset.Pawn.isScoped, isScoped);
        if (!isScoped) return false;
    }
    return true;
}

void TriggerBot::ExecuteShot()
{
    if (GetAsyncKeyState(VK_LBUTTON) < 0) // já está segurando, não atira de novo
        return;

    g_LastShotTime = std::chrono::system_clock::now();

    std::random_device RandomDevice;
    std::mt19937 RandomNumber(RandomDevice());
    std::uniform_int_distribution<> Range(1, 5);

    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    std::this_thread::sleep_for(std::chrono::microseconds(Range(RandomNumber)));
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

std::string TriggerBot::GetWeapon(const CEntity& LocalEntity)
{
    DWORD64 CurrentWeapon;
    if (!memoryManager.ReadMemory(LocalEntity.Pawn.Address + Offset.Pawn.pClippingWeapon, CurrentWeapon) || CurrentWeapon == 0)
        return "";
    DWORD64 weaponIndexAddress = CurrentWeapon + Offset.EconEntity.AttributeManager +
        Offset.WeaponBaseData.Item + Offset.WeaponBaseData.ItemDefinitionIndex;
    short weaponIndex;
    if (!memoryManager.ReadMemory(weaponIndexAddress, weaponIndex) || weaponIndex == -1)
        return "";
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

void TriggerBot::RunEnhanced(const CEntity& LocalEntity, int LocalPlayerControllerIndex, const std::vector<EntityResult>& entities)
{
    if (LocalEntity.Controller.AliveStatus == 0)
        return;

    bool pressed = (GetAsyncKeyState(TriggerBot::HotKey) & 0x8000) != 0;
    bool shouldShoot = false;
    if (TriggerBot::ActivationMode == 2) shouldShoot = true;
    else if (TriggerBot::ActivationMode == 1) shouldShoot = TriggerBot::ToggledActive;
    else shouldShoot = pressed;

    if (!shouldShoot) {
        if (GetAsyncKeyState(VK_LBUTTON) < 0)
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        g_HasValidTarget = false;
        return;
    }

    ImVec2 center{ ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f };
    int pixelRadius = (TriggerBot::FovPixels > 8) ? TriggerBot::FovPixels : 8;
    std::vector<int> hitboxes = AimControl::HitboxList.empty() ? std::vector<int>{BONEINDEX::head} : AimControl::HitboxList;

    const CEntity* bestEnt = nullptr;
    float bestDist = pixelRadius + 1;

    for (const auto& r : entities)
    {
        if (!r.isValid) continue;
        const CEntity& e = r.entity;
        if (!Cheats::IsFFA() && LocalEntity.Pawn.TeamID == e.Pawn.TeamID) continue;
        if (e.Pawn.Health <= 0) continue;
        const auto& bones = e.GetBone().BonePosList;
        for (int hitbox : hitboxes) {
            if (hitbox < 0 || static_cast<size_t>(hitbox) >= bones.size()) continue;
            const auto& bone = bones[hitbox];
            float dist = std::hypot(bone.ScreenPos.x - center.x, bone.ScreenPos.y - center.y);
            if (dist > pixelRadius || dist >= bestDist) continue;

            DWORD64 maskLocal = (DWORD64(1) << (LocalPlayerControllerIndex & 0x3F));
            bool maskVis = (e.Pawn.bSpottedByMask & maskLocal) || (LocalEntity.Pawn.bSpottedByMask & maskLocal);

            bool los = false;
            if (gMapGeo.IsReady())
                los = gMapGeo.RaycastLOS(LocalEntity.Pawn.CameraPos, bone.Pos);

            bool visFinal = (IgnoreSmoke) ? (los || maskVis) : (los && maskVis);
            if (!visFinal) continue;

            if (!CanTrigger(LocalEntity, e, LocalPlayerControllerIndex)) continue;

            bestEnt = &e;
            bestDist = dist;
        }
        if (bestEnt) break;
    }

    static std::chrono::time_point<std::chrono::system_clock> lastFireTick;
    long long sinceLastFire = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastFireTick).count();
    if (bestEnt) {
        if (sinceLastFire >= ShotDuration)
        {
            ExecuteShot();
            lastFireTick = std::chrono::system_clock::now();
            g_HasValidTarget = true;
            g_TargetFoundTime = std::chrono::system_clock::now();
        }
    } else {
        g_HasValidTarget = false;
        if (GetAsyncKeyState(VK_LBUTTON) < 0)
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
    }
}
