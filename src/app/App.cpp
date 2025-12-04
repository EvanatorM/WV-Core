#include <wv/app/App.h>

#include <wv/Logger.h>
#include <wv/platform/IPlatform.h>
#include <wv/platform/IGraphicsContext.h>
#include <wv/input/InputManager.h>
#include <wv/input/GamepadInput.h>
#include <wv/input/TouchInput.h>
#include <iostream>

namespace WillowVox
{
    float App::m_deltaTime = 0;
    float App::m_lastFrame = 0;

    void App::Run()
    {
        Logger::EngineLog("Using WillowVox Engine");

        // Create platform abstraction
        m_platform = PlatformFactory::CreatePlatform();
        if (!m_platform)
        {
            Logger::EngineError("Failed to create platform abstraction!");
            return;
        }

        // Initialize platform
        if (!m_platform->Initialize())
        {
            Logger::EngineError("Failed to initialize platform!");
            delete m_platform;
            return;
        }

        Logger::EngineLog("Running on platform: %s", m_platform->GetPlatformName());

        // Get graphics context
        m_graphicsContext = m_platform->GetGraphicsContext();
        if (!m_graphicsContext)
        {
            Logger::EngineError("Failed to get graphics context!");
            m_platform->Shutdown();
            delete m_platform;
            return;
        }

        // Initialize graphics
        if (!m_graphicsContext->Initialize(appDefaultWindowX, appDefaultWindowY, appWindowName))
        {
            Logger::EngineError("Failed to initialize graphics context!");
            m_platform->Shutdown();
            delete m_platform;
            return;
        }

        m_graphicsContext->SetBackgroundColor(0.1f, 0.1f, 0.1f, 1.0f);

        // Call user's Start function
        Start();

        // Main loop
        while(!m_graphicsContext->ShouldClose())
        {
            // Calculate deltaTime
            float currentFrame = m_graphicsContext->GetTime();
            m_deltaTime = currentFrame - m_lastFrame;
            m_lastFrame = currentFrame;

            // Process platform events
            m_platform->ProcessEvents();

            // Poll input
            m_platform->PollInput(m_inputState);

            // Update centralized input managers
            InputManager::SetInputState(&m_inputState);
            GamepadInput::SetInputState(&m_inputState);
            GamepadInput::s_platform = m_platform;  // Set platform for vibration
            TouchInput::SetInputState(&m_inputState);

            // Begin frame
            m_graphicsContext->BeginFrame();

            // Clear
            m_graphicsContext->Clear(0.1f, 0.1f, 0.1f, 1.0f);

            // Client app logic
            Update(m_inputState);

            // Client rendering
            Render();

            // End frame
            m_graphicsContext->EndFrame();

            // Swap buffers
            m_graphicsContext->SwapBuffers();

            // Reset per-frame input states
            m_platform->ResetInputFrameState(m_inputState);

            // Platform-specific per-frame update
            m_platform->Update(m_deltaTime);
        }

        // Shutdown
        m_graphicsContext->Shutdown();
        m_platform->Shutdown();
        delete m_platform;

        Logger::EngineLog("WillowVox Engine shutdown");
    }
}