#pragma once
#include <map>
#include <vector>
#include <string>
#include <functional>

#include <glm/glm.hpp>

#include "defines.h"
#include "vertex_buffer.h"
#include "transform3d.h"
#include "texture.h"
#include "shader.h"
#include "material.h"
#include "mesh.h"
#include "model.h"

namespace gl
{
    using VertexBufferId = unsigned int;
    using TextureId = unsigned int;
    using ShaderId = unsigned int;
    using MaterialId = unsigned int;
    using MeshId = unsigned int;
    using ModelId = unsigned int;
    using Transform3dId = unsigned int;
    using GLenum = int;

    class ResourceManager
    {
    public:
        struct MeshDefinition
        {
            gl::VertexBufferId vertexBuffer = DEFAULT_ID;
            gl::MaterialId material = DEFAULT_ID;
        };
        enum class VertexDataTypes
        {
            POSITIONS_3D,
            POSITIONS_2D,
            TEXCOORDS_2D,
            TEXCOORDS_3D,
            NORMALS
        };
        struct VertexBufferDefinition
        {
            std::vector<VertexDataTypes> dataLayout = {};
            std::vector<float> data = {};
        };
        struct Transform3dDefinition
        {
            glm::vec3 position = ZERO_VEC3;
            glm::vec3 cardinalsRotation = ZERO_VEC3;
            glm::vec3 scale = ONE_VEC3;
        };
        struct TextureDefinition
        {
            GLenum textureType = DEFAULT_TEX_TYPE;
            std::vector<std::string> paths = {};
            std::pair<std::string, int> samplerTextureUnitPair = {};
            bool flipImage = false;
            bool correctGamma = false;
        };
        struct ShaderDefinition
        {
            std::string vertexPath = "";
            std::string fragmentPath = "";
            std::function<void(Shader&, const Model&)> onInit = nullptr;
            std::function<void(Shader&, const Model&, const Camera&)> onDraw = nullptr;
        };
        struct ModelDefinition
        {
            std::vector<MeshId> meshes = {};
            ShaderId shader = DEFAULT_ID;
            Transform3dId transform = DEFAULT_ID;
        };
        struct MaterialDefinition
        {
            TextureId ambientMap = DEFAULT_ID;
            TextureId diffuseMap = DEFAULT_ID;
            TextureId specularMap = DEFAULT_ID;
            TextureId normalMap = DEFAULT_ID;
            glm::vec3 ambientColor = ONE_VEC3;
            glm::vec3 diffuseColor = ONE_VEC3;
            glm::vec3 specularColor = ONE_VEC3;
            float shininess = 1.0f;
        };

        static ResourceManager& Get()
        {
            static gl::ResourceManager instance;
            return instance;
        }
        const Transform3d& GetTransform(Transform3dId id) const;
        const Texture& GetTexture(TextureId id) const;
        const Material& GetMaterial(MaterialId id) const;
        Model& GetModel(ModelId id);
        Shader& GetShader(ShaderId id);
        const VertexBuffer& GetVertexBuffer(VertexBufferId id) const;
        std::vector<Mesh> GetMeshes(const std::vector<MeshId>& ids) const;
        Mesh GetMesh(MeshId id) const;
        ModelId CreateResource(const ModelDefinition def);
        MeshId CreateResource(const MeshDefinition def);
        ShaderId CreateResource(const ShaderDefinition def);
        MaterialId CreateResource(const MaterialDefinition def);
        Transform3dId CreateResource(const Transform3dDefinition def);
        TextureId CreateResource(const TextureDefinition def);
        VertexBufferId CreateResource(const VertexBufferDefinition def);
        std::vector<MeshId> LoadObj(const std::string_view path, bool flipTextures = true, bool correctGamma = true);
        bool IsModelIdValid(ModelId id) const;
        bool IsMeshIdValid(MeshId id) const;
        bool IsShaderIdValid(ShaderId id) const;
        bool IsMaterialIdValid(MaterialId id) const;
        bool IsTransform3dIdValid(Transform3dId id) const;
        bool IsTextureIdValid(TextureId id) const;
        bool IsVertexBufferIdValid(VertexBufferId id) const;
        void Shutdown();
    private:
        std::map<ModelId, Model> models_ = {};
        std::map<MeshId, Mesh> meshes_ = {};
        std::map<ShaderId, Shader> shaders_ = {};
        std::map<MaterialId, Material> materials_ = {};
        std::map<Transform3dId, Transform3d> transforms_ = {};
        std::map<TextureId, Texture> textures_ = {};
        std::map<VertexBufferId, VertexBuffer> vertexBuffers_ = {};
    };

}//!gl