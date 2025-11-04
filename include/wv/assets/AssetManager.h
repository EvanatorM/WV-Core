#pragma once

#include <wv/assets/IAssetProvider.h>
#include <wv/wvpch.h>
#include <typeindex>

namespace WillowVox
{
    class AssetManager
    {
    public:
        static AssetManager& GetInstance();

        template<typename T>
        void GetAsset(const std::string& assetName)
        {
            IAssetProvider* provider = nullptr;
            auto it = m_AssetTypes.find(typeid(T));
            if (it != m_AssetTypes.end())
            {
                provider = it->second.get();
                // Use provider to get the asset
            }
            else
            {
                // Create and register a new provider for this asset type
                m_AssetTypes[typeid(T)] = std::make_unique<IAssetProvider>();
                provider = m_AssetTypes[typeid(T)].get();
            }


        }

    private:
        std::unordered_map<std::type_index, std::unique_ptr<IAssetProvider>> m_AssetTypes;
    };
}