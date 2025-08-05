//
// Created by Laeth English on 8/5/25.
//

#include "InputHandler.hpp"
#include "CameraHandler.hpp"

void UI::HandleKeyboardEvent(SDL_Event &event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        switch (event.key.key) {
            // camera translation
            case Keybindings[KEY_FORWARD]: {
                CameraPosition += GetForwardVector() * CameraMoveSpeed;
                break;
            }
            case Keybindings[KEY_BACK]: {
                CameraPosition -= GetForwardVector() * CameraMoveSpeed;
                break;
            }
            case Keybindings[KEY_LEFT]: {
                CameraPosition -= GetRightVector() * CameraMoveSpeed;
                break;
            }
            case Keybindings[KEY_RIGHT]: {
                CameraPosition += GetRightVector() * CameraMoveSpeed;
                break;
            }
            case Keybindings[KEY_UP]: {
                CameraPosition += GetUpVector() * CameraMoveSpeed;
                break;
            }
            case Keybindings[KEY_DOWN]: {
                CameraPosition -= GetUpVector() * CameraMoveSpeed;
                break;
            }

            // misc
            case Keybindings[KEY_LOCK_VIEW]: {
                SetMouseLocked(!MouseLocked);
                break;
            }
            default:
        }
    }
}
