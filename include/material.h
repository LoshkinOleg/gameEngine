#pragma once

#include <utility>
#include <map>
#include <string_view>

#include "texture.h"

#include <glm/glm.hpp>

namespace gl
{
    using MaterialId = unsigned int;
    using TextureId = unsigned int;

    class Material
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

            std::vector<std::pair<std::string_view, Texture::Type>> texturePathsAndTypes = {};
            bool flipImages = true;
            bool correctGamma = true;
            bool useHdr = false;

            std::string_view vertexPath = "";
            std::string_view fragmentPath = "";
        };

        void Bind() const;
        void Unbind() const;
        void AddDynamicUniformPair(std::pair<std::string_view, const glm::vec3*> pair);
    private:
        friend class ResourceManager;

        void OnInit() const; // Called from ResourceManager::CreateResource(Material::Definition)
        void OnDraw() const; // Called from Bind()

        void SetInt(const std::pair<std::string_view, int> pair) const;
        void SetVec3(const std::pair<std::string_view, glm::vec3> pair) const;
        void SetMat4(const std::pair<std::string_view, glm::mat4> pair) const;
        void SetFloat(const std::pair<std::string_view, float> pair) const;

        void SetInt(const std::pair<std::string_view, const int*> pair) const;
        void SetVec3(const std::pair<std::string_view, const glm::vec3*> pair) const;
        void SetMat4(const std::pair<std::string_view, const glm::mat4*> pair) const;
        void SetFloat(const std::pair<std::string_view, const float*> pair) const;

        unsigned int PROGRAM_ = 0;

        std::vector<TextureId> textures_ = {}; // TODO: this still gets filled with DEFAULT_ID if the texture is not valid! Look in RM::CreateResource(Material::Definition) as to why!

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