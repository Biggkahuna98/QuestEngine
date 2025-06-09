#include "Renderer/TestCamera.h"
#include "Engine/Engine.h"
#include "Core/Events/EventManager.h"

#include <imgui.h>

namespace QE
{
    TestCamera::TestCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
                Front(glm::vec3{0.0f, 0.0f, -1.0f}), MovementSpeed(g_SPEED), MouseSensitivity(g_SENSITIVITY), Zoom(g_ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;

        // Subscribe to events
        auto eventManager = GetGlobalEventManager();
        eventManager->Subscribe(EventType::MouseMoved, [this](const EventBase& e)
        {
            MouseMoveEvent event = static_cast<const MouseMoveEvent&>(e);
            ProcessMouseMovement(event);
        });

        eventManager->Subscribe(EventType::MouseScrolled, [this](const EventBase& e)
        {
            MouseScrollEvent event = static_cast<const MouseScrollEvent&>(e);
            ProcessMouseScroll(event);
        });

        eventManager->Subscribe(EventType::WindowMouseToggle, [this](const EventBase&e)
        {
           ToggleUpdating();
        });
        LOG_WARN("Subscribed to events in TestCamera");

        UpdateCameraVectors();
    }

    glm::mat4 TestCamera::GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void TestCamera::Update(float deltaTime)
    {
        if (PauseUpdates)
            return;

        auto inputPtr = GetEngine()->GetInputPtr();

        float velocity = MovementSpeed * deltaTime;
        if (inputPtr->IsKeyDown(W))
            Position += Front * velocity;
        if (inputPtr->IsKeyDown(S))
            Position -= Front * velocity;
        if (inputPtr->IsKeyDown(A))
            Position -= Right * velocity;
        if (inputPtr->IsKeyDown(D))
            Position += Right * velocity;

        //ProcessMouseMovement();
        //ProcessMouseScroll();
    }

    void TestCamera::ProcessMouseMovement(MouseMoveEvent event, bool constrainPitch)
    {
        if (PauseUpdates)
            return;

        float xpos = event.MouseX;
        float ypos = event.MouseY;

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = ypos - lastY;

        // The real processing part
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        lastX = xpos;
        lastY = ypos;

        // update Front, Right and Up Vectors using the updated Euler angles
        UpdateCameraVectors();
    }

    void TestCamera::ProcessMouseScroll(MouseScrollEvent event)
    {
        if (PauseUpdates)
            return;

        Zoom -= (float)event.MouseYOffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

    void TestCamera::ToggleUpdating()
    {
        if (PauseUpdates)
        {
            PauseUpdates = false;
            firstMouse = true;
        } else
        {
            PauseUpdates = true;
        }
    }

    void TestCamera::DrawDebugInfo()
    {
        ImGui::Begin("Camera Debug Info");
            ImGui::Text("PosX: %.2f, PosY: %.2f, PosZ: %.2f", Position.x, Position.y, Position.z);
            ImGui::Text("Yaw: %.2f, Pitch: %.2f", Yaw, Pitch);
            ImGui::Text("Zoom: %.2f", Zoom);
        ImGui::End();
    }


    void TestCamera::UpdateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
}