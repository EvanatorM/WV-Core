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
    };

    App* CreateApp();
}