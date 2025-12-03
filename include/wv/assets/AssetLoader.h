#pragma once

#include <wv/wvpch.h>

namespace WillowVox
{
    // If you want to support loading a new asset type T,
    // you need to create a template specialization of this struct
    template<typename T>
    struct AssetLoader
    {
        // Load asset of type T from given name (e.g., file path)
        static std::shared_ptr<T> Load(const std::string& name);
    };
}