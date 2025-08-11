//
// Created by Laeth English on 8/10/25.
//

#include "GUI.hpp"

#include <dear-imgui/imgui.h>
#include <imgui/imgui.h>

#include "CameraHandler.hpp"
#include "InputHandler.hpp"
#include "../Core/ApplicationConfig.hpp"
#include "../Simulation/Simulation.hpp"
#include "SDL3/SDL_mouse.h"

void UI::ImGuiRender() {
    float mx, my;
    SDL_GetMouseState(&mx, &my);

    imguiBeginFrame(
        static_cast<int>(floor(mx)),
        static_cast<int>(floor(my)),
        (MouseButtonPressed(MouseButton::MOUSE_LEFT) ? IMGUI_MBUT_LEFT : 0) |
        (MouseButtonPressed(MouseButton::MOUSE_RIGHT) ? IMGUI_MBUT_RIGHT : 0) |
        (MouseButtonPressed(MouseButton::MOUSE_MIDDLE) ? IMGUI_MBUT_MIDDLE : 0),
        static_cast<int>(ceil(mx)),
        Core::DEFAULT_WIDTH,
        Core::DEFAULT_HEIGHT
        );
    //ImGui::NewFrame();

    //ImGuiBuildEntityWizard();
    //ImGuiBuildEntityList();
    ImGuiBuildDebugPanel();

    //ImGui::Render();
    imguiEndFrame();
}

void UI::ImGuiBuildDebugPanel() {
    ImGui::Begin("Debug");

    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
    ImGui::Text("Internal DeltaTime: %.5f s", Simulation::DeltaTime);

    static float camPos[3] = {CameraPosition.x, CameraPosition.y, CameraPosition.z};
    ImGui::DragFloat3("Camera position", camPos, 0.1f);
    ImGui::End();
}

