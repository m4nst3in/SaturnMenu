#include "RCS.h"
#include "../Helpers/Logger.h"
#include "../Core/DI.h"
#include "../Core/Config.h"
#include "../Game/Weapon.h"

namespace
{
    bool GetPunchFromCache(const CEntity& local, Vec2& outPunch)
    {
        auto count = local.Pawn.AimPunchCache.Count;
        if (count <= 0 || count > 128)
            return false;

        Vec3 lastPunch{};
        if (!memoryManager.ReadMemory(
            local.Pawn.AimPunchCache.Data + (count - 1) * sizeof(Vec3),
            lastPunch))
            return false;

        outPunch.x = lastPunch.x;
        outPunch.y = lastPunch.y;
        return true;
    }
}

void RCS::RecoilControl(const CEntity& LocalPlayer)
{
    if (!LegitBotConfig::RCS)
        return;
    static Vec2 oldPunch{ 0.f, 0.f };
    static int lastShotsFired = 0;
    static bool prevMouseDown = false;

    std::string weaponName = Weapon::GetWeapon(LocalPlayer);
    WeaponRCSConfig defaultConfig = GetWeaponRCSConfig(weaponName);

    float yaw = defaultConfig.yaw;
    float pitch = defaultConfig.pitch;

    auto it = WeaponConfig::WeaponConfigs.find(weaponName);
    if (it != WeaponConfig::WeaponConfigs.end())
    {
        const auto& userCfg = it->second;
        if (userCfg.rcsYaw > 0.0f)   yaw   = userCfg.rcsYaw;
        if (userCfg.rcsPitch > 0.0f) pitch = userCfg.rcsPitch;
    }

    Vec2 punch{};
    if (!GetPunchFromCache(LocalPlayer, punch)) {
        Vec2 ap = LocalPlayer.Pawn.AimPunchAngle;
        punch.x = ap.x;
        punch.y = ap.y;
    }

    int shotsFired = LocalPlayer.Pawn.ShotsFired;
    bool mouseDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

    if (mouseDown && !prevMouseDown)
        oldPunch = punch;
    if (!mouseDown && shotsFired <= 0)
    {
        oldPunch.x = 0.f; oldPunch.y = 0.f;
        lastShotsFired = shotsFired;
        prevMouseDown = mouseDown;
        return;
    }

    float sens = LocalPlayer.Client.Sensitivity;
    constexpr float m_yaw = 0.022f;
    constexpr float m_pitch = 0.022f;

    // calculating the control using the deltaPunch.xy * 2.f * yaw/pitch formula
    if (mouseDown && shotsFired >= RCS::RCSBullet)
    {
        Vec2 deltaPunch{ punch.x - oldPunch.x, punch.y - oldPunch.y };
        int mouseX = static_cast<int>(std::round((deltaPunch.y * 2.f * yaw) / (sens * m_yaw)));
        int mouseY = static_cast<int>(std::round((deltaPunch.x * 2.f * pitch) / (sens * m_pitch)));
        mouse_event(MOUSEEVENTF_MOVE, mouseX, -mouseY, 0, 0);
        oldPunch = punch;
    }

    lastShotsFired = shotsFired;
    prevMouseDown = mouseDown;
}

void RCS::UpdateAngles(const CEntity& Local, Vec2& Angles)
{
    static Vec2 oldPunch{ 0.f, 0.f };

    std::string weaponName = Weapon::GetWeapon(Local);
    WeaponRCSConfig defaultConfig = GetWeaponRCSConfig(weaponName);

    // here we gonna read yaw and pitch from the config
    float yaw = defaultConfig.yaw;
    float pitch = defaultConfig.pitch;

    auto it = WeaponConfig::WeaponConfigs.find(weaponName);
    if (it != WeaponConfig::WeaponConfigs.end())
    {
        const auto& userCfg = it->second;
        if (userCfg.rcsYaw > 0.0f)   yaw   = userCfg.rcsYaw;
        if (userCfg.rcsPitch > 0.0f) pitch = userCfg.rcsPitch;
    }

    int shotsFired = Local.Pawn.ShotsFired;
    bool mouseDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    if (!mouseDown || shotsFired < RCS::RCSBullet) {
        oldPunch.x = 0.f; oldPunch.y = 0.f;
        return;
    }

    Vec2 punch{};
    if (!GetPunchFromCache(Local, punch)) {
        Vec2 ap = Local.Pawn.AimPunchAngle;
        punch.x = ap.x;
        punch.y = ap.y;
    }

    Vec2 deltaPunch{ punch.x - oldPunch.x, punch.y - oldPunch.y };

    Angles.x -= deltaPunch.x * 2.f * pitch;
    Angles.y -= deltaPunch.y * 2.f * yaw;

    if (Angles.x > 89.f)  Angles.x = 89.f;
    if (Angles.x < -89.f) Angles.x = -89.f;
    while (Angles.y > 180.f)  Angles.y -= 360.f;
    while (Angles.y < -180.f) Angles.y += 360.f;

    oldPunch = punch;
}
