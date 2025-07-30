//
// Created by Laeth English on 7/28/25.
//

#include "Application.hpp"
#include <bgfx/bgfx.h>
#include <SDL3/SDL.h>
#include <iostream>
#include "ApplicationConfig.hpp"
#include <bgfx/platform.h>

int Core::Init() {
    // Init SDL3
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create SDL window
    SDL_Window* window = SDL_CreateWindow(
        APPLICATION_NAME,
        WIDTH,
        HEIGHT,
        (DEFAULT_FULLSCREEN ? SDL_WINDOW_FULLSCREEN : 0)
    );
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Prepare bgfx platform data
    bgfx::PlatformData pd{};
    SDL_PropertiesID winprops = SDL_GetWindowProperties(window);
    if (winprops == 0) {
        std::cerr << "Failed to get window properties! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    std::cout << "SDL Window initialized" << std::endl;

#if defined(_WIN32)
    pd.nwh = SDL_GetPointerProperty(winprops, SDL_PROP_WINDOW_WIN32_HWND, nullptr);
    pd.ndt = SDL_GetPointerProperty(winprops, SDL_PROP_WINDOW_WIN32_HINSTANCE, nullptr);
#elif defined(__linux__)
    const char* videoDriver = SDL_GetVideoDriver();
    if (videoDriver == nullptr) {
        std::cerr << "Unsupported video driver: " << videoDriver << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    if (strcmp(videoDriver, "x11")) {
        pd.ndt = SDL_GetPointerProperty(winprops, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
        pd.nwh = SDL_GetPointerProperty(winprops, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, nullptr);
    }
    else if (strcmp(videoDriver, "wayland")) {
        pd.ndt = SDL_GetPointerProperty(winprops, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr);
        pd.nwh = SDL_GetPointerProperty(winprops, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr);
    } else {
        std::cerr << "Unsupported video driver: " << videoDriver << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

#elif defined(__APPLE__)
    pd.nwh = SDL_Metal_GetLayer(SDL_Metal_CreateView(window));//SDL_GetPointerProperty(winprops, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
    pd.ndt = nullptr;
#endif

    bgfx::setPlatformData(pd);

    std::cout << "Platform data set" << std::endl;


    // init BGFX
    bgfx::Init init;
    init.type = bgfx::RendererType::Count; // auto-detect renderer
    init.platformData = pd;
    init.resolution.width = WIDTH;
    init.resolution.height = HEIGHT;
    init.resolution.reset = BGFX_RESET_VSYNC; // enable vsync

    std::cout << "BGFX Platform Data written" << std::endl;


    if (!bgfx::init(init)) {
        std::cerr << "Failed to initialize BGFX!" << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    std::cout << "BGFX initialized" << std::endl;

    // Init complete
    return 0;
}
