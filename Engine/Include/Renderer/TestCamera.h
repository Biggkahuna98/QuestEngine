#pragma once
#include "Core/Core.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Core/Events/EngineEvents.h"

namespace QE
{
    // Defaults
    constexpr float g_YAW = -90.0f;
    constexpr float g_PITCH = 0.0f;
    constexpr float g_SPEED = 2.5f;
    constexpr float g_SENSITIVITY = 0.1f;
    constexpr float g_ZOOM = 45.0f;

    class QUEST_API TestCamera
    {
    public:
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;

        float Yaw;
        float Pitch;
        float MovementSpeed;
        float MouseSensitivity;
        float Zoom;

        TestCamera(glm::vec3 position = glm::vec3{0.0f, 0.0f, 4.0f}, glm::vec3 up = glm::vec3{0.0f, 1.0f, 0.0f}, float yaw = g_YAW, float pitch = g_PITCH);

        glm::mat4 GetViewMatrix();
        void Update(float deltaTime);
        void ProcessMouseMovement(MouseMoveEvent event, bool constrainPitch = true);
        void ProcessMouseScroll(MouseScrollEvent event);
        void DrawDebugInfo();
    private:
        float lastX;
        float lastY;
        bool firstMouse = true;
        void UpdateCameraVectors();
    };
}