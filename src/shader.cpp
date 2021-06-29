#include "shader.h"

#include <fstream>
#include <sstream>

// #include <glm/glm.hpp>
#include <glad/glad.h>
#ifndef XXH_INLINE_ALL
#define XXH_INLINE_ALL
#endif // !XXH_INLINE_ALL
#include "xxhash.h"

#include "defines.h"

#include "resource_manager.h"

GLint gl::Shader::GetUniformLocation(std::string_view uniformName)
{
    const auto match = uniformNames_.find(uniformName.data());
    if (match != uniformNames_.end()) // Name of uniform already known, use it.
    {
        return match->second;
    }
    else
    {
        const int uniformIntName = glGetUniformLocation(PROGRAM_, uniformName.data());
        CheckGlError();
        uniformNames_.insert({ uniformName.data(), uniformIntName }); // Add the new entry.
        return uniformIntName;
    }
}

void gl::Shader::SetInt(const std::pair<std::string_view, int> pair)
{
    assert(isBound_);
    const int gpuName = GetUniformLocation(pair.first);
    glUniform1i(gpuName, pair.second);
    CheckGlError();
}

void gl::Shader::SetInt(const std::pair<std::string_view, const int*> pair)
{
    assert(isBound_);
    const int gpuName = GetUniformLocation(pair.first);
    glUniform1i(gpuName, *(pair.second));
    CheckGlError();
}

void gl::Shader::SetVec3(const std::pair<std::string_view, glm::vec3> pair)
{
    assert(isBound_);
    const int gpuName = GetUniformLocation(pair.first);
    glUniform3fv(gpuName, 1, &pair.second[0]);
    CheckGlError();
}

void gl::Shader::SetVec3(const std::pair<std::string_view, const glm::vec3*> pair)
{
    assert(isBound_);
    const int gpuName = GetUniformLocation(pair.first);
    glUniform3fv(gpuName, 1, &(*pair.second)[0]);
    CheckGlError();
}

void gl::Shader::SetMat4(const std::pair<std::string_view, glm::mat4> pair)
{
    assert(isBound_);
    const int gpuName = GetUniformLocation(pair.first);
    glUniformMatrix4fv(gpuName, 1, GL_FALSE, &pair.second[0][0]);
    CheckGlError();
}

void gl::Shader::SetMat4(const std::pair<std::string_view, const glm::mat4*> pair)
{
    CheckGlError();
    assert(isBound_);
    const int gpuName = GetUniformLocation(pair.first);
    // const glm::vec3 value = *pair.second;
    // glUniformMatrix4fv(ResourceManager::Get().GetUniformName(pair.first, PROGRAM_), 1, GL_FALSE, &value[0][0]);
    glUniformMatrix4fv(gpuName, 1, GL_FALSE, &(*pair.second)[0][0]);
    CheckGlError();
}

void gl::Shader::SetFloat(const std::pair<std::string_view, float> pair)
{
    assert(isBound_);
    const int gpuName = GetUniformLocation(pair.first);
    glUniform1f(gpuName, pair.second);
    CheckGlError();
}

void gl::Shader::SetFloat(const std::pair<std::string_view, const float*> pair)
{
    assert(isBound_);
    const int gpuName = GetUniformLocation(pair.first);
    glUniform1f(gpuName, *pair.second);
    CheckGlError();
}

void gl::Shader::Create(Definition def)
{
    assert(!def.vertexPath.empty() && !def.fragmentPath.empty());

    if (PROGRAM_ != 0)
    {
        EngineError("Calling Create() a second time...");
    }

    // Note: this manner of hashing differenciates between identical shader sources if they're in different directories! Shouldn't be a problem since all shaders are in the same folder anyways.
    std::string accumulatedData = std::to_string(def.GetHash());
    const XXH32_hash_t hash = XXH32(accumulatedData.c_str(), sizeof(char) * accumulatedData.size(), HASHING_SEED);

    PROGRAM_ = ResourceManager::Get().RequestPROGRAM(hash);
    staticFloats_ = def.staticFloats;
    staticInts_ = def.staticInts;
    staticMat4s_ = def.staticMat4s;
    staticVec3s_ = def.staticVec3s;
    dynamicFloats_ = def.dynamicFloats;
    dynamicInts_ = def.dynamicInts;
    dynamicMat4s_ = def.dynamicMat4s;
    dynamicVec3s_ = def.dynamicVec3s;
    if (PROGRAM_ != 0)
    {
        return;
    }

    // Load shader.
    std::string vertexCode;
    std::string fragmentCode;
    {
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            vShaderFile.open(def.vertexPath.data());
            fShaderFile.open(def.fragmentPath.data());

            // read file's buffer contents into streams
            std::stringstream vShaderStream, fShaderStream, gShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            vShaderFile.close();
            fShaderFile.close();

            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure&)
        {
            EngineError("Could not open shader file!");
        }
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;
    GLint success;
    GLchar infoLog[1024];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    CheckGlError();
    if (!success)
    {
        glGetShaderInfoLog(vertex, 1024, NULL, infoLog);
        EngineError(infoLog);
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

    CheckGlError();
    if (!success)
    {
        glGetShaderInfoLog(vertex, 1024, NULL, infoLog);
        EngineError(infoLog);
    }

    PROGRAM_ = glCreateProgram();
    glAttachShader(PROGRAM_, vertex);
    glAttachShader(PROGRAM_, fragment);
    glLinkProgram(PROGRAM_);
    glGetProgramiv(PROGRAM_, GL_LINK_STATUS, &success);
    CheckGlError();
    if (!success)
    {
        glGetShaderInfoLog(PROGRAM_, 1024, NULL, infoLog);
        EngineError(infoLog);
    }

    // Set up static uniforms.
    glUseProgram(PROGRAM_);
    isBound_ = true;
    for (const auto& pair : staticFloats_)
    {
        SetFloat(pair);
    }
    for (const auto& pair : staticInts_)
    {
        SetInt(pair);
    }
    for (const auto& pair : staticMat4s_)
    {
        SetMat4(pair);
    }
    for (const auto& pair : staticVec3s_)
    {
        SetVec3(pair);
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glUseProgram(0);
    isBound_ = false;
    CheckGlError();

    ResourceManager::Get().AppendNewPROGRAM(PROGRAM_, hash);
}

unsigned int gl::Shader::GetPROGRAM() const
{
    return PROGRAM_;
}

void gl::Shader::Bind()
{
    glUseProgram(PROGRAM_);
    isBound_ = true;
    // Update dynamic uniforms.
    for (const auto& pair : dynamicFloats_)
    {
        SetFloat(pair);
    }
    for (const auto& pair : dynamicInts_)
    {
        SetInt(pair);
    }
    for (const auto& pair : dynamicMat4s_)
    {
        SetMat4(pair);
    }
    for (const auto& pair : dynamicVec3s_)
    {
        SetVec3(pair);
    }
}

void gl::Shader::Unbind()
{
    // glUseProgram(0);
    isBound_ = false;
}

uint32_t gl::Shader::Definition::GetHash() const
{
    std::string accumulatedData = vertexPath.data();
    accumulatedData += fragmentPath.data();
    for (const auto& pair : staticFloats)
    {
        accumulatedData += pair.first; // Name of shader variable.
        accumulatedData += std::to_string(pair.second);
    }
    for (const auto& pair : staticInts)
    {
        accumulatedData += pair.first;
        accumulatedData += std::to_string(pair.second);
    }
    for (const auto& pair : staticMat4s)
    {
        accumulatedData += pair.first;

        accumulatedData += std::to_string(pair.second[0][0]);
        accumulatedData += std::to_string(pair.second[0][1]);
        accumulatedData += std::to_string(pair.second[0][2]);
        accumulatedData += std::to_string(pair.second[0][3]);

        accumulatedData += std::to_string(pair.second[1][0]);
        accumulatedData += std::to_string(pair.second[1][1]);
        accumulatedData += std::to_string(pair.second[1][2]);
        accumulatedData += std::to_string(pair.second[1][3]);

        accumulatedData += std::to_string(pair.second[2][0]);
        accumulatedData += std::to_string(pair.second[2][1]);
        accumulatedData += std::to_string(pair.second[2][2]);
        accumulatedData += std::to_string(pair.second[2][3]);

        accumulatedData += std::to_string(pair.second[3][0]);
        accumulatedData += std::to_string(pair.second[3][1]);
        accumulatedData += std::to_string(pair.second[3][2]);
        accumulatedData += std::to_string(pair.second[3][3]);
    }
    for (const auto& pair : staticVec3s)
    {
        accumulatedData += pair.first;
        accumulatedData += std::to_string(pair.second[0]);
        accumulatedData += std::to_string(pair.second[1]);
        accumulatedData += std::to_string(pair.second[2]);
    }
    for (const auto& pair : dynamicFloats)
    {
        accumulatedData += pair.first;
        accumulatedData += std::to_string((size_t)&pair.second); // addr of variable being referenced.
    }
    for (const auto& pair : dynamicInts)
    {
        accumulatedData += pair.first;
        accumulatedData += std::to_string((size_t)&pair.second);
    }
    for (const auto& pair : dynamicMat4s)
    {
        accumulatedData += pair.first;
        accumulatedData += std::to_string((size_t)&pair.second);
    }
    for (const auto& pair : dynamicVec3s)
    {
        accumulatedData += pair.first;
        accumulatedData += std::to_string((size_t)&pair.second);
    }
    return XXH32(accumulatedData.c_str(), sizeof(char) * accumulatedData.size(), HASHING_SEED);
}
