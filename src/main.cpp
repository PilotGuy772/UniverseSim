#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "Simulation/Simulation.hpp"
#include "Core/Application.hpp"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_metal.h"
#define BGFX_CONFIG_MULTITHREADED 0
#define BX_CONFIG_DEBUG 1

int main() {
    //std::cout << "Hello, World!" << std::endl;
    return Simulation::StartSimulation();

    //Core::Init();

    //  SDL_Init(SDL_INIT_VIDEO);
    //  SDL_Window* window = SDL_CreateWindow("test", 1280, 720, SDL_WINDOW_METAL);
    //
    //  bgfx::renderFrame();
    //
    //  bgfx::PlatformData pd;
    //  auto winprops = SDL_GetWindowProperties(window);
    //  pd.nwh = SDL_GetPointerProperty(winprops, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
    //  pd.ndt = nullptr;
    //  bgfx::setPlatformData(pd);
    //
    //
    //  bgfx::Init init;
    //  init.type = bgfx::RendererType::Metal;
    //  init.resolution.width = 1280;
    //  init.resolution.height = 720;
    //  init.resolution.reset = BGFX_RESET_VSYNC;
    //  init.platformData = pd;
    //  bgfx::init(init);
    //
    // bgfx::setDebug(BGFX_DEBUG_TEXT | BGFX_DEBUG_STATS);
    //
    //  bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
    //  bgfx::setViewRect(0, 0, 0, 1280, 720);
    // bgfx::touch(0);
    //
    //  unsigned int counter = 0;
    //  while(true) {
    //      bgfx::frame();
    //      counter++;
    //  }
}
