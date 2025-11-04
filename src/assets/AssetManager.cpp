#include <wv/assets/AssetManager.h>

namespace WillowVox
{
    AssetManager& AssetManager::GetInstance()
    {
        static AssetManager instance;
        return instance;
    }
}