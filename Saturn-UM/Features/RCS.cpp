#include "RCS.h"
#include "../Helpers/Logger.h"
#include "../Core/DI.h"

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
    static Vec2 oldPunch{ 0.f, 0.f };
    static int lastShotsFired = 0;
    char buf[256];
    static DWORD lastResetLogTick = 0;

    Vec2 punch{};
    if (!GetPunchFromCache(LocalPlayer, punch)) {
        Vec2 ap = LocalPlayer.Pawn.AimPunchAngle;
        punch.x = ap.x;
        punch.y = ap.y;
    }

    int shotsFired = LocalPlayer.Pawn.ShotsFired;

    if (lastShotsFired == 0 && shotsFired > 0)
    {
        oldPunch = punch;
    }

    if (shotsFired <= 0)
    {
        oldPunch.x = 0.f;
        oldPunch.y = 0.f;
        lastShotsFired = shotsFired;
        return;
    }

    Vec2 viewAngles = LocalPlayer.Pawn.ViewAngle;

    Vec2 newAngles{
        viewAngles.x + oldPunch.x - punch.x * 2.f,
        viewAngles.y + oldPunch.y - punch.y * 2.f
    };

    if (newAngles.x > 89.f)  newAngles.x = 89.f;
    if (newAngles.x < -89.f) newAngles.x = -89.f;
    while (newAngles.y > 180.f)  newAngles.y -= 360.f;
    while (newAngles.y < -180.f) newAngles.y += 360.f;

    Vec2 delta = newAngles - viewAngles;

    float sens = LocalPlayer.Client.Sensitivity;
    constexpr float m_yaw = 0.022f;
    constexpr float m_pitch = 0.022f;

    delta.x *= RCSScale.y;
    delta.y *= RCSScale.x;

    Vec2 deltaPunch{ punch.x - oldPunch.x, punch.y - oldPunch.y };
    int mouseX = static_cast<int>(std::round((deltaPunch.y * 2.f * RCSScale.x) / (sens * m_yaw)));
    int mouseY = static_cast<int>(std::round((deltaPunch.x * 2.f * RCSScale.y) / (sens * m_pitch)));

    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
    {
        mouse_event(MOUSEEVENTF_MOVE, mouseX, -mouseY, 0, 0);
        oldPunch = punch;
    }

    lastShotsFired = shotsFired;

    static DWORD lastTick = GetTickCount64();
    static int fallbackCount = 0;
    DWORD now = GetTickCount64();
    if (now - lastTick >= 1000) { fallbackCount = 0; lastTick = now; }
}


void RCS::UpdateAngles(const CEntity& Local, Vec2& Angles)
{
    static Vec2 oldPunch{ 0.f, 0.f };
    char buf[256];

    int shotsFired = Local.Pawn.ShotsFired;

    if (shotsFired <= 0)
    {
        oldPunch.x = 0.f;
        oldPunch.y = 0.f;
        return;
    }

    static DWORD lastTick = GetTickCount64();
    static int fallbackCount = 0;
    Vec2 punch{};
    if (!GetPunchFromCache(Local, punch)) {
        Vec2 ap = Local.Pawn.AimPunchAngle;
        punch.x = ap.x;
        punch.y = ap.y;
        fallbackCount++;
    }

    Vec2 viewAngles = Local.Pawn.ViewAngle;

    Vec2 deltaPunch{ punch.x - oldPunch.x, punch.y - oldPunch.y };
    Angles.x -= deltaPunch.x * 2.f;
    Angles.y -= deltaPunch.y * 2.f;
    if (Angles.x > 89.f)  Angles.x = 89.f;
    if (Angles.x < -89.f) Angles.x = -89.f;
    while (Angles.y > 180.f)  Angles.y -= 360.f;
    while (Angles.y < -180.f) Angles.y += 360.f;
    oldPunch = punch;

    DWORD now = GetTickCount64();
    if (now - lastTick >= 1000) { fallbackCount = 0; lastTick = now; }
}
