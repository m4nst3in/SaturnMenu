#pragma once
#include "..\Core\Config.h"
#include "..\Core\Render.h"
#include "..\Core\Init.h"
#include "..\Features\Aimbot.h"
#include "..\Features\Radar.h"
#include "..\Features\Misc.h"
#include "..\Features\TriggerBot.h"
#include "..\Config\ConfigMenu.h"
#include "..\Config\ConfigSaver.h"

#include "..\Resources\Language.h"
#include <unordered_map>
#include "..\Resources\Images.h"
#if __has_include("..\Resources\MenuIcon.h")
#include "..\Resources\MenuIcon.h"
#endif
#include "../Helpers/KeyManager.h"
#include "..\OS-ImGui\OS-ImGui_Base.h"
// Ícones FontAwesome nas labels
#include "..\Resources\IconsFontAwesome.h"

#include "../Features/ESP.h"
#include <string>

inline ID3D11ShaderResourceView* Logo = NULL;
inline ID3D11ShaderResourceView* MenuButton1 = NULL;
inline ID3D11ShaderResourceView* MenuButton2 = NULL;
inline ID3D11ShaderResourceView* MenuButton3 = NULL;
inline ID3D11ShaderResourceView* MenuButton4 = NULL;
inline ID3D11ShaderResourceView* MenuButton1Pressed = NULL;
inline ID3D11ShaderResourceView* MenuButton2Pressed = NULL;
inline ID3D11ShaderResourceView* MenuButton3Pressed = NULL;
inline ID3D11ShaderResourceView* MenuButton4Pressed = NULL;
inline ID3D11ShaderResourceView* HitboxImage = NULL;
inline ID3D11ShaderResourceView* MenuTitleIcon = NULL;
inline int MenuTitleIconW = 0, MenuTitleIconH = 0;
inline void SetMenuTitleIcon(const unsigned char* data, unsigned int size);


inline bool Button1Pressed = true;
inline bool Button2Pressed = false;
inline bool Button3Pressed = false;
inline bool Button4Pressed = false;

inline int LogoW = 0, LogoH = 0;
inline int buttonW = 0;
inline int buttonH = 0;
inline int hitboxW = 0, hitboxH = 0;

// checkbox for hitbox
inline bool checkbox1 = true;
inline bool checkbox2 = false;
inline bool checkbox3 = false;
inline bool checkbox4 = false;
inline bool checkbox5 = false;

namespace GUI
{
    struct VM_ESP {
        bool enabled;
        bool showBox;
        bool outline;
        bool isBoxStyle;
        bool showBone;
        bool showOutOfFOV;
        float outOfFOVRadius;
        bool showHealthBar;
        bool armorBar;
        bool showWeapon;
        bool ammoBar;
        bool showDistance;
        bool showPlayerName;
        bool showScoped;
        bool visibleCheck;
    };
    inline VM_ESP BuildVM_ESP() {
        VM_ESP vm{};
        vm.enabled = ESPConfig::ESPenabled;
        vm.showBox = ESPConfig::ShowBoxESP;
        vm.outline = ESPConfig::OutLine;
        vm.isBoxStyle = (ESPConfig::BoxType == 0);
        vm.showBone = ESPConfig::ShowBoneESP;
        vm.showOutOfFOV = ESPConfig::ShowOutOfFOVArrow;
        vm.outOfFOVRadius = ESPConfig::OutOfFOVRadiusFactor;
        vm.showHealthBar = ESPConfig::ShowHealthBar;
        vm.armorBar = ESPConfig::ArmorBar;
        vm.showWeapon = ESPConfig::ShowWeaponESP;
        vm.ammoBar = ESPConfig::AmmoBar;
        vm.showDistance = ESPConfig::ShowDistance;
        vm.showPlayerName = ESPConfig::ShowPlayerName;
        vm.showScoped = ESPConfig::ShowIsScoped;
        vm.visibleCheck = ESPConfig::VisibleCheck;
        return vm;
    }
    inline void ApplyVM_ESP(const VM_ESP& vm) {
        ESPConfig::ESPenabled = vm.enabled;
        ESPConfig::ShowBoxESP = vm.showBox;
        ESPConfig::OutLine = vm.outline;
        ESPConfig::BoxType = vm.isBoxStyle ? 0 : 1;
        ESPConfig::ShowBoneESP = vm.showBone;
        ESPConfig::ShowOutOfFOVArrow = vm.showOutOfFOV;
        ESPConfig::OutOfFOVRadiusFactor = vm.outOfFOVRadius;
        ESPConfig::ShowHealthBar = vm.showHealthBar;
        ESPConfig::ArmorBar = vm.armorBar;
        ESPConfig::ShowWeaponESP = vm.showWeapon;
        ESPConfig::AmmoBar = vm.ammoBar;
        ESPConfig::ShowDistance = vm.showDistance;
        ESPConfig::ShowPlayerName = vm.showPlayerName;
        ESPConfig::ShowIsScoped = vm.showScoped;
        ESPConfig::VisibleCheck = vm.visibleCheck;
    }

    struct VM_Trigger {
        bool enabled;
        bool scopeOnly;
        bool stopOnly;
        bool ttdTimeout;
        int delay;
        int duration;
    };
    inline VM_Trigger BuildVM_Trigger() {
        VM_Trigger vm{};
        if (!WeaponConfig::SelectedWeaponKey.empty()) {
            auto key = WeaponConfig::SelectedWeaponKey;
            auto it = WeaponConfig::WeaponConfigs.find(key);
            if (it == WeaponConfig::WeaponConfigs.end()) {
                WeaponConfig::WeaponProfile p;
                p.triggerEnabled = LegitBotConfig::TriggerBot;
                p.trigActivationMode = TriggerBot::ActivationMode;
                p.t_scopeOnly = TriggerBot::ScopeOnly;
                p.stopOnly = TriggerBot::StopedOnly;
                p.ttdTimeout = TriggerBot::TTDtimeout;
                p.delay = TriggerBot::TriggerDelay;
                p.duration = TriggerBot::ShotDuration;
                WeaponConfig::WeaponConfigs[key] = p;
                it = WeaponConfig::WeaponConfigs.find(key);
            }
            const auto& p = it->second;
            vm.enabled = p.triggerEnabled;
            vm.scopeOnly = p.t_scopeOnly;
            vm.stopOnly = p.stopOnly;
            vm.ttdTimeout = p.ttdTimeout;
            vm.delay = p.delay;
            vm.duration = p.duration;
        } else {
            vm.enabled = LegitBotConfig::TriggerBot;
            vm.scopeOnly = TriggerBot::ScopeOnly;
            vm.stopOnly = TriggerBot::StopedOnly;
            vm.ttdTimeout = TriggerBot::TTDtimeout;
            vm.delay = TriggerBot::TriggerDelay;
            vm.duration = TriggerBot::ShotDuration;
        }
        return vm;
    }
    inline void ApplyVM_Trigger(const VM_Trigger& vm) {
        if (!WeaponConfig::SelectedWeaponKey.empty()) {
            auto& p = WeaponConfig::WeaponConfigs[WeaponConfig::SelectedWeaponKey];
            p.triggerEnabled = vm.enabled;
            p.t_scopeOnly = vm.scopeOnly;
            p.stopOnly = vm.stopOnly;
            p.ttdTimeout = vm.ttdTimeout;
            p.delay = vm.delay;
            p.duration = vm.duration;
        } else {
            LegitBotConfig::TriggerBot = vm.enabled;
            TriggerBot::ScopeOnly = vm.scopeOnly;
            TriggerBot::StopedOnly = vm.stopOnly;
            TriggerBot::TTDtimeout = vm.ttdTimeout;
            TriggerBot::TriggerDelay = vm.delay;
            TriggerBot::ShotDuration = vm.duration;
        }
    }
    struct VM_RCS {
        bool enabled;
        int bullet;
        float yaw;
        float pitch;
    };
    inline VM_RCS BuildVM_RCS() {
        VM_RCS vm{};
        if (!WeaponConfig::SelectedWeaponKey.empty()) {
            auto key = WeaponConfig::SelectedWeaponKey;
            auto it = WeaponConfig::WeaponConfigs.find(key);
            if (it == WeaponConfig::WeaponConfigs.end()) {
                WeaponConfig::WeaponProfile p;
                p.rcsEnabled = LegitBotConfig::RCS;
                p.rcsBullet = RCS::RCSBullet;
                p.rcsYaw = RCS::RCSScale.x;
                p.rcsPitch = RCS::RCSScale.y;
                WeaponConfig::WeaponConfigs[key] = p;
                it = WeaponConfig::WeaponConfigs.find(key);
            }
            const auto& p = it->second;
            vm.enabled = p.rcsEnabled;
            vm.bullet = p.rcsBullet;
            vm.yaw = p.rcsYaw;
            vm.pitch = p.rcsPitch;
        } else {
            vm.enabled = LegitBotConfig::RCS;
            vm.bullet = RCS::RCSBullet;
            vm.yaw = RCS::RCSScale.x;
            vm.pitch = RCS::RCSScale.y;
        }
        return vm;
    }
    inline void ApplyVM_RCS(const VM_RCS& vm) {
        if (!WeaponConfig::SelectedWeaponKey.empty()) {
            auto& p = WeaponConfig::WeaponConfigs[WeaponConfig::SelectedWeaponKey];
            p.rcsEnabled = vm.enabled;
            p.rcsBullet = vm.bullet;
            p.rcsYaw = vm.yaw;
            p.rcsPitch = vm.pitch;
        } else {
            LegitBotConfig::RCS = vm.enabled;
            RCS::RCSBullet = vm.bullet;
            RCS::RCSScale.x = vm.yaw;
            RCS::RCSScale.y = vm.pitch;
        }
    }

    struct VM_Global {
        bool workInSpec;
        bool teamCheck;
        int menuHotKey;
        std::string menuHotKeyName;
    };
    inline VM_Global BuildVM_Global() {
        VM_Global vm{};
        vm.workInSpec = MenuConfig::WorkInSpec;
        vm.teamCheck = MenuConfig::TeamCheck;
        vm.menuHotKey = MenuConfig::HotKey;
        vm.menuHotKeyName = Text::Misc::HotKey;
        return vm;
    }
    inline void ApplyVM_Global(const VM_Global& vm) {
        MenuConfig::WorkInSpec = vm.workInSpec;
        MenuConfig::TeamCheck = vm.teamCheck;
        MenuConfig::HotKey = vm.menuHotKey;
        Text::Misc::HotKey = vm.menuHotKeyName;
    }
    inline void UpdateHotkeyLabels()
    {
        Text::ESP::HotKey = KeyMgr::GetKeyName(ESPConfig::HotKey);
        Text::Aimbot::HotKey = KeyMgr::GetKeyName(AimControl::HotKey);
        Text::Trigger::HotKey = KeyMgr::GetKeyName(TriggerBot::HotKey);
        Text::Misc::HotKey = KeyMgr::GetKeyName(MenuConfig::HotKey);
    }
    inline void LoadDefaultConfig()
	{
		if (!MenuConfig::defaultConfig)
			return;

		MyConfigSaver::LoadConfig("default.cfg");

		MenuConfig::defaultConfig = false;
	}

	inline void InitHitboxList()
	{
		if (LegitBotConfig::HitboxUpdated)
			return;
		auto HitboxList = AimControl::HitboxList;

		auto it = std::find(HitboxList.begin(), HitboxList.end(), BONEINDEX::head);
		if (it != HitboxList.end())
			checkbox1 = true;

		it = std::find(HitboxList.begin(), HitboxList.end(), BONEINDEX::neck_0);
		if (it != HitboxList.end())
			checkbox2 = true;

		it = std::find(HitboxList.begin(), HitboxList.end(), BONEINDEX::spine_1);
		if (it != HitboxList.end())
			checkbox3 = true;

		it = std::find(HitboxList.begin(), HitboxList.end(), BONEINDEX::spine_2);
		if (it != HitboxList.end())
			checkbox4 = true;

		it = std::find(HitboxList.begin(), HitboxList.end(), BONEINDEX::pelvis);
		if (it != HitboxList.end())
			checkbox5 = true;

		LegitBotConfig::HitboxUpdated = true;
	}
    inline void addHitbox(int BoneIndex)
	{
		AimControl::HitboxList.push_back(BoneIndex);
	}
    inline void removeHitbox(int BoneIndex)
	{
		for (auto it = AimControl::HitboxList.begin(); it != AimControl::HitboxList.end(); ++it) {
			if (*it == BoneIndex) {
				AimControl::HitboxList.erase(it);
				break;
			}
		}
	}

    inline void SetMenuTitleIcon(const unsigned char* data, unsigned int size)
    {
        Gui.LoadTextureFromMemoryIcon(const_cast<unsigned char*>(data), size, &MenuTitleIcon, &MenuTitleIconW, &MenuTitleIconH);
    }

inline void LoadImages()
	{
		if (Logo == NULL)
		{
			// Updater::CheckForUpdates();
			Gui.LoadTextureFromMemory(Images::Logo, sizeof Images::Logo, &Logo, &LogoW, &LogoH);
			// Ícones com fundo removido e conteúdo branco
			Gui.LoadTextureFromMemoryIcon(Images::AimbotButton, sizeof Images::AimbotButton, &MenuButton1, &buttonW, &buttonH);
			Gui.LoadTextureFromMemoryIcon(Images::VisualButton, sizeof Images::VisualButton, &MenuButton2, &buttonW, &buttonH);
			Gui.LoadTextureFromMemoryIcon(Images::MiscButton, sizeof Images::MiscButton, &MenuButton3, &buttonW, &buttonH);
			Gui.LoadTextureFromMemoryIcon(Images::ConfigButton, sizeof Images::ConfigButton, &MenuButton4, &buttonW, &buttonH);
			Gui.LoadTextureFromMemory(Images::PreviewImg, sizeof Images::PreviewImg, &HitboxImage, &hitboxW, &hitboxH);
			Gui.LoadTextureFromMemoryIcon(Images::AimbotButtonPressed, sizeof Images::AimbotButtonPressed, &MenuButton1Pressed, &buttonW, &buttonH);
			Gui.LoadTextureFromMemoryIcon(Images::VisualButtonPressed, sizeof Images::VisualButtonPressed, &MenuButton2Pressed, &buttonW, &buttonH);
			Gui.LoadTextureFromMemoryIcon(Images::MiscButtonPressed, sizeof Images::MiscButtonPressed, &MenuButton3Pressed, &buttonW, &buttonH);
			Gui.LoadTextureFromMemoryIcon(Images::ConfigButtonPressed, sizeof Images::ConfigButtonPressed, &MenuButton4Pressed, &buttonW, &buttonH);

			MenuConfig::MarkWinPos = ImVec2(ImGui::GetIO().DisplaySize.x - 300.0f, 100.f);
			MenuConfig::RadarWinPos = ImVec2(25.f, 25.f);
			MenuConfig::SpecWinPos = ImVec2(10.0f, ImGui::GetIO().DisplaySize.y / 2 - 200);
			MenuConfig::BombWinPos = ImVec2((ImGui::GetIO().DisplaySize.x - 200.0f) / 2.0f, 80.0f);
            SetMenuTitleIcon(MenuTitleIconBytes, MenuTitleIconBytes_len);
		}
	}

	// Components Settings
	// ########################################
inline void AlignRight(float ContentWidth)
{
		// Alinha baseado na largura disponível do conteúdo, mais robusto em diferentes layouts
		float rightX = ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - ContentWidth;
		ImGui::SetCursorPosX(rightX);
	}

	// Helpers para estruturar sections (sem dropdown), com borda e título
inline void BeginSection(const char* title, ImVec2 size = ImVec2(0.f, 0.f), bool bordered = true)
{
    ImGui::BeginChild(title, size, false, ImGuiWindowFlags_NoScrollbar);
    // Compactar espaçamentos dentro da section
    ImGuiStyle& st = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(st.ItemSpacing.x, 4.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(st.FramePadding.x, 5.0f));
    ImGui::TextDisabled(title);
    if (strcmp(title, "Aimbot") == 0 || strcmp(title, "Recoil Control") == 0)
    {
        ImGui::SameLine(0.0f, 6.0f);
        auto UnsafeBadgeTitle = [&](const char* id, const char* tooltip){
            ImVec4 accent = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            const float iconSize = ImGui::GetFontSize() * 0.80f;
            const char* sym = OSImGui::FontAwesome6Available ? ICON_FA_EXCLAMATION_TRIANGLE : "-";
            float iconW = ImGui::CalcTextSize(sym).x * (iconSize / ImGui::GetFontSize());
            ImDrawList* dl = ImGui::GetWindowDrawList();
            ImVec2 p = ImGui::GetCursorScreenPos();
            p.y += (ImGui::GetTextLineHeight() - iconSize) * 0.5f;
            ImU32 col = ImGui::GetColorU32(accent);
            dl->AddText(ImGui::GetFont(), iconSize, p, col, sym);
            ImGui::Dummy(ImVec2(iconW, iconSize));
            bool hovered = ImGui::IsItemHovered();
            ImGuiStorage* stg = ImGui::GetStateStorage();
            ImGuiID key = ImGui::GetID(id);
            float* alpha = stg->GetFloatRef(key, 0.0f);
            float target = hovered ? 1.0f : 0.0f;
            float dt = ImGui::GetIO().DeltaTime;
            float k = ImClamp(dt * 12.0f, 0.0f, 1.0f);
            *alpha = (*alpha) + (target - (*alpha)) * k;
            if (*alpha > 0.01f && hovered)
            {
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, *alpha);
                ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.06f, 0.06f, 0.08f, 0.97f));
                ImGui::PushStyleColor(ImGuiCol_Border, accent);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 8));
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
                ImGui::BeginTooltip();
                ImGui::TextColored(accent, "Unsafe Feature");
                ImGui::Separator();
                ImGui::TextUnformatted(tooltip);
                ImGui::EndTooltip();
                ImGui::PopStyleVar(2);
                ImGui::PopStyleColor(2);
                ImGui::PopStyleVar();
            }
        };
        if (strcmp(title, "Aimbot") == 0)
            UnsafeBadgeTitle("aimbot-unsafe-title", "Using an aimbot can get you a VAC Live cooldown if you don't use a good config. Make some adjustments before using it.");
        else
            UnsafeBadgeTitle("rcs-unsafe-title", "VAC Live has some mechanisms to detect this type of feature. Don't abuse it — try to humanize your gameplay as much as possible.");
    }
    ImGui::Separator();
}

inline const char* TrimLeft(const char* s)
{
    while (s && (*s == ' ' || *s == '\t')) ++s;
    return s;
}
inline void GradientText(const char* text, ImVec4 startColor, ImVec4 endColor, float font_size_override, ImFont* font_override);
inline void BeginSectionWithHeaderActions(const char* title, ImVec2 size, bool bordered, const std::function<void()>& actions)
{
    ImGui::BeginChild(title, size, false, ImGuiWindowFlags_NoScrollbar);
    ImGuiStyle& st = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(st.ItemSpacing.x, 4.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(st.FramePadding.x, 5.0f));
    ImGui::TextDisabled(title);
    if (strcmp(title, "Aimbot") == 0)
    {
        ImGui::SameLine(0.0f, 6.0f);
        ImVec4 accent = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        const float iconSize = ImGui::GetFontSize() * 0.80f;
        const char* sym = OSImGui::FontAwesome6Available ? ICON_FA_EXCLAMATION_TRIANGLE : "-";
        float iconW = ImGui::CalcTextSize(sym).x * (iconSize / ImGui::GetFontSize());
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 p = ImGui::GetCursorScreenPos();
        p.y += (ImGui::GetTextLineHeight() - iconSize) * 0.5f;
        ImU32 col = ImGui::GetColorU32(accent);
        dl->AddText(ImGui::GetFont(), iconSize, p, col, sym);
        ImGui::Dummy(ImVec2(iconW, iconSize));
        bool hovered = ImGui::IsItemHovered();
        ImGuiStorage* stg = ImGui::GetStateStorage();
        ImGuiID key = ImGui::GetID("aimbot-unsafe-title");
        float* alpha = stg->GetFloatRef(key, 0.0f);
        float target = hovered ? 1.0f : 0.0f;
        float dt = ImGui::GetIO().DeltaTime;
        float k = ImClamp(dt * 12.0f, 0.0f, 1.0f);
        *alpha = (*alpha) + (target - (*alpha)) * k;
        if (*alpha > 0.01f && hovered)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, *alpha);
            ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.06f, 0.06f, 0.08f, 0.97f));
            ImGui::PushStyleColor(ImGuiCol_Border, accent);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 8));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
            ImGui::BeginTooltip();
            ImGui::TextColored(accent, "Unsafe Feature");
            ImGui::Separator();
            ImGui::TextUnformatted("Using an aimbot can get you a VAC Live cooldown if you don't use a good config. Make some adjustments before using it.");
            ImGui::EndTooltip();
            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar();
        }
    }
    ImGui::SameLine(0.0f, 6.0f);
    const float btnW = 54.f, btnH = 22.f;
    const float iconSize = ImGui::GetFontSize() * 0.85f;
    const float iconW = ImGui::CalcTextSize(ICON_FA_KEYBOARD).x * (iconSize / ImGui::GetFontSize());
    AlignRight(btnW + iconW + 4.0f + ImGui::GetStyle().WindowPadding.x + 12.0f);
    float yBase = ImGui::GetCursorPosY();
    ImGui::SetCursorPosY(yBase + (ImGui::GetTextLineHeight() - btnH) * 0.5f + (btnH - iconSize) * 0.5f);
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImU32 col = ImGui::GetColorU32(ImVec4(0.7f,0.7f,0.7f,1.0f));
        dl->AddText(ImGui::GetFont(), iconSize, p, col, ICON_FA_KEYBOARD);
        ImGui::Dummy(ImVec2(iconW, iconSize));
        if (ImGui::IsItemClicked()) {
            std::string pop = std::string(title) + "##ModePopup";
            ImGui::OpenPopup(pop.c_str());
        }
        std::string popName = std::string(title) + "##ModePopup";
        if (ImGui::BeginPopup(popName.c_str())) {
            auto setMode = [&](int mode){
                if (strcmp(title, "Aimbot") == 0) {
                    AimControl::ActivationMode = mode;
                    if (!WeaponConfig::SelectedWeaponKey.empty()) {
                        auto& p = WeaponConfig::WeaponConfigs[WeaponConfig::SelectedWeaponKey];
                        p.activationMode = mode;
                    }
                } else if (strcmp(title, "Triggerbot") == 0) {
                    TriggerBot::ActivationMode = mode;
                    if (!WeaponConfig::SelectedWeaponKey.empty()) {
                        auto& p = WeaponConfig::WeaponConfigs[WeaponConfig::SelectedWeaponKey];
                        p.trigActivationMode = mode;
                    }
                }
            };
            int current = 0;
            if (strcmp(title, "Aimbot") == 0) {
                current = !WeaponConfig::SelectedWeaponKey.empty() ? WeaponConfig::WeaponConfigs[WeaponConfig::SelectedWeaponKey].activationMode : AimControl::ActivationMode;
            } else if (strcmp(title, "Triggerbot") == 0) {
                current = !WeaponConfig::SelectedWeaponKey.empty() ? WeaponConfig::WeaponConfigs[WeaponConfig::SelectedWeaponKey].trigActivationMode : TriggerBot::ActivationMode;
            }
            bool selHold = (current == 0);
            bool selToggle = (current == 1);
            bool selAlways = (current == 2);
            if (ImGui::Selectable("Toggle Key", selToggle)) setMode(1);
            if (ImGui::Selectable("Hold Key", selHold)) setMode(0);
            if (ImGui::Selectable("Always Active", selAlways)) setMode(2);
            ImGui::EndPopup();
        }
    }
    ImGui::SameLine(0.0f, 4.0f);
    ImGui::SetCursorPosY(yBase + (ImGui::GetTextLineHeight() - btnH) * 0.5f);
    if (actions) actions();
    ImGui::Separator();
    ImGui::SetCursorPosX(ImGui::GetStyle().WindowPadding.x);
}
    // Texto com degradê letra a letra (para títulos decorativos)
    inline void GradientText(const char* text, ImVec4 startColor, ImVec4 endColor, float font_size_override = 0.0f, ImFont* font_override = nullptr)
    {
        ImDrawList* draw = ImGui::GetWindowDrawList();
        ImFont* font = font_override ? font_override : ImGui::GetFont();
        float base_size = ImGui::GetFontSize();
        float font_size = font_size_override > 0.0f ? font_size_override : (font_override ? font->FontSize : base_size);
        ImVec2 p = ImGui::GetCursorScreenPos();
        // Centraliza verticalmente no frame APENAS quando usando a fonte/size padrão
        if (font_override == nullptr && font_size_override <= 0.0f)
            p.y += (ImGui::GetFrameHeight() - font_size) * 0.5f;

        int len = (int)strlen(text);
        float scale = font_size / base_size;
        for (int i = 0; i < len; ++i)
        {
            float t = len > 1 ? (float)i / (float)(len - 1) : 0.0f;
            ImVec4 c;
            c.x = startColor.x + (endColor.x - startColor.x) * t;
            c.y = startColor.y + (endColor.y - startColor.y) * t;
            c.z = startColor.z + (endColor.z - startColor.z) * t;
            c.w = startColor.w + (endColor.w - startColor.w) * t;

            char ch[2] = { text[i], '\0' };
            draw->AddText(font, font_size, p, ImGui::ColorConvertFloat4ToU32(c), ch);
            p.x += ImGui::CalcTextSize(ch).x * scale;
        }
        // Move o cursor horizontalmente conforme o texto renderizado
        ImVec2 adv = ImGui::CalcTextSize(text);
        ImGui::Dummy(ImVec2(adv.x * scale, font_size));
    }
    inline void EndSection()
    {
        ImGui::PopStyleVar(2);
        ImGui::EndChild();
    }
inline void PutSwitch(const char* string, float CursorX, float ContentWidth, bool* v, bool ColorEditor = false, const char* lable = NULL, float col[4] = NULL, const char* Tip = NULL)
{
		ImGui::PushID(string);
		float CurrentCursorX = ImGui::GetCursorPosX();
		float CurrentCursorY = ImGui::GetCursorPosY();
		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
        ImGui::TextDisabled(TrimLeft(string));
		if (Tip && ImGui::IsItemHovered())
			ImGui::SetTooltip(Tip);
		ImGui::SameLine(0.0f, 6.0f);
		// Alinhar verticalmente os controles à altura do texto
		ImGui::SetCursorPosY(CurrentCursorY);
        if (ColorEditor) {
            // Fixar posição da checkbox e desenhar seletor menor à esquerda, sem empurrar
            float fh = ImGui::GetFrameHeight();
            float spacing = 2.0f;
            float previewSize = fh - 4.0f; // ligeiramente menor que a checkbox

            // Posição fixa da checkbox (alinhada à direita)
            AlignRight(fh + ImGui::GetStyle().FramePadding.x * 2.0f);
            float checkboxX = ImGui::GetCursorPosX();
            ImGui::SetCursorPosY(CurrentCursorY);

            // Preview imediatamente à esquerda, sem contorno
            float previewX = checkboxX - spacing - previewSize;
            ImGui::SetCursorPosX(previewX);
            ImGui::SetCursorPosY(CurrentCursorY);
            ImVec4 bg = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
            ImGui::PushStyleColor(ImGuiCol_Button, bg);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, bg);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, bg);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
            bool open = ImGui::ColorButton(lable, ImVec4(col[0], col[1], col[2], col[3]), ImGuiColorEditFlags_AlphaPreview, ImVec2(previewSize, previewSize));
            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(3);
            if (open) ImGui::OpenPopup("##ColorPicker");

            // Restaurar cursor para a checkbox na posição original
            ImGui::SetCursorPosX(checkboxX);
            ImGui::SetCursorPosY(CurrentCursorY);

            if (ImGui::BeginPopup("##ColorPicker")) {
                ImGui::ColorPicker4(lable, col, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_DisplayHSV);
                ImGui::EndPopup();
            }
        }
        else {
            // Alinhar checkbox simples à direita da section, simétrico
            float fh = ImGui::GetFrameHeight();
            ImGuiStyle& s = ImGui::GetStyle();
            float cbw = fh + s.FramePadding.x * 2.0f;
            AlignRight(cbw);
        }

        // Checkbox com fundo cinza e contorno roxo; tick branco
        ImVec4 gray      = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
        ImVec4 grayHover = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
        ImVec4 grayActive= ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
        ImVec4 accent    = ImGui::GetStyleColorVec4(ImGuiCol_Button);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, gray);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, grayHover);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, grayActive);
        ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(1.00f, 1.00f, 1.00f, 0.00f));
        ImGui::PushStyleColor(ImGuiCol_Border, accent);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3.0f, 2.0f));
        ImGui::Checkbox("##checkbox", v);
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(5);
        // Tick animation: desenhar suavemente quando marcado
        {
            ImVec2 min = ImGui::GetItemRectMin();
            ImVec2 max = ImGui::GetItemRectMax();
            float sz = max.y - min.y;
            float pad = 3.0f;
            static std::unordered_map<const bool*, float> anim;
            float& prog = anim[v];
            float target = *v ? 1.0f : 0.0f;
            float k = ImClamp(ImGui::GetIO().DeltaTime * 8.0f, 0.0f, 1.0f); // mais suave
            prog = ImLerp(prog, target, k);
            if (prog > 0.001f)
            {
                float base = (sz - pad * 2.0f);
                float size = base * prog; // escala suave para ativar/desativar
                float offset = (sz - size) * 0.5f;
                ImVec2 top_left = ImVec2(min.x + offset, min.y + offset);
                ImVec4 c = ImVec4(1,1,1,prog); // fade/escala
                ImGui::RenderCheckMark(ImGui::GetWindowDrawList(), top_left, ImGui::GetColorU32(c), size);
            }
        }
        ImGui::PopID();
}

	// Helper para checkboxes pequenas (usado nos hitboxes e em outros locais diretos)
inline bool SmallCheckbox(const char* id, bool* v)
{
        ImVec4 gray      = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
        ImVec4 grayHover = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
        ImVec4 grayActive= ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
        ImVec4 accent    = ImGui::GetStyleColorVec4(ImGuiCol_Button);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, gray);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, grayHover);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, grayActive);
        ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(1.00f, 1.00f, 1.00f, 0.00f));
        ImGui::PushStyleColor(ImGuiCol_Border, accent);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3.0f, 2.0f));
        bool changed = ImGui::Checkbox(id, v);
        {
            ImVec2 min = ImGui::GetItemRectMin();
            ImVec2 max = ImGui::GetItemRectMax();
            float sz = max.y - min.y;
            float pad = 2.0f;
            static std::unordered_map<const bool*, float> anim;
            float& prog = anim[v];
            float target = *v ? 1.0f : 0.0f;
            float k = ImClamp(ImGui::GetIO().DeltaTime * 8.0f, 0.0f, 1.0f);
            prog = ImLerp(prog, target, k);
            if (prog > 0.001f)
            {
                float base = (sz - pad * 2.0f);
                float size = base * prog;
                float offset = (sz - size) * 0.5f;
                ImVec2 top_left = ImVec2(min.x + offset, min.y + offset);
                ImVec4 c = ImVec4(1,1,1,prog);
                ImGui::RenderCheckMark(ImGui::GetWindowDrawList(), top_left, ImGui::GetColorU32(c), size);
            }
        }
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(5);
        return changed;
}
    inline void PutColorEditor(const char* text, const char* lable, float CursorX, float ContentWidth, float col[4], const char* Tip = NULL)
	{
		ImGui::PushID(text);
		float CurrentCursorX = ImGui::GetCursorPosX();
		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		ImGui::TextDisabled(text);
		if (Tip && ImGui::IsItemHovered())
			ImGui::SetTooltip(Tip);
		ImGui::SameLine();
		AlignRight(ContentWidth + ImGui::GetFrameHeight() + 8);
		ImGui::ColorEdit4(lable, col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
		ImGui::PopID();
	}
    inline void PutSliderFloat(const char* string, float CursorX, float* v, const void* p_min, const void* p_max, const char* format, const char* Tip = NULL)
	{
		// if there is no fucking ID, all the sliders would be fucking forced to sync when you click on one of them ;3
		ImGui::PushID(string);
		float CurrentCursorX = ImGui::GetCursorPosX();
		float SliderWidth = ImGui::GetColumnWidth() - ImGui::GetStyle().ItemSpacing.x - CursorX - 15;
		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
        ImGui::TextDisabled(TrimLeft(string));
		if (Tip && ImGui::IsItemHovered())
			ImGui::SetTooltip(Tip);
        ImGui::SameLine(0.0f, 4.0f);
        ImGui::TextDisabled(format, *v);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 3.0f));
        ImGui::SetCursorPosX(CurrentCursorX + CursorX);
        ImGui::SetNextItemWidth(SliderWidth - 7);
        Gui.SliderScalarEx2("", ImGuiDataType_Float, v, p_min, p_max, "", ImGuiSliderFlags_NoInput);
        ImGui::PopStyleVar();
		ImGui::PopID();
	}
inline void PutSliderInt(const char* string, float CursorX, int* v, const void* p_min, const void* p_max, const char* format, const char* Tip = NULL)
	{
		ImGui::PushID(string);
		float CurrentCursorX = ImGui::GetCursorPosX();
		float SliderWidth = ImGui::GetColumnWidth() - ImGui::GetStyle().ItemSpacing.x - CursorX-15;
		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		ImGui::TextDisabled(string);
		if (Tip && ImGui::IsItemHovered())
			ImGui::SetTooltip(Tip);
        ImGui::SameLine(0.0f, 4.0f);
        ImGui::TextDisabled(format, *v);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 3.0f));
        ImGui::SetCursorPosX(CurrentCursorX + CursorX);
        ImGui::SetNextItemWidth(SliderWidth - 7);
        Gui.SliderScalarEx2("", ImGuiDataType_Float, v, p_min, p_max, "", ImGuiSliderFlags_NoInput);
        ImGui::PopStyleVar();
	ImGui::PopID();
	}

    inline bool CenteredHotkeyButton(const char* label, ImVec2 size)
    {
        float font_h = ImGui::GetFont()->FontSize;
        float ypad = (size.y - font_h) * 0.5f;
        if (ypad < 1.0f) ypad = 1.0f;
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.f, ypad));
        bool clicked = ImGui::Button(label, size);
        ImGui::PopStyleVar(2);
        return clicked;
    }
	// ########################################

	inline void DrawGui()
	{
		LoadImages();
		ImTextureID ImageID;
		ImVec2 LogoSizeSmall;

		ImageID = (void*)Logo;
		LogoSizeSmall = ImVec2(24.f, 24.f);

		ImColor BorderColor = ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border));

		char TempText[256];
		ImGuiWindowFlags Flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar;
        ImGui::SetNextWindowPos({ (ImGui::GetIO().DisplaySize.x - MenuConfig::WCS.MainWinSize.x) / 2.0f, (ImGui::GetIO().DisplaySize.y - MenuConfig::WCS.MainWinSize.y) / 2.0f }, ImGuiCond_Once);
        ImGui::SetNextWindowSize(MenuConfig::WCS.MainWinSize);
        // Fundo do menu: cinza bem escuro para combinar com roxo (aplica na janela e nos childs)
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.06f, 0.06f, 0.08f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ChildBg,  ImVec4(0.06f, 0.06f, 0.08f, 1.0f));
        ImGui::Begin("Saturn", nullptr, Flags);
			{
                // Topo com título à esquerda e abas à direita (mesma linha)
                // Calcular altura do cabeçalho com base no título (fonte nativa) e garantir simetria acima/abaixo
                float base_size = ImGui::GetFontSize();
                ImFont* titleFont = OSImGui::TitleFontLarge ? OSImGui::TitleFontLarge : OSImGui::TitleFont;
                float title_size = titleFont ? titleFont->FontSize : (base_size * 1.9f);
                ImGuiStyle& style = ImGui::GetStyle();
                float top_height = title_size + style.FramePadding.y * 2.0f; // simétrico: mesma folga em cima/baixo
                ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
                ImGui::BeginChild("TopTabs", ImVec2(0, top_height), false, ImGuiWindowFlags_NoScrollbar);
                {
                    // Título "Saturn" com degradê (início roxo) à esquerda
                    ImVec4 start = ImVec4(0.486f, 0.227f, 0.929f, 1.00f); // #7C3AED
                    ImVec4 end   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
                    // alinhar verticalmente o título dentro do cabeçalho de forma simétrica
                    float top_pad_y = (top_height - title_size) * 0.5f;
                    ImGui::SetCursorPosY(top_pad_y);
                    const float headerIndentX = 1.0f;
                    ImGui::SetCursorPosX(4.0f + headerIndentX);
                    if (MenuTitleIcon) {
                        const float iconScale = 1.25f;
                        float iconSize = title_size * iconScale;
                        if (MenuTitleIconH > 0) iconSize = ImMin(iconSize, (float)MenuTitleIconH);
                        float yAlign = top_pad_y + (title_size - iconSize) * 0.5f;
                        const float iconYOffset = 2.0f;
                        ImGui::SetCursorPosY(yAlign + iconYOffset);
                        ImGui::Image((void*)MenuTitleIcon, ImVec2(iconSize, iconSize));
                        ImGui::SameLine(0.0f, 4.0f);
                        ImGui::SetCursorPosY(top_pad_y);
                    }
                    if (titleFont) ImGui::PushFont(titleFont);
                    GradientText("Saturn", start, end, title_size, titleFont);
                    if (titleFont) ImGui::PopFont();
                    // Abas à direita do título (mesma linha, com espaçamento menor)
                    const float tabsExtraShift = -4.0f;
                    ImGui::SameLine(0.0f, 8.0f + tabsExtraShift);
                    // diminuir a altura das abas para ficarem simétricas com a base das letras do título
                    float tab_pad_y = style.FramePadding.y * 0.60f;
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, tab_pad_y));
                    // centralizar verticalmente as abas dentro do cabeçalho
                    float tab_frame_h = ImGui::GetFrameHeight();
                    float tabs_y = top_pad_y + (title_size - tab_frame_h) * 0.5f;
                    const float tabsYOffset = 3.0f;
                    ImGui::SetCursorPosY(tabs_y + tabsYOffset);
                    // remover a linha abaixo das abas (separator/border do TabBar)
                    ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0,0,0,0));
                    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0,0,0,0));
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                    if (ImGui::BeginTabBar("MainTabs", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_FittingPolicyScroll))
                    {
                        if (ImGui::BeginTabItem(ICON_FA_CROSSHAIRS "  Legit")) { MenuConfig::WCS.MenuPage = 0; ImGui::EndTabItem(); }
                        if (ImGui::BeginTabItem(ICON_FA_EYE        "  Visual")) { MenuConfig::WCS.MenuPage = 1; ImGui::EndTabItem(); }
                        if (ImGui::BeginTabItem(ICON_FA_WRENCH     "  Misc"))  { MenuConfig::WCS.MenuPage = 2; ImGui::EndTabItem(); }
                        // Config tab removida temporariamente
                        ImGui::EndTabBar();
                    }
                    if (MenuConfig::WCS.MenuPage == 0)
                    {
                        float tab_frame_h2 = ImGui::GetFrameHeight();
                        float tabs_y2 = top_pad_y + (title_size - tab_frame_h2) * 0.5f;
                        ImGui::SetCursorPosY(tabs_y2);
                        const float iconSize = ImGui::GetFontSize() * 1.15f;
                        const float iconW = ImGui::CalcTextSize(ICON_FA_GUN).x * (iconSize / ImGui::GetFontSize());
                        const float comboW = 160.0f;
                        AlignRight(iconW + comboW + 12.0f);
                        ImDrawList* dl = ImGui::GetWindowDrawList();
                        ImVec2 p = ImGui::GetCursorScreenPos();
                        float fh = ImGui::GetFrameHeight();
                        p.y += (fh - iconSize) * 0.5f;
                        ImU32 col = ImGui::GetColorU32(ImVec4(1,1,1,1));
                        const char* iconTxt = ICON_FA_GUN;
                        if (!OSImGui::FontAwesome6Available)
                            iconTxt = ICON_FA_CROSSHAIRS;
                        dl->AddText(ImGui::GetFont(), iconSize, p, col, iconTxt);
                        ImGui::Dummy(ImVec2(iconW, iconSize));
                        ImGui::SameLine(0.0f, 4.0f);
                        std::string currentLabel = "Weapons";
                        if (!WeaponConfig::SelectedWeaponKey.empty()) {
                            const std::string& k = WeaponConfig::SelectedWeaponKey;
                            if (k=="usp") currentLabel = "USP-S"; else if (k=="p2000") currentLabel = "P2000"; else if (k=="glock") currentLabel = "Glock-18"; else if (k=="elite") currentLabel = "Dual Berettas"; else if (k=="p250") currentLabel = "P250"; else if (k=="fiveseven") currentLabel = "Five-SeveN"; else if (k=="cz75a") currentLabel = "CZ75-Auto"; else if (k=="tec9") currentLabel = "Tec-9"; else if (k=="deagle") currentLabel = "Desert Eagle"; else if (k=="revolver") currentLabel = "R8 Revolver"; else if (k=="mp9") currentLabel = "MP9"; else if (k=="mac10") currentLabel = "MAC-10"; else if (k=="mp7") currentLabel = "MP7"; else if (k=="mp5sd") currentLabel = "MP5-SD"; else if (k=="ump45") currentLabel = "UMP-45"; else if (k=="p90") currentLabel = "P90"; else if (k=="bizon") currentLabel = "PP-Bizon"; else if (k=="famas") currentLabel = "FAMAS"; else if (k=="galilar") currentLabel = "Galil AR"; else if (k=="ak47") currentLabel = "AK-47"; else if (k=="m4a1") currentLabel = "M4A1-S"; else if (k=="m4a4") currentLabel = "M4A4"; else if (k=="aug") currentLabel = "AUG"; else if (k=="sg556") currentLabel = "SG 553"; else if (k=="ssg08") currentLabel = "SSG 08"; else if (k=="awp") currentLabel = "AWP"; else if (k=="g3Sg1") currentLabel = "G3SG1"; else if (k=="scar20") currentLabel = "SCAR-20"; else if (k=="nova") currentLabel = "Nova"; else if (k=="xm1014") currentLabel = "XM1014"; else if (k=="mag7") currentLabel = "MAG-7"; else if (k=="sawedoff") currentLabel = "Sawed-Off"; else if (k=="m249") currentLabel = "M249"; else if (k=="negev") currentLabel = "Negev"; else currentLabel = k;
                        }
                        ImGui::SetNextItemWidth(comboW);
                        if (ImGui::BeginCombo("##Weapons", currentLabel.c_str()))
                        {
                            auto addItem = [&](const char* label, const char* key){ bool selected = (WeaponConfig::SelectedWeaponKey == key); if (ImGui::Selectable(label, selected)) WeaponConfig::SelectedWeaponKey = key; };
                            ImGui::TextDisabled("Pistols");
                            addItem("USP-S", "usp");
                            addItem("P2000", "p2000");
                            addItem("Glock-18", "glock");
                            addItem("Dual Berettas", "elite");
                            addItem("P250", "p250");
                            addItem("Five-SeveN", "fiveseven");
                            addItem("CZ75-Auto", "cz75a");
                            addItem("Tec-9", "tec9");
                            addItem("Desert Eagle", "deagle");
                            addItem("R8 Revolver", "revolver");
                            ImGui::Separator();
                            ImGui::TextDisabled("SMG");
                            addItem("MP9", "mp9");
                            addItem("MAC-10", "mac10");
                            addItem("MP7", "mp7");
                            addItem("MP5-SD", "mp5sd");
                            addItem("UMP-45", "ump45");
                            addItem("P90", "p90");
                            addItem("PP-Bizon", "bizon");
                            ImGui::Separator();
                            ImGui::TextDisabled("Rifles");
                            addItem("FAMAS", "famas");
                            addItem("Galil AR", "galilar");
                            addItem("AK-47", "ak47");
                            addItem("M4A1-S", "m4a1");
                            addItem("M4A4", "m4a4");
                            addItem("AUG", "aug");
                            addItem("SG 553", "sg556");
                            ImGui::Separator();
                            ImGui::TextDisabled("Snipers");
                            addItem("SSG 08", "ssg08");
                            addItem("AWP", "awp");
                            addItem("G3SG1", "g3Sg1");
                            addItem("SCAR-20", "scar20");
                            ImGui::Separator();
                            ImGui::TextDisabled("Heavy Weapons");
                            addItem("Nova", "nova");
                            addItem("XM1014", "xm1014");
                            addItem("MAG-7", "mag7");
                            addItem("Sawed-Off", "sawedoff");
                            addItem("M249", "m249");
                            addItem("Negev", "negev");
                            ImGui::EndCombo();
                        }
                    }
                    ImGui::PopStyleVar(2);
                    ImGui::PopStyleColor(2);
                    ImGui::PopStyleVar();
                }
                ImGui::EndChild();
                ImGui::PopStyleVar();

			// Área de conteúdo abaixo das abas
			ImGui::BeginChild("Page", ImVec2(0, 0), false, 0);
		{
			// Layout por página
			if (MenuConfig::WCS.MenuPage == 1)
			{
                // Visual: organizar em 3 colunas (ESP, Preview, Radar)
                ImGui::Columns(3, nullptr, false);
                ImGui::SetCursorPos(ImVec2(12.f, 6.f));
					static const float MinRounding = 0.f, MaxRouding = 5.f;
					static const float MinFovFactor = 0.f, MaxFovFactor = 1.f;

                BeginSectionWithHeaderActions("ESP", ImVec2(ImGui::GetColumnWidth(), 0), true, []{
                    if (CenteredHotkeyButton(Text::ESP::HotKey.c_str(), ImVec2(54.f, 22.f)))
                    {
                        std::thread([&]() {
                            KeyMgr::GetPressedKey(ESPConfig::HotKey, &Text::ESP::HotKey);
                        }).detach();
                    }
                });
                    {
                        auto vmEsp = BuildVM_ESP();
                        PutSwitch(Text::ESP::Enable.c_str(), 0.f, ImGui::GetFrameHeight() * 1.7, &vmEsp.enabled);
                        
                        

                        if (vmEsp.enabled)
                        {
                            PutSwitch(Text::ESP::Box.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &vmEsp.showBox, true, "###BoxCol", reinterpret_cast<float*>(&ESPConfig::BoxColor));
                            if (vmEsp.showBox)
                            {
                                PutSwitch(Text::ESP::Outline.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &vmEsp.outline);
                                {
                                    PutSwitch(Text::ESP::BoxType.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7f, &vmEsp.isBoxStyle);
                                }
                            }
                            
                            PutSwitch(Text::ESP::Skeleton.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &vmEsp.showBone, true, "###BoneCol", reinterpret_cast<float*>(&ESPConfig::BoneColor));
                            
                            
                            PutSwitch(Text::ESP::OutOfFOVArrow.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &vmEsp.showOutOfFOV, true, "###OutFOVCol", reinterpret_cast<float*>(&ESPConfig::OutOfFOVArrowColor));
                            if(vmEsp.showOutOfFOV)
                            PutSliderFloat(Text::ESP::OutOfFOVRadius.c_str(), .5f, &vmEsp.outOfFOVRadius, &MinFovFactor, &MaxFovFactor, "%.1f");

                            
                            PutSwitch(Text::ESP::HealthBar.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &vmEsp.showHealthBar);
                            
                            PutSwitch(Text::ESP::ShowArmorBar.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &vmEsp.armorBar);
                            
                            PutSwitch(Text::ESP::Weapon.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &vmEsp.showWeapon);
                            PutSwitch(Text::ESP::Ammo.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &vmEsp.ammoBar);
                            PutSwitch(Text::ESP::Distance.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &vmEsp.showDistance);
                            PutSwitch(Text::ESP::PlayerName.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &vmEsp.showPlayerName);
                            PutSwitch(Text::ESP::ScopedESP.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &vmEsp.showScoped);
                            
                            PutSwitch(Text::ESP::VisCheck.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &vmEsp.visibleCheck, true, "###VisibleCol", reinterpret_cast<float*>(&ESPConfig::VisibleColor));
                        }
                        ApplyVM_ESP(vmEsp);
                    }
                    EndSection();

                // Coluna 2: Preview
                ImGui::NextColumn();
                ImGui::SetCursorPosY(6.f);
                BeginSection("ESP Preview", ImVec2(ImGui::GetColumnWidth(), 0));
				{
					ESP::RenderPreview({ ImGui::GetColumnWidth(), ImGui::GetCursorPosY() });
					ImGui::Dummy({ 0.f, ImGui::GetFrameHeight() * 3 });
				}
                EndSection();

                // Coluna 3: Radar
                ImGui::NextColumn();
                ImGui::SetCursorPosY(6.f);
                BeginSection("External Radar", ImVec2(ImGui::GetColumnWidth(), 0));
                {
                        static const float RadarPointSizeProportionMin = 0.2f, RadarPointSizeProportionMax = 2.f;
                        static const float ProportionMin = 500.f, ProportionMax = 15000.f;
                        static const float RadarRangeMin = 100.f, RadarRangeMax = 300.f;
                        static const float AlphaMin = 0.f, AlphaMax = 1.f;
                        RadarCFG::ShowRadar = false;
                        ImGui::BeginDisabled(true);
                        PutSwitch(Text::Radar::Toggle.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &RadarCFG::ShowRadar);
                        ImGui::EndDisabled();
                        ImGui::SameLine(0.0f, 6.0f);
                        {
                            ImVec4 accent = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                            const float iconSize = ImGui::GetFontSize() * 0.80f;
                            const char* sym = OSImGui::FontAwesome6Available ? ICON_FA_EXCLAMATION_TRIANGLE : "-";
                            float iconW = ImGui::CalcTextSize(sym).x * (iconSize / ImGui::GetFontSize());
                            ImDrawList* dl = ImGui::GetWindowDrawList();
                            ImVec2 p = ImGui::GetCursorScreenPos();
                            p.y += (ImGui::GetTextLineHeight() - iconSize) * 0.5f;
                            dl->AddText(ImGui::GetFont(), iconSize, p, ImGui::GetColorU32(accent), sym);
                            ImGui::Dummy(ImVec2(iconW, iconSize));
                            bool hovered = ImGui::IsItemHovered();
                            ImGuiStorage* stg = ImGui::GetStateStorage();
                            ImGuiID key = ImGui::GetID("radar-unavailable-title");
                            float* alpha = stg->GetFloatRef(key, 0.0f);
                            float target = hovered ? 1.0f : 0.0f;
                            float dt = ImGui::GetIO().DeltaTime;
                            float k = ImClamp(dt * 12.0f, 0.0f, 1.0f);
                            *alpha = (*alpha) + (target - (*alpha)) * k;
                            if (*alpha > 0.01f && hovered)
                            {
                                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, *alpha);
                                ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.06f, 0.06f, 0.08f, 0.97f));
                                ImGui::PushStyleColor(ImGuiCol_Border, accent);
                                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 8));
                                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
                                ImGui::BeginTooltip();
                                ImGui::TextColored(accent, "Info");
                                ImGui::Separator();
                                ImGui::TextUnformatted("This feature is currently unavailable as it is still under development.");
                                ImGui::EndTooltip();
                                ImGui::PopStyleVar(2);
                                ImGui::PopStyleColor(2);
                                ImGui::PopStyleVar();
                            }
                        }
                    }
                    EndSection();

                    // Encerrar grade de 3 colunas
                    ImGui::Columns(1);
						
					//ImGui::NewLine();
					//ImGui::GradientText("Crosshairs");
					//float DotMin = 1.f, DotMax = 50.f;
					//int LengthMin = 1, LengthMax = 100;
					//int GapMin = 1, GapMax = 50;
					//int ThickMin = 1, ThickMax = 20;
					//float CircleRmin = 1.f, CircleRmax = 50.f;
					//PutSwitch(Lang::CrosshairsText.Toggle, 5.f, ImGui::GetFrameHeight() * 1.7, &CrosshairsCFG::ShowCrossHair, true, "###CrosshairsCol", reinterpret_cast<float*>(&CrosshairsCFG::CrossHairColor));
					//if (CrosshairsCFG::ShowCrossHair)
					//{
					//	PutSwitch(Lang::CrosshairsText.Dot, 5.f, ImGui::GetFrameHeight() * 1.7, &CrosshairsCFG::drawDot);
					//	if (CrosshairsCFG::drawDot)
					//		PutSliderFloat(Lang::CrosshairsText.DotSizeSlider, 5.f, &CrosshairsCFG::DotSize, &DotMin, &DotMax, "%.f px");
					//	PutSwitch(Lang::CrosshairsText.Outline, 5.f, ImGui::GetFrameHeight() * 1.7, &CrosshairsCFG::drawOutLine);
					//	PutSwitch(Lang::CrosshairsText.Crossline, 5.f, ImGui::GetFrameHeight() * 1.7, &CrosshairsCFG::drawCrossline);
					//	if (CrosshairsCFG::drawCrossline)
					//	{
					//		PutSliderInt(Lang::CrosshairsText.hLengthSlider, 5.f, &CrosshairsCFG::HorizontalLength, &LengthMin, &LengthMax, "%d px");
					//		PutSliderInt(Lang::CrosshairsText.vLengthSilder, 5.f, &CrosshairsCFG::VerticalLength, &LengthMin, &LengthMax, "%d px");
					//		PutSliderInt(Lang::CrosshairsText.GapSlider, 5.f, &CrosshairsCFG::Gap, &GapMin, &GapMax, "%d px");
					//		PutSliderInt(Lang::CrosshairsText.ThicknessSlider, 5.f, &CrosshairsCFG::Thickness, &ThickMin, &ThickMax, "%d px");
					//	}
					//	PutSwitch(Lang::CrosshairsText.tStyle, 5.f, ImGui::GetFrameHeight() * 1.7, &CrosshairsCFG::tStyle);
					//	PutSwitch(Lang::CrosshairsText.Circle, 5.f, ImGui::GetFrameHeight() * 1.7, &CrosshairsCFG::drawCircle);
					//	if (CrosshairsCFG::drawCircle)
					//		PutSliderFloat(Lang::CrosshairsText.RadiusSlider, 5.f, &CrosshairsCFG::CircleRadius, &CircleRmin, &CircleRmax, "%.f px");
					//	PutSwitch(Lang::CrosshairsText.TargetCheck, 5.f, ImGui::GetFrameHeight() * 1.7, &MenuConfig::TargetingCrosshairs, true, "###CircleCol", reinterpret_cast<float*>(&CrosshairsCFG::TargetedColor));
					//	PutSwitch(Lang::CrosshairsText.TeamCheck, 5.f, ImGui::GetFrameHeight() * 1.7, &CrosshairsCFG::TeamCheck);
					//}
					
					//ImGui::Columns(1);
				}
				
			if (MenuConfig::WCS.MenuPage == 0)
			{
                // Legit: organizar em 3 colunas (Aimbot, Triggerbot, RCS)
                ImGui::Columns(3, nullptr, false);
                ImGui::SetCursorPos(ImVec2(12.f, 6.f));

                BeginSectionWithHeaderActions("Aimbot", ImVec2(ImGui::GetColumnWidth(), 0), true, []{
                    if (CenteredHotkeyButton(Text::Aimbot::HotKey.c_str(), ImVec2(54.f, 22.f)))
                    {
                        std::thread([&]() {
                            KeyMgr::GetPressedKey(AimControl::HotKey, &Text::Aimbot::HotKey);
                        }).detach();
                    }
                });
                    {
                        static const float FovMin = 0.f, FovMax = 30.f, MinFovMax = 1.f;
                        static const int BulletMin = 0, BulletMax = 5;
                        static const float SmoothMin = 0.f, SmoothMax = 10.f;
                        static const int MinHumanize = 0;
                        static const int MaxHumanize = 15;
                        struct VM_Aimbot { bool enabled; bool drawFov; bool visibleCheck; bool scopeOnly; int humanizationStrength; float aimFov; float aimFovMin; float smooth; int aimBullet; };
                        VM_Aimbot vm{};
                        if (!WeaponConfig::SelectedWeaponKey.empty()) {
                            auto key = WeaponConfig::SelectedWeaponKey;
                            auto it = WeaponConfig::WeaponConfigs.find(key);
                            if (it == WeaponConfig::WeaponConfigs.end()) {
                                WeaponConfig::WeaponProfile p;
                                p.aimEnabled = LegitBotConfig::AimBot;
                                p.activationMode = AimControl::ActivationMode;
                                p.visibleCheck = LegitBotConfig::VisibleCheck;
                                p.scopeOnly = AimControl::ScopeOnly;
                                p.humanizationStrength = AimControl::HumanizationStrength;
                                p.aimFov = AimControl::AimFov;
                                p.aimFovMin = AimControl::AimFovMin;
                                p.smooth = AimControl::Smooth;
                                p.aimBullet = AimControl::AimBullet;
                                p.hitboxes = AimControl::HitboxList;
                                WeaponConfig::WeaponConfigs[key] = p;
                                it = WeaponConfig::WeaponConfigs.find(key);
                            }
                            const auto& p = it->second;
                            vm.enabled = p.aimEnabled;
                            // activation mode handled via popup
                            vm.drawFov = ESPConfig::DrawFov;
                            vm.visibleCheck = p.visibleCheck;
                            vm.scopeOnly = p.scopeOnly;
                            vm.humanizationStrength = p.humanizationStrength;
                            vm.aimFov = p.aimFov;
                            vm.aimFovMin = p.aimFovMin;
                            vm.smooth = p.smooth;
                            vm.aimBullet = p.aimBullet;
                        } else {
                            vm.enabled = LegitBotConfig::AimBot;
                            // activation mode handled via popup
                            vm.drawFov = ESPConfig::DrawFov;
                            vm.visibleCheck = LegitBotConfig::VisibleCheck;
                            vm.scopeOnly = AimControl::ScopeOnly;
                            vm.humanizationStrength = AimControl::HumanizationStrength;
                            vm.aimFov = AimControl::AimFov;
                            vm.aimFovMin = AimControl::AimFovMin;
                            vm.smooth = AimControl::Smooth;
                            vm.aimBullet = AimControl::AimBullet;
                        }
                        PutSwitch(Text::Aimbot::Enable.c_str(), 0.f, ImGui::GetFrameHeight() * 1.7, &vm.enabled);
                        if (vm.enabled)
                        {
                        
                            PutSwitch(Text::Aimbot::DrawFov.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &vm.drawFov, true, "###FOVcol", reinterpret_cast<float*>(&LegitBotConfig::FovCircleColor));
                            PutSwitch(Text::Aimbot::VisCheck.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &vm.visibleCheck, false, NULL, NULL, Text::Aimbot::OnTip.c_str());
                            PutSwitch(Text::Aimbot::ScopeOnly.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &vm.scopeOnly);
                            PutSliderInt(Text::Aimbot::HumanizationStrength.c_str(), 10.f, &vm.humanizationStrength, &MinHumanize, &MaxHumanize, "%d");
                            PutSliderFloat(Text::Aimbot::FovSlider.c_str(), 10.f, &vm.aimFov, &vm.aimFovMin, &FovMax, "%.1f");
                            PutSliderFloat(Text::Aimbot::FovMinSlider.c_str(), 10.f, &vm.aimFovMin, &FovMin, &MinFovMax, "%.2f");
                            PutSliderFloat(Text::Aimbot::SmoothSlider.c_str(), 10.f, &vm.smooth, &SmoothMin, &SmoothMax, "%.1f", Text::Aimbot::OnlyAutoTip.c_str());
                            PutSliderInt(Text::Aimbot::BulletSlider.c_str(), 10.f, &vm.aimBullet, &BulletMin, &BulletMax, "%d", Text::Aimbot::StartBulletTip.c_str());
                        }
                        if (!WeaponConfig::SelectedWeaponKey.empty()) {
                            auto& p = WeaponConfig::WeaponConfigs[WeaponConfig::SelectedWeaponKey];
                            p.aimEnabled = vm.enabled;
                            
                            ESPConfig::DrawFov = vm.drawFov;
                            p.visibleCheck = vm.visibleCheck;
                            p.scopeOnly = vm.scopeOnly;
                            p.humanizationStrength = vm.humanizationStrength;
                            p.aimFov = vm.aimFov;
                            p.aimFovMin = vm.aimFovMin;
                            p.smooth = vm.smooth;
                            p.aimBullet = vm.aimBullet;
                        } else {
                            LegitBotConfig::AimBot = vm.enabled; ESPConfig::DrawFov = vm.drawFov; LegitBotConfig::VisibleCheck = vm.visibleCheck; AimControl::ScopeOnly = vm.scopeOnly; AimControl::HumanizationStrength = vm.humanizationStrength; AimControl::AimFov = vm.aimFov; AimControl::AimFovMin = vm.aimFovMin; AimControl::Smooth = vm.smooth; AimControl::AimBullet = vm.aimBullet;
                        }
                    }
                    EndSection();

				// Coluna 2: Triggerbot
                ImGui::NextColumn();
                ImGui::SetCursorPosY(6.f);
                BeginSectionWithHeaderActions("Triggerbot", ImVec2(ImGui::GetColumnWidth(), 0), true, []{
                    if (CenteredHotkeyButton(Text::Trigger::HotKey.c_str(), ImVec2(54.f, 22.f)))
                    {
                        std::thread([&]() {
                            KeyMgr::GetPressedKey(TriggerBot::HotKey, &Text::Trigger::HotKey);
                        }).detach();
                    }
                });
                    {
                        static const int DelayMin = 0, DelayMax = 300;
                        static const int DurationMin = 0, DurationMax = 1000;

                        auto vmTrig = BuildVM_Trigger();
                        PutSwitch(Text::Trigger::Enable.c_str(), 0.f, ImGui::GetFrameHeight() * 1.7, &vmTrig.enabled);
                        if (vmTrig.enabled)
                        {
                            
                            PutSwitch(Text::Trigger::ScopeOnly.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &vmTrig.scopeOnly);
                            
                            PutSwitch(Text::Trigger::StopOnly.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &vmTrig.stopOnly);
                            PutSwitch(Text::Trigger::TTDtimeout.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &vmTrig.ttdTimeout, false, NULL, NULL, Text::Aimbot::OnTip.c_str());
                            PutSliderInt(Text::Trigger::DelaySlider.c_str(), 5.f, &vmTrig.delay, &DelayMin, &DelayMax, "%d ms", Text::Trigger::DelayTip.c_str());
                            PutSliderInt(Text::Trigger::FakeShotSlider.c_str(), 5.f, &vmTrig.duration, &DurationMin, &DurationMax, "%d ms");
                        }
                        ApplyVM_Trigger(vmTrig);
                    }
                    EndSection();

					// Coluna 3: RCS (Recoil)
                ImGui::NextColumn();
                ImGui::SetCursorPosY(6.f);
            BeginSection("Recoil Control", ImVec2(ImGui::GetColumnWidth(), 0));
					{
						static const float recoilMin = 0.f, recoilMax = 2.f;
						static const int RCSBulletMin = 0, RCSBulletMax = 5;
                        auto vmRcs = BuildVM_RCS();
                        PutSwitch(Text::RCS::Toggle.c_str(), 0.f, ImGui::GetFrameHeight() * 1.7, &vmRcs.enabled);
                        if (vmRcs.enabled)
                        {
                            PutSliderInt(Text::RCS::BulletSlider.c_str(), 5.f, &vmRcs.bullet, &RCSBulletMin, &RCSBulletMax, "%d");
                            PutSliderFloat(Text::RCS::Yaw.c_str(), 5.f, &vmRcs.yaw, &recoilMin, &recoilMax, "%.2f");
                            PutSliderFloat(Text::RCS::Pitch.c_str(), 5.f, &vmRcs.pitch, &recoilMin, &recoilMax, "%.2f");
							float scalex = (2.22 - vmRcs.yaw) *.5f;
							float scaley = (2.12 - vmRcs.pitch) *.5f;
							ImVec2 BulletPos = ImGui::GetCursorScreenPos();

                            // Example Preview
                            ImVec2 BulletPos0, BulletPos1, BulletPos2, BulletPos3, BulletPos4, BulletPos5, BulletPos6, BulletPos7, BulletPos8, BulletPos9, BulletPos10, BulletPos11, BulletPos12, BulletPos13, BulletPos14, BulletPos15;
                            BulletPos.y += 123 * scaley;
                            BulletPos0.x = BulletPos.x + 125; BulletPos0.y = BulletPos.y + 5;
                            BulletPos1.x = BulletPos0.x - 3 * scalex; BulletPos1.y = BulletPos0.y - 5 * scaley;
                            BulletPos2.x = BulletPos1.x + 2 * scalex; BulletPos2.y = BulletPos1.y - 10 * scaley;
                            BulletPos3.x = BulletPos2.x + 4 * scalex; BulletPos3.y = BulletPos2.y - 11 * scaley;
                            BulletPos4.x = BulletPos3.x - 3 * scalex; BulletPos4.y = BulletPos3.y - 31 * scaley;
                            BulletPos5.x = BulletPos4.x - 1 * scalex; BulletPos5.y = BulletPos4.y - 20 * scaley;
                            BulletPos6.x = BulletPos5.x - 2 * scalex; BulletPos6.y = BulletPos5.y - 17 * scaley;
                            BulletPos7.x = BulletPos6.x - 15 * scalex; BulletPos7.y = BulletPos6.y - 9 * scaley;
                            BulletPos8.x = BulletPos7.x + 7 * scalex; BulletPos8.y = BulletPos7.y - 8 * scaley;
                            BulletPos9.x = BulletPos8.x + 33 * scalex; BulletPos9.y = BulletPos8.y + 2 * scaley;
                            BulletPos10.x = BulletPos9.x + 1 * scalex; BulletPos10.y = BulletPos9.y - 16 * scaley;
                            BulletPos11.x = BulletPos10.x - 9 * scalex; BulletPos11.y = BulletPos10.y + 20 * scaley;
                            BulletPos12.x = BulletPos11.x - 3 * scalex; BulletPos12.y = BulletPos11.y - 9 * scaley;
                            BulletPos13.x = BulletPos12.x + 15 * scalex; BulletPos13.y = BulletPos12.y - 5 * scaley;
                            BulletPos14.x = BulletPos13.x + 10 * scalex; BulletPos14.y = BulletPos13.y - 4 * scaley;

                            ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos0, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
                            ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos1, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
                            ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos2, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
                            ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos3, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
                            ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos4, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
                            ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos5, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
                            ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos6, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
                            ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos7, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
                            ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos8, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
                            ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos9, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
                            ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos10, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
                            ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos11, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
                            ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos12, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
                            ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos13, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));
                            ImGui::GetWindowDrawList()->AddCircleFilled(BulletPos14, 4.f, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));

							ImGui::SetCursorScreenPos(ImVec2(BulletPos.x, BulletPos.y + 10));
						}
                        ApplyVM_RCS(vmRcs);
					}
					EndSection();
					// Encerrar grade de 3 colunas
					ImGui::Columns(1);
				}


				if (MenuConfig::WCS.MenuPage == 2)
				{
					static const int FovMin = 60, FovMax = 140;
					static const int NightMin = 0, NightMax = 150;
					static const float FlashMin = 0.f, FlashMax = 255.f;
					ImGui::Columns(2, nullptr, false);
					ImGui::SetCursorPos(ImVec2(15.f, 24.f));

					BeginSection("Misc", ImVec2(ImGui::GetColumnWidth(), 0));
					{
                        PutSwitch(Text::Misc::bmbTimer.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::bmbTimer);
						PutSwitch(Text::Misc::SpecList.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::SpecList);
                        
                        
                        
						PutSwitch(Text::Misc::BunnyHop.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::BunnyHop, false, NULL, NULL, Text::Misc::InsecureTip.c_str());
                        
						PutSwitch("Auto Accept", 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::AutoAccept);
                        
					}
					EndSection();

					ImGui::NextColumn();
					ImGui::SetCursorPosY(24.f);
                    BeginSection("Global Settings", ImVec2(ImGui::GetColumnWidth(), 0));
                    {
                        auto vmGlobal = BuildVM_Global();
                        ImGui::TextDisabled(Text::Misc::MenuKey.c_str());
                        ImGui::SameLine(0.0f, 6.0f);
                        float miscHotkeyRowY = ImGui::GetCursorPosY();
                        AlignRight(60.f);
                        ImGui::SetCursorPosY(miscHotkeyRowY);
                        if (CenteredHotkeyButton(Text::Misc::HotKey.c_str(), ImVec2(60.f, 24.f)))
                        {
                            std::thread([&]() {
                                KeyMgr::GetPressedKey(vmGlobal.menuHotKey, &vmGlobal.menuHotKeyName);
                                }).detach();
                        }
                        PutSwitch(Text::Misc::SpecCheck.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &vmGlobal.workInSpec);
                        PutSwitch(Text::Misc::TeamCheck.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &vmGlobal.teamCheck);
                        

                        ApplyVM_Global(vmGlobal);
                    }
                    EndSection();

                    

					ImGui::Columns(1);
				}

                    // Página de Config removida temporariamente
					ImGui::NewLine();
			} ImGui::EndChild();
		} ImGui::End();

        ImGui::PopStyleColor(2);
        UpdateHotkeyLabels();
        LoadDefaultConfig();
    }
}
#include <functional>
