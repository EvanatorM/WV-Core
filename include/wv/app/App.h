#pragma once

namespace WillowVox
{
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
        virtual void Update() {}
        // Runs at the end of every frame for custom rendering code
        virtual void Render() {}

        // Delta time between frames
        static float m_deltaTime;

    private:
        static float m_lastFrame;
    };

    App* CreateApp();
}