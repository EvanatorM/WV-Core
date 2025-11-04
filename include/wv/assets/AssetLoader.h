#pragma once

#include <wv/wvpch.h>

namespace WillowVox
{
    template<typename T>
    struct AssetLoader
    {
        static std::shared_ptr<T> Load(const std::string& name);
    };
}