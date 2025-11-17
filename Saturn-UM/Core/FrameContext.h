#pragma once
#include <vector>
#include "EntityResult.h"
#include "../Game/Entity.h"
#include "../Features/Radar.h"
namespace Core {
struct FrameContext {
    const CEntity* local;
    int localControllerIndex;
    const std::vector<EntityResult>* entities;
    std::vector<Vec3>* aimPosList;
    Base_Radar* radar;
};
}

