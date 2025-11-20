#pragma once
#define _USE_MATH_DEFINES
#define MAXV 10000e9
#include <math.h>
#include <thread>
#include <chrono>
#include "..\Game\Game.h"
#include "..\Game\Entity.h"
#include "..\Core\Config.h"
#include <iostream>
#include "..\Game\View.h"
#include "..\Features/RCS.h"
#include <random>

extern "C" {
#include "..\Helpers\Mouse.h"
}

namespace AimControl
{
    inline int HotKey = VK_XBUTTON2;
    inline int ActivationMode = 0;
    inline bool ToggledActive = false;
    inline int AimBullet = 0;
    inline bool ScopeOnly = false;
    inline bool HumanizeVar = true;
    inline int HumanizationStrength = 0;
    inline float AimFov = 10;
    inline float AimFovMin = 0.4f;
    inline float Smooth = 5.0f;
    inline std::vector<int> HitboxList{ BONEINDEX::head };
    
    inline bool HasTarget = false;
    inline DWORD LockedTargetId = 0;

    static float PrevTargetX = 0.0f;
    static float PrevTargetY = 0.0f;
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::pair<float, float> Humanize(float TargetX, float TargetY);

    void AimBot(const CEntity& Local, std::vector<std::pair<int, CEntity>>& EntityList);
    void switchToggle();
    std::pair<float, float> CalculateTargetOffset(const Vec2& ScreenPos, int ScreenCenterX, int ScreenCenterY);
    bool CheckAutoMode(const std::string& WeaponName);
}
