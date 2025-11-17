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

        ImVec2 minP{ bones[0].ScreenPos.x, bones[0].ScreenPos.y }, maxP = minP;
        for (const auto& bj : bones) {
            if (bj.ScreenPos.x < minP.x) minP.x = bj.ScreenPos.x;
            if (bj.ScreenPos.y < minP.y) minP.y = bj.ScreenPos.y;
            if (bj.ScreenPos.x > maxP.x) maxP.x = bj.ScreenPos.x;
            if (bj.ScreenPos.y > maxP.y) maxP.y = bj.ScreenPos.y;
        }
        float height = maxP.y - minP.y;
        float baseR = (float)radius;
        float scaledR = baseR;
        if (height > 2.0f) {
            float cap = height * 0.20f + 2.0f; // raio proporcional ao tamanho na tela
            if (scaledR > cap) scaledR = cap;
        }

        for (int hb : hitboxes) {
            if (hb < 0 || (size_t)hb >= bones.size()) continue;
            ImVec2 p{ bones[hb].ScreenPos.x, bones[hb].ScreenPos.y };
            float r = scaledR;
            bool isExtremity = (hb == (int)BONEINDEX::ankle_L || hb == (int)BONEINDEX::ankle_R || hb == (int)BONEINDEX::hand_L || hb == (int)BONEINDEX::hand_R);
            if (isExtremity && height >= 140.0f) r *= 1.4f; // só ampliar em targets próximos
            if (within(p, r)) return true;
        }
        return false;
    }

    inline Vec3 ForwardFromAngles(const Vec2& ang)
    {
        const float deg = 3.14159265358979323846f / 180.0f;
        float pitch = ang.x * deg;
        float yaw = ang.y * deg;
        float cp = cosf(pitch), sp = sinf(pitch);
        float cy = cosf(yaw), sy = sinf(yaw);
        return Vec3{ cy * cp, sy * cp, -sp };
    }

    inline float BoneRadius(int b)
    {
        switch (b) {
        case (int)BONEINDEX::head: return 8.0f;
        case (int)BONEINDEX::neck_0: return 6.5f;
        case (int)BONEINDEX::spine_0: return 7.0f;
        case (int)BONEINDEX::spine_1: return 7.5f;
        case (int)BONEINDEX::spine_2: return 8.5f;
        case (int)BONEINDEX::spine_3: return 8.5f;
        case (int)BONEINDEX::pelvis: return 7.5f;
        case (int)BONEINDEX::arm_upper_L: return 5.5f;
        case (int)BONEINDEX::arm_lower_L: return 4.5f;
        case (int)BONEINDEX::hand_L: return 4.0f;
        case (int)BONEINDEX::arm_upper_R: return 5.5f;
        case (int)BONEINDEX::arm_lower_R: return 4.5f;
        case (int)BONEINDEX::hand_R: return 4.0f;
        case (int)BONEINDEX::leg_upper_L: return 6.5f;
        case (int)BONEINDEX::leg_lower_L: return 5.5f;
        case (int)BONEINDEX::ankle_L: return 4.0f;
        case (int)BONEINDEX::leg_upper_R: return 6.5f;
        case (int)BONEINDEX::leg_lower_R: return 5.5f;
        case (int)BONEINDEX::ankle_R: return 4.0f;
        default: return 10.0f;
        }
    }

    inline bool RayHits(const CEntity& local, const CEntity& target, const std::vector<int>& hitboxes)
    {
        const auto& bones = target.GetBone().BonePosList;
        if (bones.empty()) return false;
        Vec3 origin = local.Pawn.CameraPos;
        Vec3 dir = ForwardFromAngles(local.Pawn.ViewAngle);
        float len = dir.Length();
        if (len <= 0.0f) return false;
        dir = dir / len;
        for (int hb : hitboxes) {
            if (hb < 0 || (size_t)hb >= bones.size()) continue;
            const Vec3& p = bones[hb].Pos;
            Vec3 v = p - origin;
            float t = v.DotProduct(dir);
            if (t <= 0.0f) continue;
            Vec3 closest = origin + dir * t;
            float d = (closest - p).Length();
            float dist = v.Length();
            float scale;
            if (dist > 0.0f) {
                float s = 600.0f / dist;
                if (s < 0.25f) s = 0.25f;
                else if (s > 1.0f) s = 1.0f;
                scale = s;
            } else {
                scale = 1.0f;
            }
            float r = BoneRadius(hb) * scale;
            bool extremity = (hb == (int)BONEINDEX::hand_L || hb == (int)BONEINDEX::hand_R || hb == (int)BONEINDEX::ankle_L || hb == (int)BONEINDEX::ankle_R);
            if (extremity) {
                float minR = (dist < 800.0f) ? 1.6f : 1.0f;
                if (r < minR) r = minR;
            }
            if (d <= r) return true;
        }
        return false;
    }
}