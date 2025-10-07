#include <wv/rendering/Renderer.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace WillowVox
{
    void Renderer::Init()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    }

    void Renderer::Shutdown()
    {
        glfwTerminate();
    }
}