#pragma once

#include <wv/assets/AssetProvider.h>
#include <wv/wvpch.h>
#include <typeindex>

namespace WillowVox
{
    // Manages loading and providing assets of various types
    class AssetManager
    {
    public:
        static AssetManager& GetInstance();

        // Get asset of type T by name
        // Loads the asset if not already loaded
        // Uses AssetLoader<T> to load the asset if needed. If you want to support loading a new asset type T,
        // you need to create a template specialization of AssetLoader
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

        // Manually add asset of type T by name
        // This is usually not needed unless you want to pre-load assets or load runtime-generated assets
        template<typename T>
        void AddAsset(const std::string& name, std::shared_ptr<T> asset)
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
            provider->AddAsset(name, asset);
        }

    private:
        std::unordered_map<std::type_index, std::unique_ptr<IAssetProvider>> m_AssetTypes;
    };
}