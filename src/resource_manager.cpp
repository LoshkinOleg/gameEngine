#include "resource_manager.h"

#include <string>
#include <numeric>

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
    for (const auto& vbo : transformModelVBOs_)
    {
        glDeleteBuffers(1, &vbo);
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

void gl::ResourceManager::AppendNewTransformModelVBO(GLuint gpuName)
{
    const auto match = std::find(transformModelVBOs_.begin(), transformModelVBOs_.end(), gpuName);
    assert(match == transformModelVBOs_.end());
    transformModelVBOs_.push_back(gpuName);
}

GLuint gl::ResourceManager::RequestTEX(XXH32_hash_t hash) const
{
    const auto match = TEXs_.find(hash);
    if (match != TEXs_.end())
    {
        // EngineWarning("TEX hash already in the map. Returning existing gpu name.");
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

void gl::ResourceManager::DeleteVAO(GLuint gpuName)
{
    for (const auto& pair : VAOs_)
    {
        if (pair.second == gpuName)
        {
            glDeleteVertexArrays(1, &gpuName);
            VAOs_.erase(pair.first);
            return;
        }
    }
    EngineError("Trying to delete a non existent VAO!");
}

void gl::ResourceManager::DeleteVBO(GLuint gpuName)
{
    for (const auto& pair : VBOs_)
    {
        if (pair.second == gpuName)
        {
            glDeleteBuffers(1, &gpuName);
            VBOs_.erase(pair.first);
            return;
        }
    }
    EngineError("Trying to delete a non existent VBO!");
}

void gl::ResourceManager::DeleteTransformModelVBO(GLuint gpuName)
{
    const auto match = std::find(transformModelVBOs_.begin(), transformModelVBOs_.end(), gpuName);
    assert(match != transformModelVBOs_.end());
    glDeleteBuffers(1, &gpuName);
}

void gl::ResourceManager::DeleteTEX(GLuint gpuName)
{
    for (const auto& pair : TEXs_)
    {
        if (pair.second == gpuName)
        {
            glDeleteTextures(1, &gpuName);
            TEXs_.erase(pair.first);
            return;
        }
    }
    EngineError("Trying to delete a non existent TEX!");
}

void gl::ResourceManager::DeletePROGRAM(GLuint gpuName)
{
    for (const auto& pair : PROGRAMs_)
    {
        if (pair.second == gpuName)
        {
            glDeleteProgram(gpuName);
            PROGRAMs_.erase(pair.first);
            return;
        }
    }
    EngineError("Trying to delete a non existent PROGRAMs_!");
}

gl::Camera& gl::ResourceManager::GetCamera()
{
    return camera_;
}
