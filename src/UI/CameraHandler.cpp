//
// Created by Laeth English on 8/5/25.
//

#include "CameraHandler.hpp"

#include "../Core/Application.hpp"
#include "../Core/ApplicationConfig.hpp"

void UI::SetMouseLocked(bool locked) {
    MouseLocked = locked;

    if (locked) {
        SDL_HideCursor();
        SDL_SetWindowRelativeMouseMode(Core::Window, true);

        float mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        MouseX = static_cast<int>(mouseX);
        MouseY = static_cast<int>(mouseY);
    }
    else {
        SDL_ShowCursor();
        SDL_SetWindowRelativeMouseMode(Core::Window, false);
    }
}

void UI::HandleMouseEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_EVENT_MOUSE_MOTION:
            if (MouseLocked) {
                HandleMouseMotion(static_cast<float>(event.motion.xrel), static_cast<float>(event.motion.yrel));
            }
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            HandleMouseWheel(static_cast<float>(event.wheel.y));
            break;
        default:
            break;
    }
}

void UI::HandleMouseMotion(float dx, float dy) {
    if (!MouseLocked) return;

    float yawDelta = dx * CameraRotationSensitivity;
    float pitchDelta = dy * CameraRotationSensitivity;

    RotateCamera(pitchDelta, yawDelta);
}

void UI::HandleMouseWheel(float delta) {
    if (!MouseLocked) return;

    CameraPosition += GetForwardVector() * delta * CameraMoveSpeed;
}

void UI::RotateCamera(float pitch, float yaw) {
    glm::vec3 right = GetRightVector();

    glm::quat yawRot = glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat pitchRot = glm::angleAxis(pitch, right);

    CameraOrientation = yawRot * CameraOrientation;
    CameraOrientation = pitchRot * CameraOrientation;
    CameraOrientation = glm::normalize(CameraOrientation);
}

glm::vec3 UI::GetUpVector() {
    return CameraOrientation * glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::vec3 UI::GetRightVector() {
    return CameraOrientation * glm::vec3(1.0f, 0.0f, 0.0f);
}

glm::vec3 UI::GetForwardVector() {
    return CameraOrientation * glm::vec3(0.0f, 0.0f, -1.0f);
}

glm::mat4 UI::GetViewMatrix() {
    glm::mat4 rotationMatrix = glm::mat4_cast(glm::conjugate(CameraOrientation));
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -CameraPosition);
    return rotationMatrix * translationMatrix;
}

glm::mat4 UI::GetProjectionMatrix() {
    // Assuming a perspective projection for the camera
    float fov = glm::radians(45.0f); // Field of view in radians
    float aspectRatio = static_cast<float>(Core::DEFAULT_WIDTH) / static_cast<float>(Core::DEFAULT_HEIGHT);
    float nearPlane = 0.1f;
    float farPlane = 100.0f;

    return glm::perspective(fov, aspectRatio, nearPlane, farPlane);
}

