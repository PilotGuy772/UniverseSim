//
// Created by Laeth English on 8/5/25.
//

#include "Simulation.hpp"

#include "../Core/Application.hpp"
#include "../Core/ApplicationConfig.hpp"
#include "../GPU/BufferManager.hpp"
#include "../GPU/ComputeBridge.hpp"
#include "../UI/CameraHandler.hpp"
#include "../UI/InputHandler.hpp"
#include "../UI/Render.hpp"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_timer.h"

int Simulation::StartSimulation() {
    int status = Core::Init();
    if (status != 0) return status;
    GPU::Initialize();
    GPU::InitBuffers(Core::STARTING_BUFFER_SIZE);

}

void Simulation::RunMainThread() {
    // Simulation Loop:
    // 1. reset stopwatch for deltaTime
    // 2. check keyboard/mouse input with SDL
    // 3. run verlet position
    // 4. run gravity
    // 5. run verlet velocity
    // 6. check for adding new entities
    // 7. check for removing entities
    // 8. render scene
    // 9. call bgfx::frame()
    // 10. swap buffers
    // 11. repeat!

    bool running = true;

    while (running) {
        // reset stopwatch for deltaTime
        float deltaTime = Stopwatch.elapsed<stopwatch::seconds>();
        Stopwatch.start();

        // check SDL events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                running = false;

            UI::HandleKeyboardEvent(event);
            UI::HandleMouseEvent(event);
        }

        // dispatch position
        GPU::DispatchVerletPosition(deltaTime);

        // dispatch gravity
        GPU::DispatchGravity();

        // dispatch velocity
        GPU::DispatchVerletVelocity(deltaTime);

        // check for adding new entities
        bool resizeBuffersAfterFrame = false;
        if (EntityQueue.size() > 0) {
            int result = AddNextEntityFromQueue();
            if (result < 0) {
                resizeBuffersAfterFrame = true;
            }
        }

        // check for removing entities
        if (EntityDeathQueue.size() > 0) {
            KillNextEntityFromQueue();
        }

        // render scene
        UI::RenderScene();

        // frame!
        bgfx::frame();

        // swap buffers
        GPU::SwapBuffers();

        if (resizeBuffersAfterFrame) GPU::ResizeBuffers();
    }
}

