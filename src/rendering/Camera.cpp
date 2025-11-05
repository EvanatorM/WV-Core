#include <wv/rendering/Camera.h>

namespace WillowVox
{
    Camera::Camera(glm::vec3 position, glm::vec3 direction) 
        : m_window(Window::GetInstance()), m_position(position), m_direction(direction)
    {

    }

    // constructor with scalar values
    Camera::Camera(float posX, float posY, float posZ, float roll, float pitch, float yaw)
        : m_window(Window::GetInstance())
    {
        m_position = glm::vec3(posX, posY, posZ);
        m_direction = glm::vec3(pitch, yaw, roll);
    }

    glm::vec3 Camera::Front()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(m_direction.y)) * cos(glm::radians(m_direction.x));
        front.y = sin(glm::radians(m_direction.x));
        front.z = sin(glm::radians(m_direction.y)) * cos(glm::radians(m_direction.x));
        return glm::normalize(front);
    }

    glm::vec3 Camera::Right()
    {
        return glm::normalize(glm::cross(Front(), glm::vec3(0, 1, 0)));
    }

    glm::vec3 Camera::Up()
    {
        return glm::normalize(glm::cross(Right(), Front()));
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 Camera::GetViewMatrix()
    {
        return glm::lookAt(m_position, m_position + Front(), Up());
    }

    glm::mat4 Camera::GetProjectionMatrix()
    {
        glm::ivec2 w = m_window.GetWindowSize();
        return glm::perspective(glm::radians(m_fov), (float)w.x / (float)w.y, 0.1f, 1000.0f);
    }
}