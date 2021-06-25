#pragma once
#include <vector>

#ifndef TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#endif // !TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <glm/glm.hpp>

namespace gl
{
    using ModelId = unsigned int;
    using MeshId = unsigned int;

    class Model
    {
    public:
        struct ObjData
        {
            struct MaterialData
            {
                // Note: this must be strings, if using string_views, the views point to freed data by the time they're accessed by the ResourceManager for creation of a Material.
                std::string ambientMap = "";
                std::string alphaMap = "";
                std::string diffuseMap = "";
                std::string specularMap = "";
                std::string normalMap = "";
                std::string roughnessMap = "";
                std::string metallicMap = "";
                std::string sheenMap = "";
                std::string emissiveMap = "";
                float shininess = 1.0f;
                float ior = 1.0f;
                // NOTE: we won't bother with scalar factors for the textures.
            };

            std::vector<glm::vec3> positions = {};
            std::vector<glm::vec2> texCoords = {}; // NOTE: vec2 because the obj format does not support cubemaps.
            std::vector<glm::vec3> normals = {};
            std::vector<glm::vec3> tangents = {};
            MaterialData material = {};
        };
        struct Definition
        {
            std::vector<MeshId> meshes = {};
            std::vector<glm::mat4> modelMatrices = {};
        };

        void Create(const std::string_view path);
        void Create(Definition def);

        void Draw() const;

        void Translate(glm::vec3 v, size_t modelMatrixIndex = 0);
        void Rotate(glm::vec3 cardinalRotation, size_t modelMatrixIndex = 0);
        void Scale(glm::vec3 v, size_t modelMatrixIndex = 0);

        const std::vector<MeshId> GetMesheIds() const;

    private:
        std::vector<gl::ResourceManager::ObjData> ReadObj(const std::string_view path);

        std::vector<MeshId> meshes_ = {};
        std::vector<glm::mat4> modelMatrices_ = {}; // TODO: update these GPU side on Draw()
        unsigned int modelMatricesVBO_ = 0;
    };
}//!gl