//
// Created by Laeth English on 7/28/25.
//

#ifndef APPLICATIONCONFIG_HPP
#define APPLICATIONCONFIG_HPP

// Globals for application configuration
namespace Core {
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
    constexpr uint8_t VIEW_ID_SECONDARY = 1; // Secondary view, if necessary
    constexpr uint8_t VIEW_ID_COMPUTE_GRAVITY = 2; // View for computing gravitation
    constexpr uint8_t VIEW_ID_COMPUTE_INTEGRATION = 3; // View for integrating physics
}

#endif //APPLICATIONCONFIG_HPP
