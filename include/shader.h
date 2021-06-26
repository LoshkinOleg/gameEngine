#pragma once
#include <map>
#include <string_view>

#include <glm/glm.hpp>

namespace gl
{
    class Shader
    {
    public:
        struct Definition
        {
            std::map<std::string_view, int> staticInts = {};
            std::map<std::string_view, glm::vec3> staticVec3s = {};
            std::map<std::string_view, glm::mat4> staticMat4s = {};
            std::map<std::string_view, float> staticFloats = {};

            // NOTE: const* since those value are read only.
            std::map<std::string_view, const int*> dynamicInts = {};
            std::map<std::string_view, const glm::vec3*> dynamicVec3s = {};
            std::map<std::string_view, const glm::mat4*> dynamicMat4s = {};
            std::map<std::string_view, const float*> dynamicFloats = {};

            std::string_view vertexPath = "";
            std::string_view fragmentPath = "";

            uint32_t GetHash() const;
        };

        void Create(Definition def);
        unsigned int GetPROGRAM() const;

        void Bind();
        void Unbind();
    private:
        unsigned int PROGRAM_ = 0;
        bool isBound_ = false;

        void SetInt(const std::pair<std::string_view, int> pair) const;
        void SetVec3(const std::pair<std::string_view, glm::vec3> pair) const;
        void SetMat4(const std::pair<std::string_view, glm::mat4> pair) const;
        void SetFloat(const std::pair<std::string_view, float> pair) const;

        void SetInt(const std::pair<std::string_view, const int*> pair) const;
        void SetVec3(const std::pair<std::string_view, const glm::vec3*> pair) const;
        void SetMat4(const std::pair<std::string_view, const glm::mat4*> pair) const;
        void SetFloat(const std::pair<std::string_view, const float*> pair) const;

        std::map<std::string_view, int> staticInts_ = {}; // ex: texture units
        std::map<std::string_view, glm::vec3> staticVec3s_ = {}; // ex: material's ambient color
        std::map<std::string_view, glm::mat4> staticMat4s_ = {}; // ex: perspective matrix
        std::map<std::string_view, float> staticFloats_ = {}; // ex: material's shininess

        std::map<std::string_view, const int*> dynamicInts_ = {}; // ex: boolean for using normalmapping
        std::map<std::string_view, const glm::vec3*> dynamicVec3s_ = {}; // ex: camera position
        std::map<std::string_view, const glm::mat4*> dynamicMat4s_ = {}; // ex: transform model matrix
        std::map<std::string_view, const float*> dynamicFloats_ = {};
    };
}//!gl