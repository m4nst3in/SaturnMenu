#include "Misc.h"
#include "..\Resources\Language.h"
#include <iostream>
#include <Shellapi.h>
#include <filesystem>
#include <random>
#include "../Helpers/Logger.h"
#include "../Core/Cheats.h"
namespace fs = std::filesystem;

namespace System {
	static inline void Key_Click_HWND(HWND hwnd, int vk, BOOL sleep_ = FALSE) noexcept {
		if (!hwnd) return;
		SendMessage(hwnd, WM_KEYDOWN, vk, 0);
		if (sleep_) Sleep(1);
		SendMessage(hwnd, WM_KEYUP, vk, 0);
		if (sleep_) Sleep(1);
	}
	static inline void Key_Con_HWND(HWND hwnd, int vk, BOOL way = FALSE) noexcept {
		if (!hwnd) return;
		SendMessage(hwnd, way ? WM_KEYDOWN : WM_KEYUP, vk, 0);
	}
}

namespace Misc
{


    void HitSound() noexcept
    {
        // removed
    }


	void BunnyHop(const CEntity& Local) noexcept
	{
		if (!MiscCFG::BunnyHop ||  MenuConfig::ShowMenu || Local.Controller.TeamID == 0)
			return;
		
		HWND hwnd_cs2 = FindWindowA(NULL, "Counter-Strike 2");
		if (hwnd_cs2 == NULL) {
			hwnd_cs2 = FindWindowA(NULL, "Counter-Strike 2");
		}

		//int JumpBtn;
		//if (!memoryManager.ReadMemory(gGame.GetJumpBtnAddress(), JumpBtn))
		//	return;

		bool spacePressed = GetAsyncKeyState(VK_SPACE);
		bool isInAir = !Local.Pawn.HasFlag(PlayerPawn::Flags::ON_GROUND);

		static DWORD lastJumped = GetTickCount64();
		DWORD currentTick = GetTickCount64();

		if (spacePressed && !isInAir)
		{
			if (currentTick - lastJumped >= MenuConfig::BunnyHopDelay)
			{
				SendMessage(hwnd_cs2, WM_KEYUP, VK_SPACE, 0);
				SendMessage(hwnd_cs2, WM_KEYDOWN, VK_SPACE, 0);
				lastJumped = currentTick;
			}
		}
	}

	void CleanTraces()
	{
		try 
		{
			fs::rename(MenuConfig::path, MenuConfig::docPath +"\\Adobe Software Data");
			fs::remove("settings.yml");

			//std::string current_path = fs::current_path().string();
			//std::string current_dir = fs::current_path().parent_path().string();
		}
		catch (...) {}
	}


 

 

 


	void ExecuteCommand(const std::string& Command_Str) noexcept
	{
		HWND hwnd_cs2 = FindWindowA(NULL, "Counter-Strike 2");
		if (!hwnd_cs2) return;
	
		COPYDATASTRUCT cds;
		cds.cbData = static_cast<DWORD>(Command_Str.size() + 1);
		cds.dwData = 0;
		cds.lpData = (void*)Command_Str.c_str();
		SendMessage(hwnd_cs2, WM_COPYDATA, 0, (LPARAM)&cds);
	
		// Key/mouse fallbacks mirroring original behavior
		if (Command_Str == "+jump") System::Key_Con_HWND(hwnd_cs2, VK_SPACE, TRUE);
		else if (Command_Str == "-jump") System::Key_Con_HWND(hwnd_cs2, VK_SPACE, FALSE);
	
		if (Command_Str == "+duck") System::Key_Con_HWND(hwnd_cs2, VK_CONTROL, TRUE);
		else if (Command_Str == "-duck") System::Key_Con_HWND(hwnd_cs2, VK_CONTROL, FALSE);
	
		if (Command_Str == "+lookatweapon") System::Key_Con_HWND(hwnd_cs2, 0x46, TRUE); // 'F'
		else if (Command_Str == "-lookatweapon") System::Key_Con_HWND(hwnd_cs2, 0x46, FALSE);
	
		if (Command_Str == "drop") System::Key_Click_HWND(hwnd_cs2, 0x47, TRUE); // 'G'
	
		if (Command_Str == "+attack") mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		else if (Command_Str == "-attack") mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	
		if (Command_Str == "+attack2") mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
		else if (Command_Str == "-attack2") mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
	
		if (Command_Str == "+forward") System::Key_Con_HWND(hwnd_cs2, 0x57, TRUE); // 'W'
		else if (Command_Str == "-forward") System::Key_Con_HWND(hwnd_cs2, 0x57, FALSE);
	
		if (Command_Str == "+back") System::Key_Con_HWND(hwnd_cs2, 0x53, TRUE); // 'S'
		else if (Command_Str == "-back") System::Key_Con_HWND(hwnd_cs2, 0x53, FALSE);
	
		if (Command_Str == "+left") System::Key_Con_HWND(hwnd_cs2, 0x41, TRUE); // 'A'
		else if (Command_Str == "-left") System::Key_Con_HWND(hwnd_cs2, 0x41, FALSE);
	
		if (Command_Str == "+right") System::Key_Con_HWND(hwnd_cs2, 0x44, TRUE); // 'D'
		else if (Command_Str == "-right") System::Key_Con_HWND(hwnd_cs2, 0x44, FALSE);
	
	}


 

 


	namespace AutoAccept
	{
		static bool EnsureForegroundCS2()
		{
			HWND hwnd_cs2 = FindWindowA(NULL, "Counter-Strike 2");
			if (!hwnd_cs2)
				return false;
	
			// Attach thread input to allow us to set foreground window
			HWND hwndForeground = GetForegroundWindow();
			DWORD fgThread = hwndForeground ? GetWindowThreadProcessId(hwndForeground, nullptr) : 0;
			DWORD thisThread = GetCurrentThreadId();
			if (fgThread && fgThread != thisThread)
				AttachThreadInput(fgThread, thisThread, TRUE);
	
			ShowWindow(hwnd_cs2, SW_RESTORE);
			BringWindowToTop(hwnd_cs2);
			SetWindowPos(hwnd_cs2, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			SetWindowPos(hwnd_cs2, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			SetForegroundWindow(hwnd_cs2);
			SetActiveWindow(hwnd_cs2);
			SetFocus(hwnd_cs2);
	
			if (fgThread && fgThread != thisThread)
				AttachThreadInput(fgThread, thisThread, FALSE);
	
			// Small delay to let focus settle
			Sleep(80);
			return GetForegroundWindow() == hwnd_cs2;
		}
	
		static std::thread autoAcceptThread;
		static std::atomic<bool> autoAcceptRunning(false);
		static std::atomic<int> iterationCount(0);
		static const int kColorTolerance = 20; // tolerance for RGB matching
		static const int kDefaultIntervalMs = 1500; // baked-in interval
		static const int kDefaultMinPixels = 10000; // baked-in threshold
		
		void StartAutoAccept()
		{
			if (autoAcceptRunning.load()) return;
			
			autoAcceptRunning.store(true);
			autoAcceptThread = std::thread([]() {
				while (autoAcceptRunning.load())
				{
					if (MiscCFG::AutoAccept)
					{
						iterationCount++;
						
						if (DetectAcceptButton())
						{
							Log::Debug(std::string("[AutoAccept] [") + std::to_string(iterationCount.load()) + "] Found accept button! Clicking...");
						}
					}
					
					std::this_thread::sleep_for(std::chrono::milliseconds(kDefaultIntervalMs));
				}
			});
		}
		
		void StopAutoAccept()
		{
			if (!autoAcceptRunning.load()) return;
			
			autoAcceptRunning.store(false);
			
			if (autoAcceptThread.joinable())
			{
				autoAcceptThread.join();
			}
		}
		
		bool DetectAcceptButton()
		{
			// Use virtual screen to be robust on multi-monitor setups
			int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
			int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
			int screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
			int screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
			
			// Create device context for screen
			HDC hdcScreen = GetDC(NULL);
			HDC hdcMem = CreateCompatibleDC(hdcScreen);
			HBITMAP hbmScreen = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);
			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmScreen);
			
			// Copy virtual screen to bitmap including layered windows
			BitBlt(hdcMem, 0, 0, screenWidth, screenHeight, hdcScreen, vx, vy, SRCCOPY | CAPTUREBLT);
			
			// Define region of interest around CS2 window if available, otherwise center of virtual screen
			RECT wndRect{};
			HWND hwnd_cs2_roi = FindWindowA(NULL, "Counter-Strike 2");
			int roiLeft = 0, roiTop = 0, roiRight = screenWidth, roiBottom = screenHeight;
			if (hwnd_cs2_roi && GetWindowRect(hwnd_cs2_roi, &wndRect))
			{
				int wndW = wndRect.right - wndRect.left;
				int wndH = wndRect.bottom - wndRect.top;
				int roiW = static_cast<int>(wndW * 0.6f);
				int roiH = static_cast<int>(wndH * 0.6f);
				int wndCX = wndRect.left + wndW / 2;
				int wndCY = wndRect.top + wndH / 2;
				roiLeft = max(0, wndCX - roiW / 2 - vx);
				roiTop = max(0, wndCY - roiH / 2 - vy);
				roiRight = min(screenWidth, roiLeft + roiW);
				roiBottom = min(screenHeight, roiTop + roiH);
			}
			else
			{
				int roiWidth = static_cast<int>(screenWidth * 0.5f);
				int roiHeight = static_cast<int>(screenHeight * 0.5f);
				roiLeft = (screenWidth - roiWidth) / 2;
				roiTop = (screenHeight - roiHeight) / 2;
				roiRight = roiLeft + roiWidth;
				roiBottom = roiTop + roiHeight;
			}
	
			// Search for green pixels (CS2 accept button colors) within ROI
			long long sumX = 0, sumY = 0;
			int greenPixelCount = 0;
			int minX = INT_MAX, minY = INT_MAX, maxX = 0, maxY = 0;
			
			for (int x = roiLeft; x < roiRight; x += 1)
			{
				for (int y = roiTop; y < roiBottom; y += 1)
				{
					COLORREF pixelColor = GetPixel(hdcMem, x, y);
					if (IsGreenPixel(pixelColor))
					{
						greenPixelCount++;
						int absX = x + vx;
						int absY = y + vy;
						sumX += absX;
						sumY += absY;
						if (absX < minX) minX = absX;
						if (absY < minY) minY = absY;
						if (absX > maxX) maxX = absX;
						if (absY > maxY) maxY = absY;
					}
				}
			}
			
			// Clean up GDI objects
			SelectObject(hdcMem, hbmOld);
			DeleteObject(hbmScreen);
			DeleteDC(hdcMem);
			ReleaseDC(NULL, hdcScreen);
			
			// Prefer bounding-box center to avoid centroid bias from gradients
			int bboxW = (maxX >= minX) ? (maxX - minX + 1) : 0;
			int bboxH = (maxY >= minY) ? (maxY - minY + 1) : 0;
			if (greenPixelCount >= kDefaultMinPixels && bboxW > 80 && bboxH > 30)
			{
				int centerX = (minX + maxX) / 2;
				int centerY = (minY + maxY) / 2;
				ClickAcceptButton(centerX, centerY);
				return true;
			}
	
			// Fallback: use centroid if bbox looks too small or fragmented
			if (greenPixelCount >= kDefaultMinPixels)
			{
				int centroidX = static_cast<int>(sumX / greenPixelCount);
				int centroidY = static_cast<int>(sumY / greenPixelCount);
				ClickAcceptButton(centroidX, centroidY);
				return true;
			}
			
			return false;
		}
		
		void ClickAcceptButton(int x, int y)
		{
			// Bring CS2 window to foreground to ensure click is received
			HWND hwnd_cs2 = FindWindowA(NULL, "Counter-Strike 2");
			if (hwnd_cs2)
				EnsureForegroundCS2();
	
			// Helper lambda to perform one click (SendInput + client messages)
			auto performClick = [&](int cx, int cy)
			{
				INPUT inputs[2] = {};
				inputs[0].type = INPUT_MOUSE;
				inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
				inputs[1].type = INPUT_MOUSE;
				inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
				SendInput(2, inputs, sizeof(INPUT));
	
				if (hwnd_cs2)
				{
					POINT ptClientTopLeft{ 0, 0 };
					ClientToScreen(hwnd_cs2, &ptClientTopLeft);
					int clientX = cx - ptClientTopLeft.x;
					int clientY = cy - ptClientTopLeft.y;
					LPARAM lParam = MAKELPARAM(clientX, clientY);
					PostMessage(hwnd_cs2, WM_LBUTTONDOWN, MK_LBUTTON, lParam);
					PostMessage(hwnd_cs2, WM_LBUTTONUP, 0, lParam);
				}
			};
	
			// Retry strategy: 3 rapid attempts with slight jitter (faster timings)
			for (int attempt = 0; attempt < 3; ++attempt)
			{
				int jitterX = x + (attempt == 0 ? 0 : (attempt == 1 ? 2 : -2));
				int jitterY = y-10;
				SetCursorPos(jitterX, jitterY);
				Sleep(attempt == 0 ? 45 : 25);
				performClick(jitterX, jitterY);
				Sleep(35); // allow UI to respond
			}
		}
		
		bool IsGreenPixel(COLORREF color)
		{
			// Extract RGB components
			int r = GetRValue(color);
			int g = GetGValue(color);
			int b = GetBValue(color);
			
			// Target colors for the Accept button
			const int r1 = 54, g1 = 183, b1 = 82;
			const int r2 = 20, g2 = 61, b2 = 11;
			
			auto withinTol = [&](int cr, int cg, int cb, int tr, int tg, int tb) -> bool {
				return abs(cr - tr) <= kColorTolerance && abs(cg - tg) <= kColorTolerance && abs(cb - tb) <= kColorTolerance;
			};
			
			// Strong match with tolerance or general green dominance
			if (withinTol(r, g, b, r1, g1, b1) || withinTol(r, g, b, r2, g2, b2))
				return true;
			
			if (g > r + 40 && g > b + 40 && g > 90)
				return true;
			
			return false;
		}
		
		void UpdateAutoAccept()
		{
			// Run only if current map name indicates lobby (empty or "<empty>")
			const std::string mapName = Cheats::GetCurrentMapName();
			const bool lobby = (mapName.empty() || mapName == "<empty>");
			const bool shouldRun = MiscCFG::AutoAccept && lobby;
			if (shouldRun)
			{
				if (!autoAcceptRunning.load())
					StartAutoAccept();
			}
			else
			{
				if (autoAcceptRunning.load())
					StopAutoAccept();
			}
		}
	}
}
