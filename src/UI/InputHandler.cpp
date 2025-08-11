//
// Created by Laeth English on 8/5/25.
//

#include "InputHandler.hpp"

#include <iostream>

#include "CameraHandler.hpp"
#include "../Simulation/Simulation.hpp"

void UI::HandleKeyboardEvent(const SDL_Event &event) {
    if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
        if (event.key.repeat != 0) return; // Ignore key repeat

        const auto it = Keybindings.find(event.key.key);
        if (it == Keybindings.end()) return;

        Keybind bind = it->second;
        const auto idx = static_cast<size_t>(bind);

        if (event.type == SDL_EVENT_KEY_DOWN) {
            if (!KeybindingsMask[idx]) {
                KeybindingsMaskThisFrame[idx] = true;
            }
            KeybindingsMask[idx] = true;
        } else if (event.type == SDL_EVENT_KEY_UP) {
            KeybindingsMask[idx] = false;
        }
    }
    else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
        MouseButton btn;
        switch (event.button.button) {
            case SDL_BUTTON_LEFT:
                btn = MouseButton::MOUSE_LEFT;
                break;
            case SDL_BUTTON_RIGHT:
                btn = MouseButton::MOUSE_RIGHT;
                break;
            case SDL_BUTTON_MIDDLE:
                btn = MouseButton::MOUSE_MIDDLE;
                break;
            default:
                return;
        }

        const auto idx = static_cast<size_t>(btn);
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            MouseButtonsMask[idx] = true;
        }
        else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
            MouseButtonsMask[idx] = false;
        }
    }
}

void UI::ProcessInputs() {
    if (KeyPressedThisFrame(Keybind::KEY_LOCK_VIEW)) {
        SetMouseLocked(!MouseLocked);
    }
    if (KeyPressedThisFrame(Keybind::KEY_PAUSE)) {
        Simulation::RunSimulation = !Simulation::RunSimulation;
    }
    KeybindingsMaskThisFrame.fill(false);
}

bool UI::MouseButtonPressed(MouseButton bind) {
    return MouseButtonsMask[static_cast<size_t>(bind)];
}


bool UI::KeyPressed(Keybind bind) {
    return KeybindingsMask[static_cast<size_t>(bind)];
}

bool UI::KeyPressedThisFrame(Keybind bind) {
    return KeybindingsMaskThisFrame[static_cast<size_t>(bind)];
}

