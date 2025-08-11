//
// Created by Laeth English on 8/5/25.
//

#ifndef INPUTHANDLER_HPP
#define INPUTHANDLER_HPP
#include <unordered_map>
#include <vector>

#include "glm/gtc/constants.hpp"
#include "SDL3/SDL_events.h"

namespace UI {
    enum class Keybind : uint8_t {
        KEY_FORWARD,
        KEY_BACK,
        KEY_LEFT,
        KEY_RIGHT,
        KEY_LOCK_VIEW,
        KEY_MENU,
        KEY_UP,
        KEY_DOWN,
        KEY_PAUSE,
        COUNT,
        // more as needed
    };
    inline std::unordered_map<SDL_Keycode, Keybind> Keybindings = {
    {SDLK_W, Keybind::KEY_FORWARD},
    {SDLK_S, Keybind::KEY_BACK},
    {SDLK_A, Keybind::KEY_LEFT},
    {SDLK_D, Keybind::KEY_RIGHT},
    {SDLK_SPACE, Keybind::KEY_LOCK_VIEW},
    {SDLK_ESCAPE, Keybind::KEY_MENU},
    {SDLK_LSHIFT, Keybind::KEY_UP},
    {SDLK_LCTRL, Keybind::KEY_DOWN},
    {SDLK_P, Keybind::KEY_PAUSE},
    };
    inline std::array<bool, static_cast<size_t>(Keybind::COUNT)> KeybindingsMask = {};
    inline std::array<bool, static_cast<size_t>(Keybind::COUNT)> KeybindingsMaskThisFrame = {};

    void HandleKeyboardEvent(const SDL_Event &event);
    bool KeyPressed(Keybind bind);
    bool KeyPressedThisFrame(Keybind bind);
    void ProcessInputs();
}

#endif //INPUTHANDLER_HPP
