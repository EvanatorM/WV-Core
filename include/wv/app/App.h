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

    class App
    {
    public:
        void Run();

        virtual void Start() {}
        virtual void Update(const InputState& input) {}
        virtual void Render() {}

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