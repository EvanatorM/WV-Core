#pragma once

namespace WillowVox
{
    class App
    {
    public:
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