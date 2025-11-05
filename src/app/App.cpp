#include <wv/app/App.h>

#include <wv/Logger.h>
#include <wv/rendering/Renderer.h>
#include <wv/rendering/Window.h>
#include <wv/input/Input.h>
#include <iostream>

namespace WillowVox
{
    float App::m_deltaTime = 0;
    float App::m_lastFrame = 0;

    void App::Run()
    {
        Logger::Log("Using WillowVox Engine");     
        
        Renderer::Init();
        Window::InitWindow(1280, 720, "WillowVox Engine");
        auto& window = Window::GetInstance();
        window.SetBackgroundColor(0.1f, 0.1f, 0.1f, 1.0f);

        Input::Init();
    
        Start();

        while(!window.ShouldClose())
        {
            // Calculate deltaTime
            float frameStartTime = Renderer::GetTime();
            float currentFrame = frameStartTime;
            m_deltaTime = currentFrame - m_lastFrame;
            m_lastFrame = currentFrame;

            // Clear window
            window.Clear();

            // Client app logic
            Update();

            Render();

            // End-of-frame steps
            Input::ResetStates();
            window.SwapBuffers();
            window.PollEvents();
        }

        Renderer::Shutdown();
    }
}