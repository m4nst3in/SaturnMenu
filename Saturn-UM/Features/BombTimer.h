#pragma once
#include <chrono>
#include <iostream>
#include <utility>
#include <sstream>
#include <ctime>
#include <string>
#include "..\Game\Entity.h"
#include "..\Core\Config.h"
#include "..\Core\Init.h"

namespace bmb
{
	bool isPlanted = false;
	std::time_t plantTime;

	uint64_t currentTimeMillis() {
		using namespace std::chrono;
		return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	}

	int getBombSite(bool Planted)
	{
		if (Planted)
		{
			int site;
			uintptr_t cPlantedC4;
			if (!memoryManager.ReadMemory<uintptr_t>(gGame.GetClientDLLAddress() + Offset.PlantedC4, cPlantedC4))
				return 0;
			if (!memoryManager.ReadMemory<uintptr_t>(cPlantedC4, cPlantedC4))
				return 0;
			if (!memoryManager.ReadMemory<int>(cPlantedC4 + Offset.C4.m_nBombSite, site))
				return 0;

			return site;
		}
		else
			return 0;
		
	}

    void RenderWindow(int inGame)
    {
        if ((!MiscCFG::bmbTimer) || (inGame == 0 && !MenuConfig::ShowMenu))
            return;
        if (!Init::isGameWindowActive() && !MenuConfig::ShowMenu) return;

		uintptr_t bomb;
		bool isBombPlanted;
		bool IsBeingDefused;
		float DefuseTime;
		float defuseRemaining;
        
		auto plantedAddress = gGame.GetClientDLLAddress() + Offset.PlantedC4;

		memoryManager.ReadMemory(plantedAddress, bomb);
		memoryManager.ReadMemory(bomb, bomb);
		memoryManager.ReadMemory(plantedAddress - 0x8, isBombPlanted);

		auto time = currentTimeMillis();

		if (isBombPlanted && !isPlanted && (plantTime == NULL || time - plantTime > 60000))
		{
			isPlanted = true;
			plantTime = time;
		}

		memoryManager.ReadMemory(bomb + Offset.C4.m_bBeingDefused, IsBeingDefused);
		memoryManager.ReadMemory(bomb + Offset.C4.m_flDefuseCountDown, DefuseTime);

		if (!isPlanted && !MenuConfig::ShowMenu)
			return;

        static float windowWidth = 220.0f;
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus;
        const auto& io = ImGui::GetIO();
        const float margin = 16.0f;
        ImVec2 nextPos = ImVec2(io.DisplaySize.x - windowWidth - margin, io.DisplaySize.y - 120.0f - margin);
        ImGui::SetNextWindowPos(nextPos, ImGuiCond_Once);
        ImGui::SetNextWindowSize({ windowWidth, 0 }, ImGuiCond_Once);

        // alpha transition
        static float alpha = 0.0f;
        float target = (isPlanted || MenuConfig::ShowMenu) ? 1.0f : 0.0f;
        float dt = io.DeltaTime;
        float k = ImClamp(dt * 12.0f, 0.0f, 1.0f);
        alpha = alpha + (target - alpha) * k;
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        ImGui::Begin("Bomb Timer", nullptr, flags);

        

        float remaining = (40000 - (int64_t)time + plantTime) / (float)1000;

        float startPosX = ((ImGui::GetWindowSize().x - 180) * 0.5f) + 3;
        ImGui::SetCursorPosX(startPosX);
		
        if (isPlanted && remaining >= 0)
        {
            std::ostringstream ss;
            ss.precision(3);
            ss << "Bomba no " << (!getBombSite(isBombPlanted) ? "A" : "B") << ": " << std::fixed << std::round(remaining * 1000.0) / 1000.0 << " s";
            if (IsBeingDefused) ss << " [DESARMANDO]";
            Gui.MyText(std::move(ss).str().c_str(), true);
        }
        else
        {
            Gui.MyText("C4 não plantada", true);
        }

		if (isPlanted && remaining >= 0 && IsBeingDefused)
		{
			float defuseRemaining = 0.0f;
			DWORD64 globalVars = 0;
			if (memoryManager.ReadMemory<DWORD64>(gGame.GetClientDLLAddress() + Offset.GlobalVars, globalVars) && globalVars)
			{
				globalvars gv{ globalVars };
				if (gv.GetcurrentTime() && gv.g_fCurrentTime > 0.0f)
				{
					float defuseEndTime = 0.0f;
					if (memoryManager.ReadMemory<float>(bomb + Offset.C4.m_flDefuseCountDown, defuseEndTime) && defuseEndTime > 0.0f)
					{
						defuseRemaining = defuseEndTime - gv.g_fCurrentTime;
						if (defuseRemaining < 0.0f || defuseRemaining > 10.0f) defuseRemaining = 0.0f;
					}
				}
			}

            ImGui::SetCursorPosX(startPosX);
            ImGui::TextColored(ImColor(131, 137, 150, 200), "Desarmando: %.3f s", defuseRemaining);
		}
		if (isPlanted && !isBombPlanted)
		{
			isPlanted = false;
		}
        MenuConfig::BombWinPos = ImGui::GetWindowPos();
        ImGui::End();
        ImGui::PopStyleVar();
    }
}