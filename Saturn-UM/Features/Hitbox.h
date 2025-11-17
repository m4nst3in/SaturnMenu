#pragma once
#include <vector>
#include "Aimbot.h"
#include "../Game/Entity.h"
#include "../OS-ImGui/imgui/imgui.h"

namespace Hitbox
{
    inline std::vector<int> AimbotList() { return { (int)BONEINDEX::head }; }
    inline std::vector<int> TriggerList() {
        return {
            (int)BONEINDEX::head,
            (int)BONEINDEX::neck_0,
            (int)BONEINDEX::spine_0,
            (int)BONEINDEX::spine_1,
            (int)BONEINDEX::spine_2,
            (int)BONEINDEX::spine_3,
            (int)BONEINDEX::pelvis,
            (int)BONEINDEX::arm_upper_L,
            (int)BONEINDEX::arm_lower_L,
            (int)BONEINDEX::hand_L,
            (int)BONEINDEX::arm_upper_R,
            (int)BONEINDEX::arm_lower_R,
            (int)BONEINDEX::hand_R,
            (int)BONEINDEX::leg_upper_L,
            (int)BONEINDEX::leg_lower_L,
            (int)BONEINDEX::ankle_L,
            (int)BONEINDEX::leg_upper_R,
            (int)BONEINDEX::leg_lower_R,
            (int)BONEINDEX::ankle_R
        };
    }
    inline bool AnyInFov(const CEntity& e, int radius, const std::vector<int>& hitboxes, ImVec2 center)
    {
        const auto& bones = e.GetBone().BonePosList;
        if (bones.empty()) return false;
        auto within = [&](const ImVec2& p, float r){ float dx = p.x - center.x; float dy = p.y - center.y; return (dx*dx + dy*dy) <= r*r; };
        for (int hb : hitboxes) {
            if (hb < 0 || (size_t)hb >= bones.size()) continue;
            ImVec2 p{ bones[hb].ScreenPos.x, bones[hb].ScreenPos.y };
            float r = (float)radius;
            if (hb == (int)BONEINDEX::ankle_L || hb == (int)BONEINDEX::ankle_R || hb == (int)BONEINDEX::hand_L || hb == (int)BONEINDEX::hand_R)
                r *= 1.6f; // extremidades pequenas
            if (within(p, r)) return true;
        }
        // fallback: dentro da caixa 2D do esqueleto com pequeno padding inferior (pé)
        ImVec2 minP{ bones[0].ScreenPos.x, bones[0].ScreenPos.y }, maxP = minP;
        for (const auto& bj : bones) {
            if (bj.ScreenPos.x < minP.x) minP.x = bj.ScreenPos.x;
            if (bj.ScreenPos.y < minP.y) minP.y = bj.ScreenPos.y;
            if (bj.ScreenPos.x > maxP.x) maxP.x = bj.ScreenPos.x;
            if (bj.ScreenPos.y > maxP.y) maxP.y = bj.ScreenPos.y;
        }
        const float pad = 8.0f; // cobrir sola do pé
        return center.x >= (minP.x - pad) && center.x <= (maxP.x + pad) && center.y >= (minP.y - pad) && center.y <= (maxP.y + pad + 10.0f);
    }
}