#pragma once
#include "../Game/Game.h"
#include "../Game/Entity.h"
#include "../Features\Aimbot.h"
#include "../Features/Radar.h"
#include "../Features/TriggerBot.h"
#include "../Features/Misc.h"
#include "EntityResult.h"
#include <vector>

// processed entity data and results in Core/EntityResult.h

namespace Cheats
{
	static DWORD m_currentTick = 0;
	static DWORD m_previousTick = 0;
	static std::vector<std::pair<int, CEntity>> cachedResults;

	void Run();
	
	std::vector<std::pair<int, CEntity>> CollectEntityData(CEntity& localEntity, int& localPlayerControllerIndex);
	std::vector<EntityResult> ProcessEntities(CEntity& localEntity, int& localPlayerControllerIndex);
	void HandleEnts(const std::vector<EntityResult>& entities, CEntity& localEntity, 
		int localPlayerControllerIndex, Base_Radar& gameRadar, std::vector<Vec3>& aimPosList);

	std::string GetCurrentMapName();
}

struct {
	ImFont* normal15px = nullptr;
} fonts;
