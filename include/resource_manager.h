#pragma once
#include <string_view>
#include <map>

using XXH32_hash_t = uint32_t;
using GLuint = unsigned int;

namespace gl
{
    class ResourceManager
    {
    public:
        ResourceManager() = default;
        ~ResourceManager();
        ResourceManager(const ResourceManager&) = delete; // Disallow things like ResourceManager r = ResourceManager::Get(), only allow ResourceManager& r = ResourceManager::Get() .
        static ResourceManager& Get()
        {
            static gl::ResourceManager instance;
            return instance;
        }

        // These functions return the gpu name of the data structure if the hashed data is identical to some instance that has been previously created. Else it returns 0.
        GLuint RequestVAO(XXH32_hash_t hash) const;
        void AppendNewVAO(GLuint gpuName, XXH32_hash_t hash = 0);
        GLuint RequestVBO(XXH32_hash_t hash) const;
        void AppendNewVBO(GLuint gpuName, XXH32_hash_t hash = 0);
        GLuint RequestTEX(XXH32_hash_t hash) const;
        void AppendNewTEX(GLuint gpuName, XXH32_hash_t hash = 0);
        GLuint RequestPROGRAM(XXH32_hash_t hash) const;
        void AppendNewPROGRAM(GLuint gpuName, XXH32_hash_t hash = 0);

        int GetUniformName(std::string_view strName, unsigned int gpuProgramName);

        void Shutdown() const;

    private:
        std::map<XXH32_hash_t, GLuint> VAOs_ = {};
        std::map<XXH32_hash_t, GLuint> VBOs_ = {};
        std::map<XXH32_hash_t, GLuint> TEXs_ = {};
        std::map<XXH32_hash_t, GLuint> PROGRAMs_ = {};

        std::map<std::string_view, int> uniformNames_ = {};
    };

}//!gl