#pragma once
#include <string_view>
#include <vector>
#include <map>

#include "camera.h"
#include "material.h"

using XXH32_hash_t = unsigned int;

namespace gl
{
    class ResourceManager
    {
    public:
        struct ObjData
        {
            // Mesh vertices.
            std::vector<glm::vec3> positions = {};
            std::vector<glm::vec2> uvs = {};
            std::vector<glm::vec3> normals = {};
            std::vector<glm::vec3> tangents = {};

            // Mesh material data.
            std::string dir = "";
            bool isPBR = false; // co-opting OBJ's "illum" variable to define whether a material is a PBR one or not. illum = 1 is for PBR, anything else means Blinn-Phong.
            std::string alphaMap = ""; // Tex unit 0
            std::string normalMap = ""; // Tex unit 1
            // Blinn-Phong.
            std::string ambientMap = ""; // Tex unit 2
            std::string diffuseMap = ""; // Tex unit 3
            std::string specularMap = ""; // Tex unit 4
            float shininess = 64.0f;
            // PBR.
            std::string albedoMap = ""; // Tex unit 2
            std::string roughnessMap = ""; // Tex unit 3
            std::string metallicMap = ""; // Tex unit 4
            float ior = 1.0f;
        };

        ResourceManager() = default;
        ~ResourceManager();
        ResourceManager(const ResourceManager&) = delete; // Disallow things like ResourceManager r = ResourceManager::Get(), only allow ResourceManager& r = ResourceManager::Get() .
        static ResourceManager& Get()
        {
            static gl::ResourceManager instance;
            return instance;
        }

        // These functions return the gpu name of the data structure if the hashed data is identical to some instance that has been previously created. Else it returns 0.
        unsigned int RequestVAO(XXH32_hash_t hash) const;
        void AppendNewVAO(unsigned int gpuName, XXH32_hash_t hash = 0);
        unsigned int RequestVBO(XXH32_hash_t hash) const;
        void AppendNewVBO(unsigned int gpuName, XXH32_hash_t hash = 0);
        unsigned int RequestTEX(XXH32_hash_t hash) const;
        void AppendNewTEX(unsigned int gpuName, XXH32_hash_t hash = 0);
        unsigned int RequestPROGRAM(XXH32_hash_t hash) const;
        void AppendNewPROGRAM(unsigned int gpuName, XXH32_hash_t hash = 0);

        void DeleteVAO(unsigned int gpuName);
        void DeleteVBO(unsigned int gpuName);
        void DeleteTEX(unsigned int gpuName);
        void DeletePROGRAM(unsigned int gpuName);

        static std::vector<ObjData> ReadObj(std::string_view path);
        /*
        @brief: This function returns a list of per mesh materials with material related data filled out. Use it to avoid having repetitive sections in a Program::Init().
        */
        static std::vector<Material::Definition> PreprocessMaterialData(const std::vector<ObjData> objData);

        Camera& GetCamera();

        void Shutdown() const;

    private:
        std::map<XXH32_hash_t, unsigned int> VAOs_ = {};
        std::map<XXH32_hash_t, unsigned int> VBOs_ = {};
        std::map<XXH32_hash_t, unsigned int> TEXs_ = {};
        std::map<XXH32_hash_t, unsigned int> PROGRAMs_ = {};

        Camera camera_ = {}; // Most shaders need a view matrix and the camera's position, so it's need to be accessible globally.
    };

}//!gl