#pragma once
#include <chrono>

#include "../Game/Game.h"
#include "../Game/Entity.h"
#include "../Core/Config.h"
#include "../Core/EntityResult.h"
#include <vector>

namespace TriggerBot
{
    // Configuration
    inline int TriggerDelay = 10; // ms
    inline int ShotDuration = 400; // ms
    inline bool ScopeOnly = false;
    
    inline bool StopedOnly = false;
    inline bool TTDtimeout = false;
    inline bool VisibleCheck = true;
    inline int FovPixels = 18;

    // Input configuration
    inline int HotKey = VK_XBUTTON1;
    inline int ActivationMode = 0;
    inline bool ToggledActive = false;

    // Timing variables
	inline std::chrono::time_point<std::chrono::system_clock> g_LastShotTime;
    inline std::chrono::time_point<std::chrono::system_clock> g_TargetFoundTime;
	inline bool g_HasValidTarget = false;

    // Main functions
    void Run(const CEntity& LocalEntity, const int& LocalPlayerControllerIndex);
    void RunEnhanced(const CEntity& LocalEntity, int LocalPlayerControllerIndex, const std::vector<EntityResult>& entities);

    // Validation functions
    bool CanTrigger(const CEntity& LocalEntity, const CEntity& TargetEntity, const int& LocalPlayerControllerIndex);

    // Execution functions
	void ExecuteShot();

    // Utility functions
	std::string GetWeapon(const CEntity& LocalEntity);
	bool CheckWeapon(const std::string& WeaponName);
	bool CheckScopeWeapon(const std::string& WeaponName);
}
