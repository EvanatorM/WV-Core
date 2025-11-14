#pragma once

#include <wv/assets/AssetLoader.h>
#include <wv/wvpch.h>

namespace WillowVox
{
    class Texture
    {
    public:
        static std::shared_ptr<Texture> FromName(const std::string& name);
        static std::shared_ptr<Texture> FromData(const std::vector<unsigned char>& data, int width, int height);

        Texture(const char* path);
        Texture(const std::vector<unsigned char>& data, int width, int height);
        ~Texture();

        enum TexSlot
        {
            TEX00,
            TEX01,
            TEX02,
            TEX03,
            TEX04,
            TEX05,
            TEX06,
            TEX07,
            TEX08,
            TEX09,
            TEX10,
            TEX11,
            TEX12,
            TEX13,
            TEX14,
            TEX15,
            TEX16
        };

        void BindTexture(TexSlot slot);

        int m_width, m_height;

    private:
        unsigned int m_textureId;
    };

    template<> struct AssetLoader<Texture>
    {
        static std::shared_ptr<Texture> Load(const std::string& name)
        {
            return Texture::FromName(name);
        }
    };
}