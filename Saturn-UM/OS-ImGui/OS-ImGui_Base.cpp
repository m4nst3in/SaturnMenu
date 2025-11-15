#include "OS-ImGui_Base.h"
#include <Windows.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "..\Resources\WeaponIcon.h"
#include "..\Resources\Font.h"
#include "..\Resources\Language.h"
#include "..\Resources\IconsFontAwesome.h"
#include "..\Resources\resource.h"

namespace OSImGui
{
    ImFont* TitleFont = nullptr;
    ImFont* TitleFontLarge = nullptr;
    bool FontAwesome6Available = false;
    bool OSImGui_Base::InitImGui(ID3D11Device* device, ID3D11DeviceContext* device_context)
    {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontDefault();
        (void)io;

        ImFontAtlas* fontAtlas = new ImFontAtlas();
        ImFontConfig arialConfig;
        arialConfig.FontDataOwnedByAtlas = false;

        ImFont* arialFont = fontAtlas->AddFontFromMemoryTTF((void*)MainFont, sizeof(MainFont), 20.0f, &arialConfig, fontAtlas->GetGlyphRangesDefault());
        
        HMODULE hModule = GetModuleHandleW(nullptr);
        HRSRC hRes = FindResourceW(hModule, MAKEINTRESOURCEW(IDR_FONT_AWESOME_SOLID), RT_RCDATA);
        bool faMerged = false;
        if (hRes)
        {
            HGLOBAL hData = LoadResource(hModule, hRes);
            if (hData)
            {
                void* pData = LockResource(hData);
                DWORD dataSize = SizeofResource(hModule, hRes);
                if (pData && dataSize > 0)
                {
                    ImFontConfig faConfig;
                    faConfig.MergeMode = true;
                    faConfig.PixelSnapH = true;
                    faConfig.OversampleH = 3;
                    faConfig.OversampleV = 3;
                    faConfig.FontDataOwnedByAtlas = false; // dados pertencem ao recurso
                    faConfig.GlyphMinAdvanceX = 15.0f; // largura mínima para ícones (mais compacto)
                    faConfig.GlyphOffset = ImVec2(0.0f, 1.0f); // leve ajuste vertical para centralizar

                    static const ImWchar fa_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
                    if (fontAtlas->AddFontFromMemoryTTF(pData, (int)dataSize, 18.0f, &faConfig, fa_ranges))
                        faMerged = true;
                }
            }
        }
        // Fallback: tentar carregar o TTF do disco caso o recurso não esteja acessível no binário
        if (!faMerged)
        {
            ImFontConfig faConfig;
            faConfig.MergeMode = true;
            faConfig.PixelSnapH = true;
            faConfig.OversampleH = 3;
            faConfig.OversampleV = 3;
            faConfig.GlyphMinAdvanceX = 15.0f;
            faConfig.GlyphOffset = ImVec2(0.0f, 1.0f);

            static const ImWchar fa_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
            // Tentativas comuns de caminho relativo durante desenvolvimento
            const char* faPaths[] = {
                "Resources/fa-solid-900.ttf",
                "Saturn-UM/Resources/fa-solid-900.ttf",
                "../Saturn-UM/Resources/fa-solid-900.ttf"
            };
            for (const char* p : faPaths)
            {
                if (fontAtlas->AddFontFromFileTTF(p, 18.0f, &faConfig, fa_ranges))
                {
                    faMerged = true;
                    break;
                }
            }
        }
        FontAwesome6Available = faMerged;

        // Fonte maior para títulos, evita blur ao ampliar (adicionada após mesclar FontAwesome na fonte padrão)
        ImFontConfig arialTitleConfig;
        arialTitleConfig.FontDataOwnedByAtlas = false;
        arialTitleConfig.OversampleH = 3;
        arialTitleConfig.OversampleV = 3;
        arialTitleConfig.PixelSnapH = true;
        TitleFont = fontAtlas->AddFontFromMemoryTTF((void*)MainFont, sizeof(MainFont), 34.0f, &arialTitleConfig, fontAtlas->GetGlyphRangesDefault());
        ImFontConfig arialTitleLargeConfig = arialTitleConfig;
        TitleFontLarge = fontAtlas->AddFontFromMemoryTTF((void*)MainFont, sizeof(MainFont), 42.0f, &arialTitleLargeConfig, fontAtlas->GetGlyphRangesDefault());

        // Carrega ícones internos (CS icons) em uma fonte separada (não mesclada)
        ImFont* WeaponIconFont = fontAtlas->AddFontFromMemoryTTF((void*)cs_icon, sizeof(cs_icon), 20.0f);

        // Garantir que a textura de fontes seja construída com os glyphs mesclados
        fontAtlas->Build();
        io.Fonts = fontAtlas;
        io.FontDefault = arialFont;

        ImGui::SaturnDefaultStyle();

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 6.0f;
        style.ChildRounding = 4.0f;
        style.FrameRounding = 2.0f;
        style.TabRounding = 3.0f;
        style.ScrollbarRounding = 4.0f;
        style.WindowBorderSize = 1.0f;
        style.FrameBorderSize = 1.0f;
        style.ItemSpacing = ImVec2(5.0f, 3.0f);
        style.ItemInnerSpacing = ImVec2(4.0f, 3.0f);
        style.FramePadding = ImVec2(5.0f, 3.0f);
        style.WindowPadding = ImVec2(8.0f, 6.0f);
        style.CurveTessellationTol = 0.8f;

        style.Colors[ImGuiCol_WindowBg]        = ImVec4(0.08f, 0.09f, 0.10f, 1.00f);
        style.Colors[ImGuiCol_ChildBg]         = ImVec4(0.09f, 0.10f, 0.12f, 1.00f);
        style.Colors[ImGuiCol_PopupBg]         = ImVec4(0.08f, 0.09f, 0.10f, 0.98f);
        style.Colors[ImGuiCol_Border]          = ImVec4(0.20f, 0.24f, 0.30f, 0.60f);
        style.Colors[ImGuiCol_BorderShadow]    = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_FrameBg]         = ImVec4(0.13f, 0.16f, 0.20f, 1.00f);
        style.Colors[ImGuiCol_FrameBgHovered]  = ImVec4(0.16f, 0.22f, 0.30f, 0.85f);
        style.Colors[ImGuiCol_FrameBgActive]   = ImVec4(0.13f, 0.16f, 0.20f, 1.00f);
        style.Colors[ImGuiCol_Button]          = ImVec4(0.12f, 0.28f, 0.48f, 0.50f);
        style.Colors[ImGuiCol_ButtonHovered]   = ImVec4(0.16f, 0.44f, 0.80f, 0.90f);
        style.Colors[ImGuiCol_ButtonActive]    = ImVec4(0.09f, 0.38f, 0.85f, 1.00f);
        style.Colors[ImGuiCol_CheckMark]       = ImVec4(1.00f, 1.00f, 1.00f, 0.95f);
        style.Colors[ImGuiCol_SliderGrab]      = ImVec4(0.18f, 0.56f, 0.95f, 1.00f);
        style.Colors[ImGuiCol_SliderGrabActive]= ImVec4(0.22f, 0.62f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_Header]          = ImVec4(0.11f, 0.28f, 0.52f, 0.55f);
        style.Colors[ImGuiCol_HeaderHovered]   = ImVec4(0.16f, 0.44f, 0.80f, 0.90f);
        style.Colors[ImGuiCol_HeaderActive]    = ImVec4(0.09f, 0.38f, 0.85f, 1.00f);
        style.Colors[ImGuiCol_Separator]       = ImVec4(0.16f, 0.22f, 0.30f, 0.60f);
        style.Colors[ImGuiCol_SeparatorHovered]= ImVec4(0.16f, 0.44f, 0.80f, 0.78f);
        style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.16f, 0.44f, 0.80f, 1.00f);
        style.Colors[ImGuiCol_Tab]             = ImVec4(0.11f, 0.28f, 0.52f, 0.60f);
        style.Colors[ImGuiCol_TabHovered]      = ImVec4(0.16f, 0.44f, 0.80f, 0.90f);
        style.Colors[ImGuiCol_TabActive]       = ImVec4(0.12f, 0.36f, 0.68f, 1.00f);
        style.Colors[ImGuiCol_TitleBg]         = ImVec4(0.05f, 0.06f, 0.08f, 1.00f);
        style.Colors[ImGuiCol_TitleBgActive]   = ImVec4(0.05f, 0.06f, 0.08f, 1.00f);
        style.Colors[ImGuiCol_Text]            = ImVec4(0.90f, 0.93f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled]    = ImVec4(0.60f, 0.68f, 0.78f, 1.00f);

        io.LogFilename = nullptr;
        io.LogFilename = nullptr;

        if (!ImGui_ImplWin32_Init(Window.hWnd))
            throw OSException("ImGui_ImplWin32_Init() call failed.");
        if (!ImGui_ImplDX11_Init(device, device_context))
            throw OSException("ImGui_ImplDX11_Init() call failed.");

        return true;
    }

    void OSImGui_Base::CleanImGui()
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        // Only available in builds where D3D device helpers are compiled
#ifdef _CONSOLE
        g_Device.CleanupDeviceD3D();
#endif
        if (Window.hWnd)
            DestroyWindow(Window.hWnd);
        if (!Window.ClassName.empty() && Window.hInstance)
            UnregisterClassA(Window.ClassName.c_str(), Window.hInstance);
    }

    std::wstring OSImGui_Base::StringToWstring(const std::string& str)
    {
        if (str.empty()) return L"";
        int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
        if (len <= 0) return L"";
        std::wstring wstatic(len - 1, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wstatic.data(), len);
        return wstatic;
    }
}