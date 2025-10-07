#include <wv/app/App.h>

#include <wv/Logger.h>
#include <wv/rendering/Renderer.h>
#include <wv/rendering/Window.h>
#include <iostream>

namespace WillowVox
{
    void App::Run()
    {
        Logger::Log("Using WillowVox Engine");     
        
        Renderer::Init();
        Window window(1280, 720, "WillowVox Engine");
        window.SetBackgroundColor(0.1f, 0.1f, 0.1f, 1.0f);
    
        Start();

        while(!window.ShouldClose())
        {
            window.Clear();

            Update();

            Render();

            window.SwapBuffers();
            window.PollEvents();
        }

        Renderer::Shutdown();
    }
}