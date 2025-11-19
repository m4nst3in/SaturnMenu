#pragma once
#include "../Game/Entity.h"
#include "../OS-ImGui/imgui/imgui.h"

struct EntityResult {
    int entityIndex = -1;
    CEntity entity;
    ImVec4 espRect;
    int distance = 0;
    bool isInScreen = false;
    bool isValid = false;
};
