#include "OS-ImGui_Base.h"
// Ensure Windows and ImGui backends are available in this TU
#include <Windows.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "..\Resources\WeaponIcon.h"
#include "..\Resources\Font.h"
#include "..\Resources\Language.h"

namespace OSImGui
{
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
        
        ImFontConfig iconConfig;
        iconConfig.MergeMode = true;
        iconConfig.PixelSnapH = true;
        iconConfig.OversampleH = 3;
        iconConfig.OversampleV = 3;
        iconConfig.FontDataOwnedByAtlas = false;

        ImFont* WeaponIconFont = fontAtlas->AddFontFromMemoryTTF((void*)cs_icon, sizeof(cs_icon), 20.0f);

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
        style.ItemSpacing = ImVec2(6.0f, 4.0f);
        style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
        style.FramePadding = ImVec2(6.0f, 5.0f);
        style.WindowPadding = ImVec2(10.0f, 8.0f);

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