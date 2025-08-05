//
// Created by Laeth English on 8/5/25.
//

#ifndef INPUTHANDLER_HPP
#define INPUTHANDLER_HPP
#include <unordered_map>

#include "glm/gtc/constants.hpp"
#include "SDL3/SDL_events.h"

namespace UI {
    enum Keybind {
        KEY_FORWARD,
        KEY_BACK,
        KEY_LEFT,
        KEY_RIGHT,
        KEY_LOCK_VIEW,
        KEY_MENU,
        KEY_UP,
        KEY_DOWN,
        // more as needed
    };
    inline std::unordered_map<Keybind, SDL_Keycode> Keybindings = {
        {KEY_FORWARD, SDLK_W},
        {KEY_BACK, SDLK_S},
        {KEY_LEFT, SDLK_A},
        {KEY_RIGHT, SDLK_D},
        {KEY_LOCK_VIEW, SDLK_SPACE},
        {KEY_MENU, SDLK_ESCAPE},
        {KEY_UP, SDLK_LSHIFT},
        {KEY_DOWN, SDLK_LCTRL},
    };

    void HandleKeyboardEvent(SDL_Event& event);
}

#endif //INPUTHANDLER_HPP
