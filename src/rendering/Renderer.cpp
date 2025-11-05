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

    void Renderer::PostWindowInit()
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CW);
    }

    void Renderer::Shutdown()
    {
        glfwTerminate();
    }

    float Renderer::GetTime()
    {
        return glfwGetTime();
    }
}