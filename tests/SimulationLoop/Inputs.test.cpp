//
// Created by Laeth English on 8/6/25.
//

// test the various components of the main sim loop
#include <thread>
#include <catch2/catch_all.hpp>
#include <Core/Application.hpp>
#include <glm/vec3.hpp>
#include <GPU/ComputeBridge.hpp>
#include <SDL3/SDL_events.h>
#include <UI/CameraHandler.hpp>
#include <UI/InputHandler.hpp>

#include "GPU/BufferManager.hpp"

TEST_CASE("Handles SDL input events properly.", "[SimLoop]") {
    // send mocked inputs to the various input handlers
    SECTION("Initialization") {
        REQUIRE(Core::Init() == 0);
        GPU::Initialize();
    }

    SECTION("Keyboard events for camera movement") {
        SDL_Event event;
        event.type = SDL_EVENT_KEY_DOWN;
        event.key.key = SDLK_W;

        glm::vec3 old = UI::CameraPosition;
        UI::HandleKeyboardEvent(event);
        REQUIRE(UI::CameraPosition.x == old.x);
        REQUIRE(UI::CameraPosition.y == old.y);
        REQUIRE(UI::CameraPosition.z == old.z - UI::CameraMoveSpeed);
    }

    SECTION("Mouse events for camera rotation") {
        SDL_Event event;
        event.type = SDL_EVENT_MOUSE_MOTION;
        event.motion.xrel = 10.0f;
        event.motion.yrel = -10.0f;

        glm::quat old = UI::CameraOrientation;
        UI::SetMouseLocked(true);
        UI::HandleMouseEvent(event);
        glm::quat cur = UI::CameraOrientation;
        // Result should be: 0.0868241, -0.0868241, 0.992404, -0.00759612
        REQUIRE(cur.x == Catch::Approx(0.0868241).epsilon(0.01f));
        REQUIRE(cur.y == Catch::Approx(-0.0868241).epsilon(0.01f));
        REQUIRE(cur.z == Catch::Approx(0.992404).epsilon(0.01f));
        REQUIRE(cur.w == Catch::Approx(-0.00759612).epsilon(0.01f));
    }
}

