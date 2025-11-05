#pragma once

namespace WillowVox
{
    class Renderer
    {
    public:
        static void Init();
        static void Shutdown();

        static float GetTime();
    };
}