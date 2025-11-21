#include "Aimbot.h"
#include "../Game/Entity.h"
#include <algorithm>
#undef max
#undef min
#include "../Core/Cheats.h"

// Helper para posição de bone pelo seu modelo CBone/BonePosList
inline Vec3 GetBonePos(const CBone& boneData, int boneId) {
    if (boneId < 0 || boneId >= static_cast<int>(boneData.BonePosList.size()))
        return Vec3{0,0,0};
    return boneData.BonePosList[boneId].Pos;
}

void AimControl::switchToggle() {
    AimControl::ToggledActive = !AimControl::ToggledActive;
}

std::pair<float, float> AimControl::CalculateTargetOffset(const Vec2& ScreenPos, int ScreenCenterX, int ScreenCenterY) {
    float TargetX = ScreenPos.x - ScreenCenterX;
    float TargetY = ScreenPos.y - ScreenCenterY;
    return { TargetX, TargetY };
}

std::pair<float, float> AimControl::Humanize(float TargetX, float TargetY) {
    static float HumanizationAmount = HumanizationStrength * 2 / 100;
    if (HumanizationAmount <= 0.0f) {
        PrevTargetX = TargetX;
        PrevTargetY = TargetY;
        return { TargetX, TargetY };
    }
    std::uniform_real_distribution<float> jitterDist(-10.f, 10.f);
    std::uniform_real_distribution<float> microDist(-10.f, 10.f);
    std::uniform_real_distribution<float> smoothnessDist(0.4f, 10.f);

    float MovementDistance = std::sqrt(TargetX * TargetX + TargetY * TargetY);

    float MicroJitterX = microDist(gen) * std::min(MovementDistance * 0.25f, 8.0f) * HumanizationAmount;
    float MicroJitterY = microDist(gen) * std::min(MovementDistance * 0.25f, 8.0f) * HumanizationAmount;

    float JitterScale = std::min(MovementDistance * 0.15f, 12.0f) * HumanizationAmount;
    float JitterX = jitterDist(gen) * JitterScale;
    float JitterY = jitterDist(gen) * JitterScale;

    float PerpX = -TargetY * 0.35f * jitterDist(gen) * HumanizationAmount;
    float PerpY = TargetX * 0.35f * jitterDist(gen) * HumanizationAmount;

    float baseSmoothFactor = smoothnessDist(gen);
    float SmoothFactor = 1.0f - ((1.0f - baseSmoothFactor) * HumanizationAmount);
    float SmoothedX = TargetX * SmoothFactor + PrevTargetX * (1.0f - SmoothFactor);
    float SmoothedY = TargetY * SmoothFactor + PrevTargetY * (1.0f - SmoothFactor);

    std::uniform_real_distribution<float> reactionDist(0.0f, 1.0f);
    if (reactionDist(gen) < 0.15f * HumanizationAmount) {
        SmoothedX = PrevTargetX;
        SmoothedY = PrevTargetY;
    }

    float HumanizedX = SmoothedX + MicroJitterX + JitterX + PerpX;
    float HumanizedY = SmoothedY + MicroJitterY + JitterY + PerpY;

    PrevTargetX = TargetX;
    PrevTargetY = TargetY;
    return { HumanizedX, HumanizedY };
}

// Aimbot ultra preciso, sempre gruda na cabeça visível e o mais perto do centro da tela!
// Otimizado para usermode external tipo Noturnal.
void AimControl::AimBot(const CEntity& Local, std::vector<std::pair<int, CEntity>>& EntityList) {
    if (Local.Pawn.ShotsFired <= AimBullet - 1 && AimBullet != 0) {
        HasTarget = false;
        LockedTargetId = 0;
        return;
    }

    bool isScoped = false;
    memoryManager.ReadMemory<bool>(Local.Pawn.Address + Offset.Pawn.isScoped, isScoped);
    if (AimControl::ScopeOnly && !isScoped) {
        HasTarget = false;
        LockedTargetId = 0;
        return;
    }

    int localTeam = Local.Pawn.TeamID;
    int ScreenCenterX = Gui.Window.Size.x / 2;
    int ScreenCenterY = Gui.Window.Size.y / 2;

    float BestScreenDistance = FLT_MAX;
    const CEntity* targetEnt = nullptr;
    Vec2 bestScreenPos{0,0};
    Vec3 bestWorldPos{0,0,0};

    // Seleção do alvo: só considera inimigos vivos e visíveis!
    for (auto& [idx, ent] : EntityList) {
        if (ent.Pawn.Health <= 0) continue;
        if (!Cheats::IsFFA() && ent.Pawn.TeamID == localTeam) continue;

        int headIdx = AimControl::HitboxList.empty() ? BONEINDEX::head : AimControl::HitboxList[0];
        Vec3 head = GetBonePos(ent.Pawn.BoneData, headIdx);
        Vec2 screenHead;
        if (!gGame.View.WorldToScreen(head, screenHead)) continue;

        bool visible = true;

        float screenDistance = std::hypot(screenHead.x - ScreenCenterX, screenHead.y - ScreenCenterY);

        constexpr float DEG_TO_RAD = 3.14159265358979323846f / 180.f;
        float halfWindowSize = Gui.Window.Size.x / 2.f;
        float staticFovTan = tan(90.0f * DEG_TO_RAD / 2.f);
        float aimFovTan = tan(AimControl::AimFov * DEG_TO_RAD / 2.f);
        float aimFovRadius = (aimFovTan / staticFovTan) * halfWindowSize;
        if (screenDistance < BestScreenDistance && screenDistance <= aimFovRadius) {
            BestScreenDistance = screenDistance;
            targetEnt = &ent;
            bestScreenPos = screenHead;
            bestWorldPos = head;
        }
    }

    if (!targetEnt) {
        HasTarget = false;
        LockedTargetId = 0;
        return;
    }
    LockedTargetId = targetEnt->Pawn.Address; // trava no inimigo

    HasTarget = true;

    // Lógica de mira (snap legit/instant)
    auto [TargetX, TargetY] = CalculateTargetOffset(bestScreenPos, ScreenCenterX, ScreenCenterY);
    TargetX /= Local.Client.Sensitivity;
    TargetY /= Local.Client.Sensitivity;

    // Snap instantâneo se Smooth == 0, senão smooth real
    if (Smooth > 0.0f) {
        TargetX /= Smooth;
        TargetY /= Smooth;
    }

    if (HumanizeVar && Smooth > 0.0f) {
        auto [hX, hY] = Humanize(TargetX, TargetY);
        TargetX = hX; TargetY = hY;
    }

    static DWORD lastAimTime = 0;
    static float accumX = 0.f, accumY = 0.f;
    DWORD currentTick = GetTickCount64();

    // Menor delay possível para legítimo ultra rápido
    if (currentTick - lastAimTime >= MenuConfig::AimDelay) {
        int outX = static_cast<int>(std::round(TargetX));
        int outY = static_cast<int>(std::round(TargetY));
        if (Smooth > 0.0f) {
            accumX += TargetX; accumY += TargetY;
            outX = static_cast<int>(std::round(accumX));
            outY = static_cast<int>(std::round(accumY));
            accumX -= outX;
            accumY -= outY;
        }
        if (outX || outY) {
            mouse_event(MOUSEEVENTF_MOVE, outX, outY, 0, 0);
        }
        lastAimTime = currentTick;
    }
}

bool AimControl::CheckAutoMode(const std::string& WeaponName) {
    if (WeaponName == "deagle" || WeaponName == "elite" || WeaponName == "fiveseven" || WeaponName == "glock"
        || WeaponName == "awp" || WeaponName == "xm1014" || WeaponName == "mag7" || WeaponName == "sawedoff"
        || WeaponName == "tec9" || WeaponName == "zeus" || WeaponName == "p2000" || WeaponName == "nova"
        || WeaponName == "p250" || WeaponName == "ssg08" || WeaponName == "usp" || WeaponName == "revolver")
        return false;
    else
        return true;
}
