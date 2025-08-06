//
// Created by Laeth English on 8/5/25.
//

#include "InputHandler.hpp"

#include <iostream>

#include "CameraHandler.hpp"

void UI::HandleKeyboardEvent(SDL_Event &event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        if (event.key.key == Keybindings[KEY_FORWARD]) {
            CameraPosition += GetForwardVector() * CameraMoveSpeed;
            std::cout << "Moved camera, new position: (" << CameraPosition.x << ", " << CameraPosition.y << ", " << CameraPosition.z << ")" << std::endl;
        }
        else if (event.key.key == Keybindings[KEY_BACK]) {
            CameraPosition -= GetForwardVector() * CameraMoveSpeed;
            std::cout << "Moved camera, new position: (" << CameraPosition.x << ", " << CameraPosition.y << ", " << CameraPosition.z << ")" << std::endl;
        }
        else if (event.key.key == Keybindings[KEY_LEFT]) {
            CameraPosition -= GetRightVector() * CameraMoveSpeed;
            std::cout << "Moved camera, new position: (" << CameraPosition.x << ", " << CameraPosition.y << ", " << CameraPosition.z << ")" << std::endl;
        }
        else if (event.key.key == Keybindings[KEY_RIGHT]) {
            CameraPosition += GetRightVector() * CameraMoveSpeed;
            std::cout << "Moved camera, new position: (" << CameraPosition.x << ", " << CameraPosition.y << ", " << CameraPosition.z << ")" << std::endl;
        }
        else if (event.key.key == Keybindings[KEY_UP]) {
            CameraPosition += GetUpVector() * CameraMoveSpeed;
            std::cout << "Moved camera, new position: (" << CameraPosition.x << ", " << CameraPosition.y << ", " << CameraPosition.z << ")" << std::endl;
        }
        else if (event.key.key == Keybindings[KEY_DOWN]) {
            CameraPosition -= GetUpVector() * CameraMoveSpeed;
            std::cout << "Moved camera, new position: (" << CameraPosition.x << ", " << CameraPosition.y << ", " << CameraPosition.z << ")" << std::endl;
        }
        else if (event.key.key == Keybindings[KEY_LOCK_VIEW]) {
            SetMouseLocked(!MouseLocked);
        }
    }
}
