#pragma once

namespace WillowVox
{
    extern const char* appWindowName;
    extern int appDefaultWindowX;
    extern int appDefaultWindowY;

    class App
    {
    public:
        App() = default;
        virtual ~App() = default;

        void Run();

        virtual void Start() {}
        virtual void Update() {}
        virtual void Render() {}

        static float m_deltaTime;

    private:
        static float m_lastFrame;
    };

    App* CreateApp();
}