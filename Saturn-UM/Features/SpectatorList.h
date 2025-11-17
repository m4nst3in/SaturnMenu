#pragma once
#include "..\Game\Entity.h"
#include "..\Core\Init.h"
#include <unordered_set>

namespace SpecList
{
    struct SpecData
    {
        std::unordered_set<std::string> current_spectators;
        uintptr_t spectated_pawn;
        bool needs_refresh;
        uintptr_t prev_target_pawn = 0;
    };

    static SpecData g_spec_data;

    uintptr_t GetObserverTarget(uintptr_t entityPawnAddress)
    {
        if (!entityPawnAddress)
            return 0;

        uintptr_t observerServices = 0;
        if (!memoryManager.ReadMemory<uintptr_t>(entityPawnAddress + Offset.PlayerController.m_pObserverServices, observerServices) || !observerServices)
            return 0;

        uint32_t observedTargetHandle = 0;
        if (!memoryManager.ReadMemory<uint32_t>(observerServices + Offset.PlayerController.m_hObserverTarget, observedTargetHandle) || !observedTargetHandle)
            return 0;

        return CEntity::ResolveEntityHandle(observedTargetHandle);
    }

    void GetSpectatorList(const std::vector<CEntity>& allEntities, CEntity& LocalEntity)
    {
        auto prev_spectators = g_spec_data.current_spectators;

        g_spec_data.current_spectators.clear();
        g_spec_data.spectated_pawn = 0;
        g_spec_data.prev_target_pawn = 0;

        if (!LocalEntity.IsAlive() && LocalEntity.Pawn.Address != 0)
        {
            g_spec_data.prev_target_pawn = GetObserverTarget(LocalEntity.Pawn.Address);
            g_spec_data.spectated_pawn = g_spec_data.prev_target_pawn;
        }

        for (const auto& entity : allEntities)
        {
            if (!entity.Controller.Address || entity.Controller.PlayerName.empty())
                continue;

            if (entity.Controller.Address == LocalEntity.Controller.Address)
                continue;

            if (LocalEntity.IsAlive() && entity.IsAlive())
                continue;

            uintptr_t pawn_handle = 0;
            if (!memoryManager.ReadMemory<uintptr_t>(entity.Controller.Address + Offset.PlayerController.m_hPawn, pawn_handle))
                continue;

            uintptr_t pawn_addr = 0;
            if (pawn_handle != 0)
                pawn_addr = CEntity::ResolveEntityHandle(pawn_handle);

            if (!pawn_addr)
                pawn_addr = entity.Pawn.Address;

            if (!pawn_addr)
                continue;

            uintptr_t spec_target = GetObserverTarget(pawn_addr);
            if (!spec_target)
                continue;

            if (spec_target == LocalEntity.Pawn.Address || (g_spec_data.spectated_pawn != 0 && spec_target == g_spec_data.spectated_pawn))
                g_spec_data.current_spectators.insert(entity.Controller.PlayerName);
        }

        g_spec_data.needs_refresh = (g_spec_data.current_spectators != prev_spectators);
    }

    void SpectatorWindowList(CEntity& LocalEntity)
    {
        if (!MiscCFG::SpecList || (LocalEntity.Pawn.TeamID == 0 && !MenuConfig::ShowMenu))
            return;
        if (!Init::ShouldRenderESP()) return;

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus;

        float lineHeight = ImGui::GetTextLineHeightWithSpacing();
        float requiredHeight = g_spec_data.current_spectators.size() * lineHeight + 20.0f;

        const auto& io = ImGui::GetIO();
        const float margin = 16.0f;
        float winW = 180.0f;
        ImVec2 nextPos = ImVec2(margin, (io.DisplaySize.y - requiredHeight) * 0.5f);
        ImGui::SetNextWindowPos(nextPos, ImGuiCond_Once);
        ImGui::SetNextWindowSize({ winW, requiredHeight }, ImGuiCond_Once);
        ImGui::GetStyle().WindowRounding = 8.0f;

        std::string title = "Spectators";
        // alpha transition based on list emptiness
        static float alpha = 0.0f;
        bool dragging = ImGui::IsMouseDragging(0, 0.0f) && ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
        float target = dragging ? 1.0f : (g_spec_data.current_spectators.empty() ? 0.0f : 1.0f);
        float dt = io.DeltaTime;
        float k = ImClamp(dt * 12.0f, 0.0f, 1.0f);
        alpha = alpha + (target - alpha) * k;
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        ImGui::Begin(title.c_str(), NULL, flags);

        

        if (!g_spec_data.current_spectators.empty())
        {
            for (const auto& spectator : g_spec_data.current_spectators)
            {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
                ImVec4 accent = ImVec4(0.486f, 0.227f, 0.929f, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1,1,1,1));
                ImGui::Text(spectator.c_str());
                ImGui::PopStyleColor();
            }
        }

        MenuConfig::SpecWinPos = ImGui::GetWindowPos();
        ImGui::End();
        ImGui::PopStyleVar();
    }
}