#pragma once

#include <wv/assets/AssetLoader.h>
#include <wv/wvpch.h>

namespace WillowVox
{
    class Shader
    {
    public:
        static std::shared_ptr<Shader> FromFiles(const char* vertexShaderPath, const char* fragmentShaderPath);
        static std::shared_ptr<Shader> FromFiles(const std::string& name);
        static std::shared_ptr<Shader> FromSource(const char* vertexShaderCode, const char* fragmentShaderCode);

        Shader(unsigned int programId) : _programId(programId) {}
        // Make sure destructor only runs on the main thread
        ~Shader();

        void Bind();

        void SetBool(const char* name, bool value) const;
        void SetInt(const char* name, int value) const;
        void SetFloat(const char* name, float value) const;
        void SetVec2(const char* name, glm::vec2 value) const;
        void SetVec2(const char* name, float x, float y) const;
        void SetVec3(const char* name, glm::vec3 value) const;
        void SetVec3(const char* name, float x, float y, float z) const;
        void SetVec4(const char* name, glm::vec4 value) const;
        void SetVec4(const char* name, float x, float y, float z, float w) const;
        void SetMat4(const char* name, glm::mat4 value) const;

    private:
        unsigned int _programId;
    };

    template<> struct AssetLoader<Shader>
    {
        static std::shared_ptr<Shader> Load(const std::string& name)
        {
            return Shader::FromFiles(name);
        }
    };
}