#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "mesh.h"
#include "material.h"

namespace gl
{
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

        void Create(const std::string_view path, std::vector<glm::mat4> modelMatrices = { IDENTITY_MAT4 }, Material::Definition material = {});

        void Draw();

        void Translate(glm::vec3 v, size_t modelMatrixIndex = 0);
        void Rotate(glm::vec3 cardinalRotation, size_t modelMatrixIndex = 0);
        void Scale(glm::vec3 v, size_t modelMatrixIndex = 0);

    private:

        std::vector<Mesh> meshes_ = {};
        std::vector<glm::mat4> modelMatrices_ = {}; // TODO: update these GPU side on Draw()
        unsigned int modelMatricesVBO_ = 0;
    };
}//!gl