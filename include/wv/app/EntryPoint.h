#pragma once

#include <wv/app/App.h>

extern WillowVox::App* WillowVox::CreateApp();

int main(int argc, char** argv)
{
    auto app = WillowVox::CreateApp();
    app->Run();
    delete app;
}