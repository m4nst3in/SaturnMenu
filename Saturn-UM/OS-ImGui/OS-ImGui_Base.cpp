#include "OS-ImGui_Base.h"
// Ensure Windows and ImGui backends are available in this TU
#include <Windows.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <filesystem>
#include <urlmon.h>

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
        namespace fs = std::filesystem;
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontDefault();
        (void)io;

        ImFontAtlas* fontAtlas = new ImFontAtlas();
        ImFont* uiFont = nullptr;

        // Fonte principal: tentar Noto Sans Condensed Bold do Assets; fallback para fonte embutida
        {
            ImFontConfig notoCfg;
            notoCfg.OversampleH = 3;
            notoCfg.OversampleV = 3;
            notoCfg.PixelSnapH = true;
            const char* notoPaths[] = {
                "Assets/NotoSans_Condensed-Bold.ttf",
                "../Assets/NotoSans_Condensed-Bold.ttf",
                "../../Assets/NotoSans_Condensed-Bold.ttf"
            };
            for (const char* p : notoPaths)
            {
                if (uiFont == nullptr && fs::exists(p))
                    uiFont = fontAtlas->AddFontFromFileTTF(p, 18.0f, &notoCfg, fontAtlas->GetGlyphRangesDefault());
            }

            if (uiFont == nullptr)
            {
                ImFontConfig memCfg;
                memCfg.FontDataOwnedByAtlas = false;
                memCfg.OversampleH = 3;
                memCfg.OversampleV = 3;
                memCfg.PixelSnapH = true;
                uiFont = fontAtlas->AddFontFromMemoryTTF((void*)MainFont, sizeof(MainFont), 20.0f, &memCfg, fontAtlas->GetGlyphRangesDefault());
            }
        }
        // Mesclar FA6 adicionalmente (mesmo se já mesclado via recurso), para garantir ícones novos
        {
            ImFontConfig faConfig;
            faConfig.MergeMode = true;
            faConfig.PixelSnapH = true;
            faConfig.OversampleH = 3;
            faConfig.OversampleV = 3;
            faConfig.GlyphMinAdvanceX = 15.0f;
            faConfig.GlyphOffset = ImVec2(0.0f, 1.0f);
            static const ImWchar fa_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
            const wchar_t* url = L"https://raw.githubusercontent.com/FortAwesome/Font-Awesome/6.x/webfonts/fa-solid-900.ttf";
            std::wstring dest = StringToWstring("Saturn-UM/Resources/fa6-solid.ttf");
            std::error_code ec;
            std::filesystem::create_directories("Saturn-UM/Resources", ec);
            URLDownloadToFileW(nullptr, url, dest.c_str(), 0, nullptr);
            if (std::filesystem::exists("Saturn-UM/Resources/fa6-solid.ttf"))
            {
                if (fontAtlas->AddFontFromFileTTF("Saturn-UM/Resources/fa6-solid.ttf", 18.0f, &faConfig, fa_ranges))
                    FontAwesome6Available = true;
            }
        }
        
        // Mesclar a fonte FontAwesome na fonte padrão para uso de ícones nas labels
        // A fonte é embutida como recurso RCDATA (IDR_FONT_AWESOME_SOLID) em Resources/Resource.rc
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

            const wchar_t* url = L"https://raw.githubusercontent.com/FortAwesome/Font-Awesome/6.x/webfonts/fa-solid-900.ttf";
            std::wstring dest = StringToWstring("Saturn-UM/Resources/fa6-solid.ttf");
            fs::create_directories("Saturn-UM/Resources");
            URLDownloadToFileW(nullptr, url, dest.c_str(), 0, nullptr);
            if (fs::exists("Saturn-UM/Resources/fa6-solid.ttf"))
            {
                if (fontAtlas->AddFontFromFileTTF("Saturn-UM/Resources/fa6-solid.ttf", 18.0f, &faConfig, fa_ranges))
                {
                    faMerged = true;
                }
            }
            if (!faMerged)
            {
                const char* faPaths[] = {
                    "Resources/fa-solid-900.ttf",
                    "Saturn-UM/Resources/fa-solid-900.ttf",
                    "../Saturn-UM/Resources/fa-solid-900.ttf"
                };
                for (const char* p : faPaths)
                {
                    if (fs::exists(p))
                    {
                        if (fontAtlas->AddFontFromFileTTF(p, 18.0f, &faConfig, fa_ranges))
                        {
                            faMerged = true;
                            break;
                        }
                    }
                }
            }
        }

        // Fonte para títulos (média)
        {
            ImFontConfig titleCfg;
            titleCfg.OversampleH = 3;
            titleCfg.OversampleV = 3;
            titleCfg.PixelSnapH = true;
            const char* notoPathsTitle[] = {
                "Assets/NotoSans_Condensed-Bold.ttf",
                "../Assets/NotoSans_Condensed-Bold.ttf",
                "../../Assets/NotoSans_Condensed-Bold.ttf"
            };
            for (const char* p : notoPathsTitle)
            {
                if (TitleFont == nullptr && fs::exists(p))
                    TitleFont = fontAtlas->AddFontFromFileTTF(p, 22.0f, &titleCfg, fontAtlas->GetGlyphRangesDefault());
            }
            if (TitleFont == nullptr)
            {
                ImFontConfig memCfg;
                memCfg.FontDataOwnedByAtlas = false;
                memCfg.OversampleH = 3;
                memCfg.OversampleV = 3;
                memCfg.PixelSnapH = true;
                TitleFont = fontAtlas->AddFontFromMemoryTTF((void*)MainFont, sizeof(MainFont), 24.0f, &memCfg, fontAtlas->GetGlyphRangesDefault());
            }
        }

        // Fonte maior para títulos (evita blur por escala)
        {
            ImFontConfig titleCfgLarge;
            titleCfgLarge.OversampleH = 3;
            titleCfgLarge.OversampleV = 3;
            titleCfgLarge.PixelSnapH = true;
            const char* notoPathsTitleLarge[] = {
                "Assets/NotoSans_Condensed-Bold.ttf",
                "../Assets/NotoSans_Condensed-Bold.ttf",
                "../../Assets/NotoSans_Condensed-Bold.ttf"
            };
            for (const char* p : notoPathsTitleLarge)
            {
                if (TitleFontLarge == nullptr && fs::exists(p))
                    TitleFontLarge = fontAtlas->AddFontFromFileTTF(p, 34.0f, &titleCfgLarge, fontAtlas->GetGlyphRangesDefault());
            }
            if (TitleFontLarge == nullptr)
            {
                ImFontConfig memCfgLarge;
                memCfgLarge.FontDataOwnedByAtlas = false;
                memCfgLarge.OversampleH = 3;
                memCfgLarge.OversampleV = 3;
                memCfgLarge.PixelSnapH = true;
                TitleFontLarge = fontAtlas->AddFontFromMemoryTTF((void*)MainFont, sizeof(MainFont), 36.0f, &memCfgLarge, fontAtlas->GetGlyphRangesDefault());
            }
        }

        // Carrega ícones internos (CS icons) em uma fonte separada (não mesclada)
        ImFont* WeaponIconFont = fontAtlas->AddFontFromMemoryTTF((void*)cs_icon, sizeof(cs_icon), 20.0f);

        // Garantir que a textura de fontes seja construída com os glyphs mesclados
        fontAtlas->Build();
        io.Fonts = fontAtlas;
        io.FontDefault = uiFont;

        ImGui::SaturnDefaultStyle();

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 8.0f;
        style.ChildRounding = 6.0f;
        style.FrameRounding = 6.0f;
        style.TabRounding = 6.0f;
        style.TabBorderSize = 0.0f;
        style.GrabRounding = 6.0f;
        style.ScrollbarRounding = 6.0f;
        style.WindowBorderSize = 1.0f;
        style.FrameBorderSize = 1.0f;
        style.ItemSpacing = ImVec2(8.0f, 8.0f);
        style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);
        style.FramePadding = ImVec2(10.0f, 8.0f);
        style.WindowPadding = ImVec2(12.0f, 10.0f);

        // Cosmic Dark palette
        const ImVec4 accent        = ImVec4(0.486f, 0.227f, 0.929f, 1.00f); // #7C3AED
        const ImVec4 accentHover   = ImVec4(0.560f, 0.320f, 0.950f, 1.00f);
        const ImVec4 accentActive  = ImVec4(0.400f, 0.180f, 0.800f, 1.00f);
        const ImVec4 textPrimary   = ImVec4(0.902f, 0.902f, 0.941f, 1.00f); // #E6E6F0
        const ImVec4 textSecondary = ImVec4(0.639f, 0.654f, 0.780f, 1.00f); // #A3A7C7

        style.Colors[ImGuiCol_WindowBg]         = ImVec4(0.045f, 0.063f, 0.125f, 1.00f); // #0B1020
        style.Colors[ImGuiCol_ChildBg]          = ImVec4(0.070f, 0.090f, 0.200f, 1.00f); // #121833
        style.Colors[ImGuiCol_PopupBg]          = ImVec4(0.045f, 0.063f, 0.125f, 0.98f);
        style.Colors[ImGuiCol_Border]           = ImVec4(1.00f, 1.00f, 1.00f, 0.08f);
        style.Colors[ImGuiCol_BorderShadow]     = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_FrameBg]          = ImVec4(0.100f, 0.130f, 0.260f, 1.00f);
        style.Colors[ImGuiCol_FrameBgHovered]   = ImVec4(0.120f, 0.160f, 0.300f, 1.00f);
        style.Colors[ImGuiCol_FrameBgActive]    = ImVec4(0.090f, 0.120f, 0.240f, 1.00f);
        style.Colors[ImGuiCol_Button]           = accent;
        style.Colors[ImGuiCol_ButtonHovered]    = accentHover;
        style.Colors[ImGuiCol_ButtonActive]     = accentActive;
        style.Colors[ImGuiCol_CheckMark]        = accent;
        style.Colors[ImGuiCol_SliderGrab]       = accent;
        style.Colors[ImGuiCol_SliderGrabActive] = accentActive;
        style.Colors[ImGuiCol_Header]           = ImVec4(0.120f, 0.160f, 0.300f, 1.00f);
        style.Colors[ImGuiCol_HeaderHovered]    = ImVec4(0.140f, 0.180f, 0.340f, 1.00f);
        style.Colors[ImGuiCol_HeaderActive]     = ImVec4(0.110f, 0.140f, 0.260f, 1.00f);
        style.Colors[ImGuiCol_Separator]        = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
        style.Colors[ImGuiCol_SeparatorHovered] = accentHover;
        style.Colors[ImGuiCol_SeparatorActive]  = accentActive;
        style.Colors[ImGuiCol_Tab]              = ImVec4(0.120f, 0.120f, 0.140f, 1.00f);
        style.Colors[ImGuiCol_TabHovered]       = accentHover;
        style.Colors[ImGuiCol_TabActive]        = accent;
        style.Colors[ImGuiCol_TitleBg]          = ImVec4(0.045f, 0.063f, 0.125f, 1.00f);
        style.Colors[ImGuiCol_TitleBgActive]    = ImVec4(0.060f, 0.080f, 0.150f, 1.00f);
        style.Colors[ImGuiCol_Text]             = textPrimary;
        style.Colors[ImGuiCol_TextDisabled]     = textSecondary;

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
