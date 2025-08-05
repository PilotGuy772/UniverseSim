//
// Created by Laeth English on 8/5/25.
//

#ifndef CAMERAHANDLER_HPP
#define CAMERAHANDLER_HPP

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "SDL3/SDL_events.h"

namespace UI {
    inline glm::vec3 CameraPosition{0.0f, 0.0f, 0.0f};
    inline glm::quat CameraOrientation{0.0f, 0.0f, 0.0f, 1.0f};

    inline bool MouseLocked = false;
    inline int MouseX = 0;
    inline int MouseY = 0;

    inline float CameraMoveSpeed = 10.0f;
    inline float CameraRotationSensitivity = 1.0f;

    // SDL event handling
    void SetMouseLocked(bool locked);
    void HandleMouseEvent(const SDL_Event& event);
    void HandleMouseMotion(float dx, float dy);
    void HandleMouseWheel(float delta);

    // Camera rotation
    void RotateCamera(float pitch, float yaw);
    glm::vec3 GetUpVector();
    glm::vec3 GetRightVector();
    glm::vec3 GetForwardVector();

    // matrices
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();



}

#endif //CAMERAHANDLER_HPP
