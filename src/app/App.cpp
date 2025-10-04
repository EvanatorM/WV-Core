#include <wv/app/App.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <wv/Logger.h>
#include <iostream>

namespace WillowVox
{
    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    void App::Run()
    {
        Logger::Log("Using WillowVox Engine");

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow* window = glfwCreateWindow(800, 600, "Window", NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return;
        }
    
        glViewport(0, 0, 800, 600);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        Start();

        while(!glfwWindowShouldClose(window))
        {
            glClear(GL_COLOR_BUFFER_BIT);

            Update();

            Render();

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glfwTerminate();
    }
}