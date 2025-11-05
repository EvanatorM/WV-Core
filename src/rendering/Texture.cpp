#include <wv/rendering/Texture.h>

#include <wv/Logger.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <filesystem>

namespace WillowVox
{
    std::shared_ptr<Texture> Texture::FromName(const std::string& name)
    {
        // Base path without extension
        std::string path = "assets/textures/" + name;

        // Get the file based on supported file formats
        // This is a little scuffed right now.
        // I did this because I want to separate the asset name from the asset path, but it's
        // a bit strange to implement with textures due to all of the possible file formats.
        // I haven't figured out what to do for assets yet but this system works for now.
        if (std::filesystem::exists(path + ".png"))
            return std::make_shared<Texture>((path + ".png").c_str());
        else if (std::filesystem::exists(path + ".jpg"))
            return std::make_shared<Texture>((path + ".jpg").c_str());
        else if (std::filesystem::exists(path + ".jpeg"))
            return std::make_shared<Texture>((path + ".jpeg").c_str());
        else if (std::filesystem::exists(path + ".tga"))
            return std::make_shared<Texture>((path + ".tga").c_str());
        else if (std::filesystem::exists(path + ".bmp"))
            return std::make_shared<Texture>((path + ".bmp").c_str());
        else if (std::filesystem::exists(path + ".psd"))
            return std::make_shared<Texture>((path + ".psd").c_str());
        else if (std::filesystem::exists(path + ".hdr"))
            return std::make_shared<Texture>((path + ".hdr").c_str());
        else if (std::filesystem::exists(path + ".pic"))
            return std::make_shared<Texture>((path + ".pic").c_str());
    }

    Texture::Texture(const char* path)
    {
        // Load texture data
        stbi_set_flip_vertically_on_load(true);

        int nrChannels;
        unsigned char* data = stbi_load(path, &m_width, &m_height, &nrChannels, 0);
        if (data)
        {
            // Create texture
            glGenTextures(1, &m_textureId);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_textureId);

            // Set texture parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            // Set texture data
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            Logger::Error("Failed to load texture: %s", path);
        }

        // Free texture data
        stbi_image_free(data);
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &m_textureId);
    }

    void Texture::BindTexture(TexSlot slot)
    {
        switch (slot)
        {
        case TEX00:
            glActiveTexture(GL_TEXTURE0);
            break;
        case TEX01:
            glActiveTexture(GL_TEXTURE1);
            break;
        case TEX02:
            glActiveTexture(GL_TEXTURE2);
            break;
        case TEX03:
            glActiveTexture(GL_TEXTURE3);
            break;
        case TEX04:
            glActiveTexture(GL_TEXTURE4);
            break;
        case TEX05:
            glActiveTexture(GL_TEXTURE5);
            break;
        case TEX06:
            glActiveTexture(GL_TEXTURE6);
            break;
        case TEX07:
            glActiveTexture(GL_TEXTURE7);
            break;
        case TEX08:
            glActiveTexture(GL_TEXTURE8);
            break;
        case TEX09:
            glActiveTexture(GL_TEXTURE9);
            break;
        case TEX10:
            glActiveTexture(GL_TEXTURE10);
            break;
        case TEX11:
            glActiveTexture(GL_TEXTURE11);
            break;
        case TEX12:
            glActiveTexture(GL_TEXTURE12);
            break;
        case TEX13:
            glActiveTexture(GL_TEXTURE13);
            break;
        case TEX14:
            glActiveTexture(GL_TEXTURE14);
            break;
        case TEX15:
            glActiveTexture(GL_TEXTURE15);
            break;
        case TEX16:
            glActiveTexture(GL_TEXTURE16);
            break;
        }

        glBindTexture(GL_TEXTURE_2D, m_textureId);
    }
}