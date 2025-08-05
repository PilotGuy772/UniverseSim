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
#if defined(__linux__)
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "x11");
#endif

    // Init SDL3
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

#if defined(__linux__)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif

    // Create SDL window
    SDL_Window* window = SDL_CreateWindow(
        APPLICATION_NAME,
        WIDTH,
        HEIGHT,
#if defined(__linux__)
        SDL_WINDOW_OPENGL |
#endif
        (DEFAULT_FULLSCREEN ? SDL_WINDOW_FULLSCREEN : 0)
    );
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_ShowWindow(window);
    SDL_Delay(100);

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
    const char* videoDriver = SDL_GetCurrentVideoDriver();
    std::cout << "Video driver: " << videoDriver << std::endl;

    if (strcmp(videoDriver, "x11") == 0) {
        pd.ndt = SDL_GetPointerProperty(winprops, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
        //pd.nwh = SDL_GetPointerProperty(winprops, SDL_PROP_WINDOW_X11_SCREEN_NUMBER, nullptr);
        std::cout << "X11 - Display: " << pd.ndt << ", Window: " << pd.nwh << std::endl;
    }
    else if (strcmp(videoDriver, "wayland") == 0) {
        pd.ndt = SDL_GetPointerProperty(winprops, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr);
        pd.nwh = SDL_GetPointerProperty(winprops, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr);
        std::cout << "Wayland - Display: " << pd.ndt << ", Surface: " << pd.nwh << std::endl;
    }

    if (pd.ndt == nullptr/* || pd.nwh == nullptr*/) {
        std::cerr << "ERROR: Failed to get native handles!" << std::endl;
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
#if defined(__linux__)
    init.type = bgfx::RendererType::OpenGL;
#elif defined(__APPLE__)
    init.type = bgfx::RendererType::Metal;
#endif
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

    const bgfx::Caps* caps = bgfx::getCaps();
    if (caps->supported & BGFX_CAPS_COMPUTE) {
        std::cout << "Compute shaders supported" << std::endl;
    } else {
        std::cout << "Compute shaders NOT supported" << std::endl;
    }

    // Also check OpenGL version being used
    std::cout << "Renderer: " << bgfx::getRendererName(bgfx::getRendererType()) << std::endl;

    // Init complete
    return 0;
}
