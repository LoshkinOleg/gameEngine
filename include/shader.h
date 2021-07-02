#pragma once
#include <map>
#include <string>
#include <string_view>

#include <glm/glm.hpp>

using XXH32_hash_t = unsigned int;

namespace gl
{
    class Shader
    {
    public:
        struct Definition
        {
            std::map<std::string, int> staticInts = {};
            std::map<std::string, glm::vec3> staticVec3s = {};
            std::map<std::string, glm::mat4> staticMat4s = {};
            std::map<std::string, float> staticFloats = {};

            // NOTE: const* since those value are read only.
            std::map<std::string, const int*> dynamicInts = {};
            std::map<std::string, const glm::vec3*> dynamicVec3s = {};
            std::map<std::string, const glm::mat4*> dynamicMat4s = {};
            std::map<std::string, const float*> dynamicFloats = {};

            std::string vertexPath = "";
            std::string fragmentPath = "";

            XXH32_hash_t GetHash() const;
        };

        void Create(Definition def);
        unsigned int GetPROGRAM() const;

        void Bind();
        void Unbind();
    private:
        unsigned int PROGRAM_ = 0;
        bool isBound_ = false;

        int GetUniformLocation(std::string_view uniformName);

        void SetInt(const std::pair<std::string_view, int> pair);
        void SetVec3(const std::pair<std::string_view, glm::vec3> pair);
        void SetMat4(const std::pair<std::string_view, glm::mat4> pair);
        void SetFloat(const std::pair<std::string_view, float> pair);

        void SetInt(const std::pair<std::string_view, const int*> pair);
        void SetVec3(const std::pair<std::string_view, const glm::vec3*> pair);
        void SetMat4(const std::pair<std::string_view, const glm::mat4*> pair);
        void SetFloat(const std::pair<std::string_view, const float*> pair);

        std::map<std::string, int> staticInts_ = {}; // ex: texture units
        std::map<std::string, glm::vec3> staticVec3s_ = {}; // ex: material's ambient color
        std::map<std::string, glm::mat4> staticMat4s_ = {}; // ex: perspective matrix
        std::map<std::string, float> staticFloats_ = {}; // ex: material's shininess

        std::map<std::string, const int*> dynamicInts_ = {}; // ex: boolean for using normalmapping
        std::map<std::string, const glm::vec3*> dynamicVec3s_ = {}; // ex: camera position
        std::map<std::string, const glm::mat4*> dynamicMat4s_ = {}; // ex: transform model matrix
        std::map<std::string, const float*> dynamicFloats_ = {};

        std::map<std::string, int> uniformNames_ = {};
    };
}//!gl