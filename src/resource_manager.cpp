#include "resource_manager.h"

#include <string>

#include <glad/glad.h>
#ifndef XXH_INLINE_ALL
#define XXH_INLINE_ALL
#endif // !XXH_INLINE_ALL
#include "xxhash.h"

#include "defines.h"

gl::ResourceManager::~ResourceManager()
{
    EngineWarning("Destroying an instance of ResourceManager. This should only happen at the end of the program's lifetime.");
}

int gl::ResourceManager::GetUniformName(std::string_view strName, unsigned int gpuProgramName)
{
    const auto match = uniformNames_.find(std::to_string(gpuProgramName) + strName.data());
    if (match != uniformNames_.end()) // Name of uniform already known, use it.
    {
        return match->second;
    }
    else
    {
        const int uniformIntName = glGetUniformLocation(gpuProgramName, strName.data());
        CheckGlError();
        uniformNames_.insert({ std::to_string(gpuProgramName) + strName.data(), uniformIntName }); // Add the new entry.
        return uniformIntName;
    }
}

void gl::ResourceManager::Shutdown() const
{
    for (const auto& pair : PROGRAMs_)
    {
        glDeleteProgram(pair.second);
    }
    for (const auto& pair : TEXs_)
    {
        glDeleteTextures(1, &pair.second);
    }
    for (const auto& pair : VBOs_)
    {
        glDeleteBuffers(1, &pair.second);
    }
    for (const auto& pair : VAOs_)
    {
        glDeleteVertexArrays(1, &pair.second);
    }
}

GLuint gl::ResourceManager::RequestVAO(XXH32_hash_t hash) const
{
    const auto match = VAOs_.find(hash);
    if (match != VAOs_.end()) // Such a VAO exists already, return it'd gpu name.
    {
        EngineWarning("VAO hash already in the map. Returning existing gpu name.");
        return match->second;
    }
    else return 0; // No VAO with such data exists, let the caller create a new VBO.
}

void gl::ResourceManager::AppendNewVAO(GLuint gpuName, XXH32_hash_t hash)
{
    if (hash == 0) hash = VAOs_.size();
    assert(VAOs_.find(hash) == VAOs_.end());

    VAOs_.insert({ hash, gpuName });
}

GLuint gl::ResourceManager::RequestVBO(XXH32_hash_t hash) const
{
    const auto match = VBOs_.find(hash);
    if (match != VBOs_.end())
    {
        EngineWarning("VBO hash already in the map. Returning existing gpu name.");
        return match->second;
    }
    else return 0;
}

void gl::ResourceManager::AppendNewVBO(GLuint gpuName, XXH32_hash_t hash)
{
    if (hash == 0) hash = VBOs_.size();
    assert(VBOs_.find(hash) == VBOs_.end());

    VBOs_.insert({ hash, gpuName });
}

GLuint gl::ResourceManager::RequestTEX(XXH32_hash_t hash) const
{
    const auto match = TEXs_.find(hash);
    if (match != TEXs_.end())
    {
        EngineWarning("TEX hash already in the map. Returning existing gpu name.");
        return match->second;
    }
    else return 0;
}

void gl::ResourceManager::AppendNewTEX(GLuint gpuName, XXH32_hash_t hash)
{
    if (hash == 0) hash = TEXs_.size();
    assert(TEXs_.find(hash) == TEXs_.end());

    TEXs_.insert({ hash, gpuName });
}

GLuint gl::ResourceManager::RequestPROGRAM(XXH32_hash_t hash) const
{
    const auto match = PROGRAMs_.find(hash);
    if (match != PROGRAMs_.end())
    {
        EngineWarning("PROGRAM hash already in the map. Returning existing gpu name.");
        return match->second;
    }
    else return 0;
}

void gl::ResourceManager::AppendNewPROGRAM(GLuint gpuName, XXH32_hash_t hash)
{
    if (hash == 0) hash = PROGRAMs_.size();
    assert(PROGRAMs_.find(hash) == PROGRAMs_.end());

    PROGRAMs_.insert({ hash, gpuName });
}