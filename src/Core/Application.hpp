//
// Created by Laeth English on 7/28/25.
//

#ifndef APPLICATION_HPP
#define APPLICATION_HPP
#include "SDL3/SDL_video.h"


namespace Core {
    inline SDL_Window* Window = nullptr;

    /**
     * @brief Initializes the application with BGFX in a new SDL window.
     */
    int Init();
}

#endif //APPLICATION_HPP
