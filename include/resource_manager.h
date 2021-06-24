#pragma once
#include <map>

#include "vertex_buffer.h"
#include "texture.h"
#include "material.h"
#include "mesh.h"
#include "transform.h"
#include "model.h"
#include "framebuffer.h"
#include "skybox.h"
#include "camera.h"

namespace gl
{
    class ResourceManager
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

        ResourceManager() = default;
        ~ResourceManager();
        ResourceManager(const ResourceManager&) = delete; // Disallow things like ResourceManager r = ResourceManager::Get(), only allow ResourceManager& r = ResourceManager::Get() .
        static inline ResourceManager& Get()
        {
            static gl::ResourceManager instance;
            return instance;
        }

        Transform& GetTransform(TransformId id);
        const Texture& GetTexture(TextureId id) const;
        const std::vector<Texture> GetTextures(std::vector<TextureId> ids) const;
        Material& GetMaterial(MaterialId id);
        Camera& GetCamera(CameraId id);
        Model& GetModel(ModelId id);
        const Skybox& GetSkybox(SkyboxId id) const;
        int GetUniformName(std::string_view strName, unsigned int gpuProgramName);
        const Framebuffer& GetFramebuffer(FramebufferId id) const;
        const VertexBuffer& GetVertexBuffer(VertexBufferId id) const;
        const Mesh GetMesh(MeshId id) const;
        const std::vector<Mesh> GetMeshes(const std::vector<MeshId>& ids) const;

        ModelId CreateResource(const Model::Definition def);
        MeshId CreateResource(const Mesh::Definition def);
        MaterialId CreateResource(const Material::Definition def);
        TransformId CreateResource(const Transform::Definition def);
        TextureId CreateResource(const Texture::Definition def, unsigned int nrOfFramebufferColorAttachments = 1);
        SkyboxId CreateResource(const Skybox::Definition def);
        VertexBufferId CreateResource(const VertexBuffer::Definition def);
        FramebufferId CreateResource(const Framebuffer::Definition def);
        CameraId CreateResource(const Camera::Definition def);
        ModelId CreateResource(const std::vector<ObjData> objData, const std::vector<glm::mat4> modelMatrices = {IDENTITY_MAT4}, const Material::Definition matdef = Material::Definition(), bool flipImages = true, bool correctGamma = true);

        std::vector<ObjData> ReadObjData(std::string_view path, bool flipNormals = false, bool reverseWindingOrder = false) const;

        bool IsModelIdValid(ModelId id) const;
        bool IsMeshIdValid(MeshId id) const;
        bool IsMaterialIdValid(MaterialId id) const;
        bool IsTransformIdValid(TransformId id) const;
        bool IsTextureIdValid(TextureId id) const;
        bool IsVertexBufferIdValid(VertexBufferId id) const;
        bool IsFramebufferValid(FramebufferId id) const;
        bool IsSkyboxValid(SkyboxId id) const;
        bool IsCameraValid(CameraId id) const;
        void Shutdown();

    private:
        std::map<ModelId, Model> models_ = {};
        std::map<MeshId, Mesh> opaqueMeshes_ = {};
        std::map<MeshId, Mesh> transparentMeshes_ = {};
        std::map<MaterialId, Material> materials_ = {};
        std::map<TransformId, Transform> transforms_ = {}; // Can't use vectors because of some obscure .find() related error.
        std::map<TextureId, Texture> textures_ = {};
        std::map<VertexBufferId, VertexBuffer> vertexBuffers_ = {};
        std::map<FramebufferId, Framebuffer> framebuffers_ = {};
        std::map<SkyboxId, Skybox> skyboxes_ = {}; // Does it make sense to have multiple skyboxes?... Multi-layered ones maybe?
        std::map<CameraId, Camera> cameras_ = {}; // Can't use vectors because of some obscure .find() related error.

        std::map<std::string, int> uniformNames_ = {};
    };

}//!gl