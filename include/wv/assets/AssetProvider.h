#pragma once

#include <wv/assets/IAssetProvider.h>

namespace WillowVox
{
    template<typename T>
    class AssetProvider : public IAssetProvider
    {
    public:
        T GetAsset(const std::string& assetName)
        {
            auto it = m_Assets.find(assetName);
            if (it != m_Assets.end())
            {
                return it->second;
            }
            else
            {
                T asset = LoadAsset(assetName);
                m_Assets[assetName] = asset;
                return asset;
            }
        }

        T LoadAsset(const std::string& assetName);

    private:
        std::unordered_map<std::string, T> m_Assets;
    };
}