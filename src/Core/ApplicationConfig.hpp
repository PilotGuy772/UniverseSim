//
// Created by Laeth English on 7/28/25.
//

#ifndef APPLICATIONCONFIG_HPP
#define APPLICATIONCONFIG_HPP

// Globals for application configuration
namespace Core {
    // Defines for shader type on Linux
#if defined(__linux__)
//#define SHADER_TYPE_GLSL 0
//#define SHADER_TYPE_SPIRV 0
#define SHADER_TYPE_430 0
#endif

    // Application metadata
    const auto APPLICATION_NAME = "Universe Simulator";
    const auto APPLICATION_SLUG = "universe-sim";
    const auto APPLICATION_VERSION = "0.0.1";

    // default graphics settings
    constexpr auto DEFAULT_WIDTH = 1280;
    constexpr auto DEFAULT_HEIGHT = 720;
    inline int WIDTH = DEFAULT_WIDTH;
    inline int HEIGHT = DEFAULT_HEIGHT;
    constexpr auto DEFAULT_FULLSCREEN = false;

    // View IDs
    constexpr uint8_t VIEW_ID_MAIN = 0; // Main view for rendering
    constexpr uint8_t VIEW_ID_RESERVED = 1;
    constexpr uint8_t VIEW_ID_COMPUTE_POSITION = 2; // View for computing gravitation
    constexpr uint8_t VIEW_ID_COMPUTE_GRAVITY = 3; // View for integrating physics
    constexpr uint8_t VIEW_ID_COMPUTE_VELOCITY = 4; // View for computing velocities
    constexpr uint8_t VIEW_ID_ENTITY_ADDER = 5; // Entity adder view, also used for buffer copying
    constexpr uint8_t VIEW_ID_COMPUTE_READBACK_SINGLE = 6; // View for reading back single entity data

    // Default sim settings
    constexpr uint32_t STARTING_BUFFER_SIZE = 64;
}

#endif //APPLICATIONCONFIG_HPP
