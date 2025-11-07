#pragma once

#include <wv/wvpch.h>
#include <wv/rendering/Window.h>

namespace WillowVox
{
    class Camera
    {
    public:
        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 direction = glm::vec3(0, -90.0f, 0), float fov = 70.0f);
        Camera(float posX, float posY, float posZ, float roll, float pitch, float yaw, float fov = 70.0f);

        glm::vec3 Front();
        glm::vec3 Right();
        glm::vec3 Up();

        glm::mat4 GetViewMatrix();
        glm::mat4 GetProjectionMatrix();

        glm::vec3 m_position;
        glm::vec3 m_direction;
        float m_fov;

    private:
        Window& m_window;
    };
}