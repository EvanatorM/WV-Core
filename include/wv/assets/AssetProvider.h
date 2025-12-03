#pragma once

#include <wv/assets/AssetLoader.h>
#include <wv/wvpch.h>

namespace WillowVox
{
    class IAssetProvider
    {
    public:
    };

    // Provides assets of type T, loading them on demand
    template<typename T>
    class AssetProvider : public IAssetProvider
    {
    public:
        // Get asset of type T by name
        // Loads the asset if not already loaded
        std::shared_ptr<T> GetAsset(const std::string& name)
        {
            auto it = m_assets.find(name);
            if (it != m_assets.end())
                return it->second;
            else
            {
                std::shared_ptr<T> asset = AssetLoader<T>::Load(name);
                m_assets.emplace(name, asset);
                return asset;
            }
        }

        // Add asset of type T by name
        // Used to manually add assets (e.g., pre-loaded or runtime-generated)
        void AddAsset(const std::string& name, std::shared_ptr<T> asset)
        {
            m_assets.emplace(name, asset);
        }

    private:
        std::unordered_map<std::string, std::shared_ptr<T>> m_assets;
    };
}