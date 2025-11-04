#pragma once

#include <wv/assets/AssetLoader.h>
#include <wv/wvpch.h>

namespace WillowVox
{
    class IAssetProvider
    {
    public:
    };

    template<typename T>
    class AssetProvider : public IAssetProvider
    {
    public:
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

        void AddAsset(const std::string& name, std::shared_ptr<T> asset)
        {
            m_assets.emplace(name, asset);
        }

    private:
        std::unordered_map<std::string, std::shared_ptr<T>> m_assets;
    };
}