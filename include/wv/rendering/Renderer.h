#pragma once

namespace WillowVox
{
    class Renderer
    {
    public:
        static void Init();
        static void PostWindowInit();
        static void Shutdown();

        static float GetTime();

        static void SetVsync(bool enabled);
        static bool VysncEnabled() { return m_vsyncEnabled; }

    private:
        static bool m_vsyncEnabled;
    };
}