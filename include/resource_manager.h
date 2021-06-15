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
#include "framebuffer.h"
#include "skybox.h"
#include "camera.h"

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
    using FramebufferId = unsigned int;
    using SkyboxId = unsigned int;
    using CameraId = unsigned int;

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
            bool flipImage = false; // TODO: not handling it for Framebuffers, throw a warning in the code for this!
            bool correctGamma = false; // TODO: not handling it for Framebuffers, throw a warning in the code for this!
            bool hdr = false; // TODO: not handling it for GL_TEXTURE_2D, throw a warning in the code for this!
        };
        struct ShaderDefinition
        {
            std::string vertexPath = "";
            std::string fragmentPath = "";
            std::function<void(Shader&, const Model&)> onInit = [](Shader&, const Model&)->void {};
            std::function<void(Shader&, const Model&, const Camera&)> onDraw = [](Shader&, const Model&, const Camera&)->void {};
        };
        struct ModelDefinition
        {
            std::vector<MeshId> meshes = {};
            Transform3dId transform = DEFAULT_ID;
        };
        struct MaterialDefinition
        {
            TextureId ambientMap = DEFAULT_ID;
            TextureId diffuseMap = DEFAULT_ID;
            TextureId specularMap = DEFAULT_ID;
            TextureId normalMap = DEFAULT_ID;
            ShaderId shader = DEFAULT_ID;
            glm::vec3 ambientColor = ONE_VEC3;
            glm::vec3 diffuseColor = ONE_VEC3;
            glm::vec3 specularColor = ONE_VEC3;
            float shininess = 1.0f;
        };
        struct FramebufferDefinition
        {
            std::vector<Framebuffer::Attachments> attachments = { Framebuffer::Attachments::COLOR, Framebuffer::Attachments::DEPTH24_STENCIL8 };
            ShaderId shader = DEFAULT_ID;
            bool hdr = false;
        };
        struct SkyboxDefinition
        {
            Skybox::Paths paths = {};
            ShaderId shader = DEFAULT_ID;
            bool flipImages = false;
            bool correctGamma = false;
            bool hdr = false;
        };
        struct CameraDefinition
        {
            glm::vec3 position = glm::vec3(0.0f, 0.0f, 10.0f);
            glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
            float pitch = 0.0f;
        };

        static ResourceManager& Get()
        {
            static gl::ResourceManager instance;
            return instance;
        }
        const Transform3d& GetTransform(Transform3dId id) const;
        const Texture& GetTexture(TextureId id) const;
        const Material& GetMaterial(MaterialId id) const;
        Camera& GetCamera(CameraId id);
        Model& GetModel(ModelId id);
        const Skybox& GetSkybox(SkyboxId id) const;
        Shader& GetShader(ShaderId id);
        const Framebuffer& GetFramebuffer(FramebufferId id) const;
        std::vector<Shader> GetShaders(const std::vector<ShaderId>& ids) const;
        const VertexBuffer& GetVertexBuffer(VertexBufferId id) const;
        Mesh GetMesh(MeshId id) const;
        std::vector<Mesh> GetMeshes(const std::vector<MeshId>& ids) const;
        ModelId CreateResource(const ModelDefinition def);
        MeshId CreateResource(const MeshDefinition def);
        ShaderId CreateResource(const ShaderDefinition def);
        MaterialId CreateResource(const MaterialDefinition def);
        Transform3dId CreateResource(const Transform3dDefinition def);
        TextureId CreateResource(const TextureDefinition def);
        SkyboxId CreateResource(const SkyboxDefinition def);
        VertexBufferId CreateResource(const VertexBufferDefinition def);
        FramebufferId CreateResource(const FramebufferDefinition def);
        CameraId CreateResource(const CameraDefinition def);
        std::vector<MeshId> LoadObj(const std::string_view path, bool flipTextures = true, bool correctGamma = true);
        bool IsModelIdValid(ModelId id) const;
        bool IsMeshIdValid(MeshId id) const;
        bool IsShaderIdValid(ShaderId id) const;
        bool IsMaterialIdValid(MaterialId id) const;
        bool IsTransform3dIdValid(Transform3dId id) const;
        bool IsTextureIdValid(TextureId id) const;
        bool IsVertexBufferIdValid(VertexBufferId id) const;
        bool IsFramebufferValid(FramebufferId id) const;
        bool IsSkyboxValid(SkyboxId id) const;
        bool IsCameraValid(CameraId id) const;
        void Shutdown();
    private:
        std::map<ModelId, Model> models_ = {};
        std::map<MeshId, Mesh> meshes_ = {};
        std::map<ShaderId, Shader> shaders_ = {};
        std::map<MaterialId, Material> materials_ = {};
        std::map<Transform3dId, Transform3d> transforms_ = {}; // Can't use vectors because of some obscure .find() related error.
        std::map<TextureId, Texture> textures_ = {};
        std::map<VertexBufferId, VertexBuffer> vertexBuffers_ = {};
        std::map<FramebufferId, Framebuffer> framebuffers_ = {};
        std::map<SkyboxId, Skybox> skyboxes_ = {}; // Does it make sense to have multiple skyboxes?... Multi-layered ones maybe?
        std::map<CameraId, Camera> cameras_ = {}; // Can't use vectors because of some obscure .find() related error.
    };

}//!gl