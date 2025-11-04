#pragma once

#include <wv/assets/AssetProvider.h>
#include <wv/wvpch.h>
#include <typeindex>

namespace WillowVox
{
    class AssetManager
    {
    public:
        static AssetManager& GetInstance();

        template<typename T>
        std::shared_ptr<T> GetAsset(const std::string& name)
        {
            // Get the asset provider for the given type
            AssetProvider<T>* provider = nullptr;
            auto it = m_AssetTypes.find(typeid(T));
            if (it != m_AssetTypes.end())
                provider = static_cast<AssetProvider<T>*>(it->second.get());
            else
            {
                // Create and register a new provider for this asset type
                m_AssetTypes[typeid(T)] = std::make_unique<AssetProvider<T>>();
                provider = static_cast<AssetProvider<T>*>(m_AssetTypes[typeid(T)].get());
            }

            // Get the asset from the provider
            return provider->GetAsset(name);
        }

    private:
        std::unordered_map<std::type_index, std::unique_ptr<IAssetProvider>> m_AssetTypes;
    };
}