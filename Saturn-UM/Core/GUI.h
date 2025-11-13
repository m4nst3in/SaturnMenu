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
#include "../Helpers/KeyManager.h"
#include "..\OS-ImGui\OS-ImGui_Base.h"
// Ícones FontAwesome nas labels
#include "..\Resources\IconsFontAwesome.h"

#include "../Features/ESP.h"

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
    ImGui::SameLine(0.0f, 6.0f);
    const float btnW = 54.f, btnH = 22.f;
    const float iconSize = ImGui::GetFontSize() * 0.85f;
    const float iconW = ImGui::CalcTextSize(ICON_FA_KEYBOARD).x * (iconSize / ImGui::GetFontSize());
    AlignRight(btnW + iconW + 4.0f);
    float yBase = ImGui::GetCursorPosY();
    ImGui::SetCursorPosY(yBase + (ImGui::GetTextLineHeight() - btnH) * 0.5f + (btnH - iconSize) * 0.5f);
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImU32 col = ImGui::GetColorU32(ImVec4(0.7f,0.7f,0.7f,1.0f));
        dl->AddText(ImGui::GetFont(), iconSize, p, col, ICON_FA_KEYBOARD);
        ImGui::Dummy(ImVec2(iconW, iconSize));
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
                    ImGui::SetCursorPosX(12.0f);
                    if (titleFont) ImGui::PushFont(titleFont);
                    GradientText("Saturn", start, end, title_size, titleFont);
                    if (titleFont) ImGui::PopFont();
                    // Abas à direita do título (mesma linha, com espaçamento menor)
                    ImGui::SameLine(0.0f, 10.0f);
                    // diminuir a altura das abas para ficarem simétricas com a base das letras do título
                    float tab_pad_y = style.FramePadding.y * 0.60f; // ~40% menos altura
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, tab_pad_y));
                    // centralizar verticalmente as abas dentro do cabeçalho
                    float tab_frame_h = ImGui::GetFrameHeight();
                    float tabs_y = top_pad_y + (title_size - tab_frame_h) * 0.5f;
                    ImGui::SetCursorPosY(tabs_y);
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
                    ImGui::PopStyleVar(2);
                    ImGui::PopStyleColor(2);
                    ImGui::PopStyleVar();
                }
                ImGui::EndChild();
                ImGui::PopStyleVar();

			// Área de conteúdo abaixo das abas
			ImGui::BeginChild("Page", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar);
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
						PutSwitch(Text::ESP::Enable.c_str(), 0.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ESPenabled);
                        
                        

                        if (ESPConfig::ESPenabled)
                        {
                            PutSwitch(Text::ESP::Box.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowBoxESP, true, "###BoxCol", reinterpret_cast<float*>(&ESPConfig::BoxColor));
                            if (ESPConfig::ShowBoxESP)
                            {
                                PutSwitch(Text::ESP::Outline.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::OutLine);
                                {
                                    bool isBoxStyle = (ESPConfig::BoxType == 0);
                                    PutSwitch(Text::ESP::BoxType.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7f, &isBoxStyle);
                                    ESPConfig::BoxType = isBoxStyle ? 0 : 1;
                                }
                            }
                            
                            PutSwitch(Text::ESP::Skeleton.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowBoneESP, true, "###BoneCol", reinterpret_cast<float*>(&ESPConfig::BoneColor));
                            
                            
                            PutSwitch(Text::ESP::OutOfFOVArrow.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowOutOfFOVArrow, true, "###OutFOVCol", reinterpret_cast<float*>(&ESPConfig::OutOfFOVArrowColor));
                            if(ESPConfig::ShowOutOfFOVArrow)
                            PutSliderFloat(Text::ESP::OutOfFOVRadius.c_str(), .5f, &ESPConfig::OutOfFOVRadiusFactor, &MinFovFactor, &MaxFovFactor, "%.1f");

                            
                            PutSwitch(Text::ESP::HealthBar.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowHealthBar);
                            
                            PutSwitch(Text::ESP::ShowArmorBar.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ArmorBar);
                            
                            PutSwitch(Text::ESP::Weapon.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowWeaponESP);
                            PutSwitch(Text::ESP::Ammo.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::AmmoBar);
                            PutSwitch(Text::ESP::Distance.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowDistance);
                            PutSwitch(Text::ESP::PlayerName.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowPlayerName);
                            PutSwitch(Text::ESP::ScopedESP.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowIsScoped);
                            
                            PutSwitch(Text::ESP::VisCheck.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::VisibleCheck, true, "###VisibleCol", reinterpret_cast<float*>(&ESPConfig::VisibleColor));
                        }
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
						PutSwitch(Text::Radar::Toggle.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &RadarCFG::ShowRadar);
						if (RadarCFG::ShowRadar)
						{
							PutSwitch(Text::Radar::CustomCheck.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &RadarCFG::customRadar);
							if (RadarCFG::customRadar)
							{
								PutSwitch(Text::Radar::CrossLine.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &RadarCFG::ShowRadarCrossLine);
								PutSliderFloat(Text::Radar::SizeSlider.c_str(), 5.f, &RadarCFG::RadarPointSizeProportion, &RadarPointSizeProportionMin, &RadarPointSizeProportionMax, "%.1f");
								PutSliderFloat(Text::Radar::ProportionSlider.c_str(), 5.f, &RadarCFG::Proportion, &ProportionMin, &ProportionMax, "%.1f");
								PutSliderFloat(Text::Radar::RangeSlider.c_str(), 5.f, &RadarCFG::RadarRange, &RadarRangeMin, &RadarRangeMax, "%.1f");
								PutSliderFloat(Text::Radar::AlphaSlider.c_str(), 5.f, &RadarCFG::RadarBgAlpha, &AlphaMin, &AlphaMax, "%.1f");
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
						PutSwitch(Text::Aimbot::Enable.c_str(), 0.f, ImGui::GetFrameHeight() * 1.7, &LegitBotConfig::AimBot);
						if (LegitBotConfig::AimBot)
						{
                            
                            
                            
							PutSwitch(Text::Aimbot::Toggle.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &LegitBotConfig::AimToggleMode, false, NULL, NULL, Text::Aimbot::OffTip.c_str());
							PutSwitch(Text::Aimbot::DrawFov.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::DrawFov, true, "###FOVcol", reinterpret_cast<float*>(&LegitBotConfig::FovCircleColor));
							PutSwitch(Text::Aimbot::VisCheck.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &LegitBotConfig::VisibleCheck, false, NULL, NULL, Text::Aimbot::OnTip.c_str());
                            
							PutSwitch(Text::Aimbot::ScopeOnly.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &AimControl::ScopeOnly);

                            PutSliderInt(Text::Aimbot::HumanizationStrength.c_str(), 10.f, &AimControl::HumanizationStrength, &MinHumanize, &MaxHumanize, "%d");

							PutSliderFloat(Text::Aimbot::FovSlider.c_str(), 10.f, &AimControl::AimFov, &AimControl::AimFovMin, &FovMax, "%.1f");
							PutSliderFloat(Text::Aimbot::FovMinSlider.c_str(), 10.f, &AimControl::AimFovMin, &FovMin, &MinFovMax, "%.2f");
                            PutSliderFloat(Text::Aimbot::SmoothSlider.c_str(), 10.f, &AimControl::Smooth, &SmoothMin, &SmoothMax, "%.1f", Text::Aimbot::OnlyAutoTip.c_str());
                            PutSliderInt(Text::Aimbot::BulletSlider.c_str(), 10.f, &AimControl::AimBullet, &BulletMin, &BulletMax, "%d", Text::Aimbot::StartBulletTip.c_str());
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

						PutSwitch(Text::Trigger::Enable.c_str(), 0.f, ImGui::GetFrameHeight() * 1.7, &LegitBotConfig::TriggerBot);
						if (LegitBotConfig::TriggerBot)
						{
                            PutSwitch(Text::Trigger::Toggle.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &LegitBotConfig::TriggerAlways, false, NULL, NULL, Text::Aimbot::OffTip.c_str());
							PutSwitch(Text::Trigger::ScopeOnly.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &TriggerBot::ScopeOnly);
                            
							PutSwitch(Text::Trigger::StopOnly.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &TriggerBot::StopedOnly);
							PutSwitch(Text::Trigger::TTDtimeout.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &TriggerBot::TTDtimeout, false, NULL, NULL, Text::Aimbot::OnTip.c_str());
							PutSliderInt(Text::Trigger::DelaySlider.c_str(), 5.f, &TriggerBot::TriggerDelay, &DelayMin, &DelayMax, "%d ms", Text::Trigger::DelayTip.c_str());
							PutSliderInt(Text::Trigger::FakeShotSlider.c_str(), 5.f, &TriggerBot::ShotDuration, &DurationMin, &DurationMax, "%d ms");
						}
					}
					EndSection();

					// Coluna 3: RCS (Recoil)
                ImGui::NextColumn();
                ImGui::SetCursorPosY(6.f);
            BeginSection("Recoil Control", ImVec2(ImGui::GetColumnWidth(), 0));
					{
						static const float recoilMin = 0.f, recoilMax = 2.f;
						static const int RCSBulletMin = 0, RCSBulletMax = 5;
						PutSwitch(Text::RCS::Toggle.c_str(), 0.f, ImGui::GetFrameHeight() * 1.7, &LegitBotConfig::RCS);
						if (LegitBotConfig::RCS)
						{
							PutSliderInt(Text::RCS::BulletSlider.c_str(), 5.f, &RCS::RCSBullet, &RCSBulletMin, &RCSBulletMax, "%d");
							PutSliderFloat(Text::RCS::Yaw.c_str(), 5.f, &RCS::RCSScale.x, &recoilMin, &recoilMax, "%.2f");
							PutSliderFloat(Text::RCS::Pitch.c_str(), 5.f, &RCS::RCSScale.y, &recoilMin, &recoilMax, "%.2f");
							float scalex = (2.22 - RCS::RCSScale.x) *.5f;
							float scaley = (2.12 - RCS::RCSScale.y) *.5f;//Simulate reasonable error values
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
						PutSwitch(Text::Misc::bmbTimer.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::bmbTimer, true, "###bmbTimerCol", reinterpret_cast<float*>(&MiscCFG::BombTimerCol));
						PutSwitch(Text::Misc::SpecList.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::SpecList);
						PutSwitch(Text::Misc::Watermark.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::WaterMark);
                        
                        
						PutSwitch(Text::Misc::HitMerker.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::HitMarker);
						PutSwitch(Text::Misc::BunnyHop.c_str(), 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::BunnyHop, false, NULL, NULL, Text::Misc::InsecureTip.c_str());
                        
						PutSwitch("Auto Accept", 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::AutoAccept);
						PutSwitch("Anti-afk", 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::AntiAFKKick);
					}
					EndSection();

					ImGui::NextColumn();
					ImGui::SetCursorPosY(24.f);
					BeginSection("Global Settings", ImVec2(ImGui::GetColumnWidth(), 0));
					{
                        ImGui::TextDisabled(Text::Misc::MenuKey.c_str());
                        ImGui::SameLine(0.0f, 6.0f);
                        float miscHotkeyRowY = ImGui::GetCursorPosY();
                        AlignRight(60.f);
                        ImGui::SetCursorPosY(miscHotkeyRowY);
                        if (CenteredHotkeyButton(Text::Misc::HotKey.c_str(), ImVec2(60.f, 24.f)))
                        {
                            std::thread([&]() {
                                KeyMgr::GetPressedKey(MenuConfig::HotKey, &Text::Misc::HotKey);
                                }).detach();
                        }
						PutSwitch(Text::Misc::SpecCheck.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &MenuConfig::WorkInSpec);
						PutSwitch(Text::Misc::TeamCheck.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &MenuConfig::TeamCheck);
						PutSwitch(Text::Misc::AntiRecord.c_str(), 5.f, ImGui::GetFrameHeight() * 1.7, &MenuConfig::BypassOBS);

                        // Removed external and maintenance buttons per user request
                        // (Source Code, Contact Author, Unhook, Clear Traces)
					}
					EndSection();

					ImGui::Columns(1);
				}

                    // Página de Config removida temporariamente
					ImGui::NewLine();
			} ImGui::EndChild();
		} ImGui::End();

        // Reverter cores de fundo apenas para esta janela
        ImGui::PopStyleColor(2);

		LoadDefaultConfig();
	}
}
#include <functional>
