#pragma once

#include <wv/platform/InputState.h>

namespace WillowVox
{
    // Forward declarations
    class IPlatform;
    class IGraphicsContext;

    extern const char* appWindowName;
    extern int appDefaultWindowX;
    extern int appDefaultWindowY;

    // This is the base application class
    // The project must create a subclass of this
    class App
    {
    public:
        App() = default;
        virtual ~App() = default;

        // To be used by the engine. Starts the application
        void Run();

        // Runs at the start of the project
        virtual void Start() {}
        // Runs every frame
        virtual void Update(const InputState& input) {}
        // Runs at the end of every frame for custom rendering code
        virtual void Render() {}

        // Delta time between frames
        static float m_deltaTime;

        // Access to platform and graphics context
        IPlatform* GetPlatform() const { return m_platform; }
        IGraphicsContext* GetGraphicsContext() const { return m_graphicsContext; }

    protected:
        // Derived apps can access input state
        InputState m_inputState;

    private:
        static float m_lastFrame;

        IPlatform* m_platform = nullptr;
        IGraphicsContext* m_graphicsContext = nullptr;
    };

    App* CreateApp();
}