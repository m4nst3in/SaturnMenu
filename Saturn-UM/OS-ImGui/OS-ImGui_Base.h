#pragma once
// Core includes for ImGui/DirectX/Windows
#include "OS-ImGui_Struct.h"
#include "OS-ImGui_Exception.h"
#include <Windows.h>
#include "imgui/imgui.h"
#include <d3d11.h>
#include <dxgi.h>
#include <dwmapi.h>
// std includes
#include <iostream>
#include <string>
#include <functional>
#include <vector>

#pragma comment(lib,"dwmapi.lib")

namespace OSImGui
{
    // Fonte grande para títulos (exposta para uso no GUI)
    extern ImFont* TitleFont;
	class D3DDevice
	{
	public:
		ID3D11Device* g_pd3dDevice = nullptr;
		ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
		IDXGISwapChain* g_pSwapChain = nullptr;
		ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
#ifdef _CONSOLE
		bool CreateDeviceD3D(HWND hWnd);
		void CleanupDeviceD3D();
		void CreateRenderTarget();
		void CleanupRenderTarget();
#endif
	};

	inline D3DDevice g_Device;

	enum WindowType
	{
		NEW,
		ATTACH
	};

	class WindowData
	{
	public:
		HWND  hWnd = NULL;
		HINSTANCE hInstance = nullptr;
		std::string Name;
		std::wstring wName;
		std::string ClassName;
		std::wstring wClassName;
		Vec2 Pos;
		Vec2 Size;
		ImColor BgColor{ 255, 255, 255 };
	};

    class OSImGui_Base
    {
    public:
        std::function<void()> CallBackFn = nullptr;
        bool EndFlag = false;
    public:
        WindowData Window;
        WindowData DestWindow;
    public:
        virtual void NewWindow(std::string WindowName, Vec2 WindowSize, std::function<void()> CallBack) = 0;
        virtual void Quit() { EndFlag = true; };
    public:
        virtual bool CreateMyWindow() = 0;
        virtual void MainLoop() {};
        bool InitImGui(ID3D11Device* device, ID3D11DeviceContext* device_context);
        void CleanImGui();
        std::wstring StringToWstring(const std::string& str);
    };
}