#pragma once
#include "../Game/Bone.h"
#include "../Game/Game.h"
#include <unordered_map>
#include <string>
#include <vector>

namespace MenuConfig
{
	inline std::string name = "Saturn";
	inline std::string version = "1.0.0";

	inline std::string path = "";
	inline std::string docPath = "";
	inline int RenderFPS = 1000;
	inline int AimDelay = 1;
	inline int BunnyHopDelay = 25;
    inline int RenderDistance = 1000;

	struct {
		// 0: Visual 1: Aimbot 2: Misc 3: Config
		int MenuPage = 0;
    ImVec2 MainWinSize = ImVec2(900.f, 600.f);
		ImVec2 LogoPos = ImVec2(0.f, 0.f);
		ImVec2 Button1Pos = ImVec2(0.f, 59.f);
		ImVec2 Button2Pos = ImVec2(0.f, 118.f);
		ImVec2 Button3Pos = ImVec2(0.f, 177.f);
		ImVec2 Button4Pos = ImVec2(0.f, 236.f);
		ImVec2 ChildPos = ImVec2(60.f, 0.f);
		ImVec2 ChildSize = ImVec2(540.f, 500.f);
	} WCS;	// Window Component Settings

	inline bool defaultConfig = false;

	// 0: Window 1: Collapse
	inline int WindowStyle = 0;
	inline bool ShowMenu = true;
	inline bool TeamCheck = true;
	inline bool BypassOBS = false;
	inline bool WorkInSpec = true;

	inline ImVec2 MarkWinPos;
	inline ImVec2 RadarWinPos;
	inline ImVec2 SpecWinPos;
	inline ImVec2 BombWinPos;
	inline bool MarkWinChengePos = false;
	inline bool RadarWinChengePos = false;
	inline bool SpecWinChengePos = false;
	inline bool BombWinChengePos = false;
	inline int HotKey = VK_END;
}

namespace LegitBotConfig
{
    inline bool AimBot = false;
	inline bool AimAlways = false;
	inline bool AimToggleMode = false;
	// 0: head 1: neck 3: spine
	inline int  AimPosition = 0;
	inline DWORD  AimPositionIndex = BONEINDEX::head;
    inline bool VisibleCheck = false;
	inline bool HitboxUpdated = false;
	inline bool ShowFovLine = false;
	inline ImColor FovCircleColor = ImColor(131, 137, 150, 180);
	inline ImColor FovLineColor = ImColor(0, 98, 98, 220);
	inline float FovLineSize = 60.f;

    inline bool TriggerBot = false;
	inline bool TriggerAlways = false;

	inline bool RCS = true;


}

namespace WeaponConfig
{
    struct WeaponProfile {
        bool aimEnabled = false;
        bool toggleMode = false;
        bool visibleCheck = true;
        bool scopeOnly = false;
        int humanizationStrength = 0;
        float aimFov = 5.f;
        float aimFovMin = 0.5f;
        float smooth = 2.f;
        int aimBullet = 0;
        std::vector<int> hitboxes{};
        bool triggerEnabled = false;
        bool autoMode = false;
        bool t_scopeOnly = false;
        bool stopOnly = false;
        bool ttdTimeout = false;
        int delay = 20;
        int duration = 200;
        bool rcsEnabled = true;
        int rcsBullet = 1;
        float rcsYaw = 1.f;
        float rcsPitch = 1.f;
    };
    inline std::unordered_map<std::string, WeaponProfile> WeaponConfigs;
    inline std::string SelectedWeaponKey = "";
}

namespace ESPConfig
{
    inline int HotKey = VK_RMENU;
	inline bool ESPenabled = true;
	inline bool AmmoBar = false;
	inline bool ShowBoneESP = true;
	inline bool ShowBoxESP = true;
	inline bool ShowHealthBar = true;
	inline bool ShowWeaponESP = true;
    
	inline bool ShowPlayerName = true;
	inline bool DrawFov = false;
	inline bool ShowDistance = false;
    
	inline bool ArmorBar = false;
    
	inline bool ShowHeadBox = true;
	inline bool ShowPreview = true;
	inline bool VisibleCheck = false;
	inline bool FilledBox = false;
	inline bool FilledVisBox = false;
	inline bool MultiColor = false;
	inline bool OutLine = true;
	inline bool ShowIsScoped = true;
	inline bool ShowIsBlind = false;
	inline bool FlashCheck = false;
	// 0: normal 1: Corner
	inline int BoxType = 0;
	inline float BoxRounding = 5;
	inline bool ShowLineToEnemy = false;
	inline ImColor LineToEnemyColor = ImColor(59, 71, 148, 180);
	// 0: Top 1: Center 2: Bottom
	inline int LinePos = 0;

	inline ImColor BoneColor = ImColor(131, 137, 150, 180);
	inline ImColor BoxColor = ImColor(59, 71, 148, 180);
    
	inline ImColor HeadBoxColor = ImColor(131, 137, 150, 180);
	inline ImColor VisibleColor = ImColor(59, 71, 148, 180);
	inline ImColor FilledColor = ImColor(59, 71, 148, 128);
	inline ImColor BoxFilledVisColor = ImColor(0, 98, 98, 128);
	inline ImColor FilledColor2 = ImColor(59, 71, 148, 128);

    inline bool ShowOutOfFOVArrow = false;
    inline float OutOfFOVRadiusFactor = 0.45f;
    inline ImColor OutOfFOVArrowColor = ImColor(255, 180, 50, 230);

    
}

//namespace CrosshairsCFG
//{
//	inline bool ShowCrossHair = false;
//
//	inline float CrossHairSize = 75;
//	inline bool drawDot = true;
//	inline bool drawCrossline = true;
//	inline bool tStyle = false;
//	inline bool drawCircle = false;
//	inline bool drawOutLine = true;
//	inline bool DynamicGap = false;
//	inline bool TeamCheck = true;
//
//	inline int crosshairPreset = 0;
//	inline int Gap = 8;
//	inline int HorizontalLength = 6;
//	inline int VerticalLength = 6;
//	inline int Thickness = 1.0f;
//	inline float DotSize = 1.0f;
//	inline float CircleRadius = 3.f;
//
//	inline bool isAim = false;
//	inline bool isJump = false;
//
//	inline ImColor CrossHairColor = ImColor(0, 255, 0, 255);
//	inline ImColor TargetedColor = ImColor(255, 0, 0, 255);
// 
// inline bool TargetingCrosshairs = false;
//}

namespace RadarCFG
{
	inline bool ShowRadar = false;
	inline float RadarRange = 125;
	inline float RadarPointSizeProportion = 1.f;
	inline bool ShowRadarCrossLine = false;
	inline ImColor RadarCrossLineColor = ImColor(131, 137, 150, 180);
	// 0: circle 1: arrow 2: circle with arrow
	inline int RadarType = 2;
	inline float Proportion = 2700.f;
	inline bool customRadar = false;
	inline float RadarBgAlpha = 0.1f;
}

namespace MiscCFG
{
    inline bool BunnyHop = false;
    inline bool WaterMark = true;
    inline bool SniperCrosshair = false; // removed feature
    inline ImColor SniperCrosshairColor = ImColor(32, 178, 170, 255);
    inline int	HitSound = 0;
    inline bool HitMarker = false;
    inline bool bmbTimer = true;
    inline bool SpecList = false;
    inline ImColor BombTimerCol = ImColor(131, 137, 150, 255);
    inline bool AutoAccept = false;
    inline bool AntiAFKKick = false;
}
