#include "resource_manager.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#define XXH_INLINE_ALL
#include "xxhash.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "utility.h"

const gl::Transform3d& gl::ResourceManager::GetTransform(gl::Transform3dId id) const
{
    const auto match = transforms_.find(id);
    if (match != transforms_.end())
    {
        return match->second;
    }
    else
    {
        std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": Trying to access a non existent Transform3d!" << std::endl;
        abort();
    }
}
const gl::Texture& gl::ResourceManager::GetTexture(gl::TextureId id) const
{
    const auto match = textures_.find(id);
    if (match != textures_.end())
    {
        return match->second;
    }
    else
    {
        std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": Trying to access a non existent Texture!" << std::endl;
        abort();
    }
}
const gl::Material& gl::ResourceManager::GetMaterial(gl::MaterialId id) const
{
    const auto match = materials_.find(id);
    if (match != materials_.end())
    {
        return match->second;
    }
    else
    {
        std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": Trying to access a non existent Material!" << std::endl;
        abort();
    }
}
gl::Model& gl::ResourceManager::GetModel(gl::ModelId id)
{
    const auto match = models_.find(id);
    if (match != models_.end())
    {
        return match->second;
    }
    else
    {
        std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": Trying to access a non existent Model!" << std::endl;
        abort();
    }
}
gl::Shader& gl::ResourceManager::GetShader(gl::ShaderId id)
{
    auto match = shaders_.find(id);
    if (match != shaders_.end())
    {
        return match->second;
    }
    else
    {
        std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": Trying to access a non existent Shader!" << std::endl;
        abort();
    }
}
const gl::VertexBuffer& gl::ResourceManager::GetVertexBuffer(gl::VertexBufferId id) const
{
    const auto match = vertexBuffers_.find(id);
    if (match != vertexBuffers_.end())
    {
        return match->second;
    }
    else
    {
        std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": Trying to access a non existent VertexBuffer!" << std::endl;
        abort();
    }
}
std::vector<gl::Mesh> gl::ResourceManager::GetMeshes(const std::vector<gl::MeshId>&ids) const
{
    std::vector<Mesh> returnVal;
    for (const auto& id : ids)
    {
        const auto match = meshes_.find(id);
        if (match != meshes_.end())
        {
            returnVal.push_back(match->second);
        }
        else
        {
            std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": Trying to access a non existent Mesh!" << std::endl;
            abort();
        }
    }
    return returnVal;
}
gl::ModelId gl::ResourceManager::CreateResource(const gl::ResourceManager::ModelDefinition def)
{
    // Accumulate relevant data into a string for hashing.
    std::string accumulatedData = "";
    for (const auto& id : def.meshes)
    {
        accumulatedData += std::to_string(id);
    }
    accumulatedData += std::to_string(def.shader);
    accumulatedData += std::to_string(def.transform);

    const XXH32_hash_t hash = XXH32(accumulatedData.c_str(), sizeof(char) * accumulatedData.size(), HASHING_SEED);
    assert(hash != UINT_MAX); // We aren't handling this issue...

    if (IsModelIdValid(hash))
    {
        std::cout << "WARNING: attempting to create a new Model with data identical to an existing one. Returning the id of the existing one instead." << std::endl;
        return hash;
    }

    gl::Model model;
    model.id_ = hash;
    model.meshes_ = def.meshes;
    model.shader_ = def.shader;
    model.transform_ = def.transform;

    // Call shader's init function now that the model is set up.
    Shader& shader = shaders_[model.shader_];
    shader.Bind();
    shader.OnInit(model);
    shader.Unbind();

    models_.insert({ hash, model });

    return hash;
}
gl::MeshId gl::ResourceManager::CreateResource(const gl::ResourceManager::MeshDefinition def)
{
    // Accumulate relevant data into a string for hashing.
    std::string accumulatedData = "";
    accumulatedData += std::to_string(def.material);
    accumulatedData += std::to_string(def.vertexBuffer);

    const XXH32_hash_t hash = XXH32(accumulatedData.c_str(), sizeof(char) * accumulatedData.size(), HASHING_SEED);
    assert(hash != UINT_MAX); // We aren't handling this issue...

    if (IsMeshIdValid(hash))
    {
        std::cout << "WARNING: attempting to create a new Mesh with data identical to an existing one. Returning the id of the existing one instead." << std::endl;
        return hash;
    }

    Mesh mesh;
    mesh.id_ = hash;
    mesh.vertexBuffer_ = def.vertexBuffer;
    mesh.material_ = def.material;

    meshes_.insert({ hash, mesh });

    return hash;
}
gl::ShaderId gl::ResourceManager::CreateResource(const gl::ResourceManager::ShaderDefinition def)
{
    // Accumulate relevant data into a string for hashing.
    std::string accumulatedData = "";
    accumulatedData += def.vertexPath;
    accumulatedData += def.fragmentPath;
    // NOTE: not taking into account the code of onInit and onDraw for hashing!

    const XXH32_hash_t hash = XXH32(accumulatedData.c_str(), sizeof(char) * accumulatedData.size(), HASHING_SEED);
    assert(hash != UINT_MAX); // We aren't handling this issue...

    if (IsShaderIdValid(hash))
    {
        std::cout << "WARNING: attempting to create a new Shader with data identical to an existing one. Returning the id of the existing one instead." << std::endl;
        return hash;
    }

    gl::Shader shader;
    shader.id_ = hash;
    shader.onInit_ = def.onInit;
    shader.onDraw_ = def.onDraw;

    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vShaderFile.open(def.vertexPath);
        fShaderFile.open(def.fragmentPath);

        // read file's buffer contents into streams
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": could not open shader file: " << e.what() << std::endl;
        abort();
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
    CheckGlError(__FILE__, __LINE__);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 1024, NULL, infoLog);
        std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": vertex shader compilation has thrown an exception: " << infoLog << std::endl;
        abort();
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

    CheckGlError(__FILE__, __LINE__);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 1024, NULL, infoLog);
        std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": fragment shader compilation has thrown an exception: " << infoLog << std::endl;
        abort();
    }

    shader.PROGRAM_ = glCreateProgram();
    glAttachShader(shader.PROGRAM_, vertex);
    glAttachShader(shader.PROGRAM_, fragment);
    glLinkProgram(shader.PROGRAM_);
    glGetProgramiv(shader.PROGRAM_, GL_LINK_STATUS, &success);
    CheckGlError(__FILE__, __LINE__);
    if (!success)
    {
        glGetShaderInfoLog(shader.PROGRAM_, 1024, NULL, infoLog);
        std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": shader linker has thrown an exception: " << infoLog << std::endl;
        abort();
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glUseProgram(0);
    CheckGlError(__FILE__, __LINE__);

    shaders_.insert({ hash, shader });
    return hash;
}
gl::MaterialId gl::ResourceManager::CreateResource(const gl::ResourceManager::MaterialDefinition def)
{
    // Accumulate all the relevant data in a single string for hashing.
    std::string accumulatedData = "";
    accumulatedData += std::to_string(def.ambientMap);
    accumulatedData += std::to_string(def.diffuseMap);
    accumulatedData += std::to_string(def.specularMap);
    accumulatedData += std::to_string(def.normalMap);
    accumulatedData += std::to_string(def.ambientColor[0]);
    accumulatedData += std::to_string(def.ambientColor[1]);
    accumulatedData += std::to_string(def.ambientColor[2]);
    accumulatedData += std::to_string(def.diffuseColor[0]);
    accumulatedData += std::to_string(def.diffuseColor[1]);
    accumulatedData += std::to_string(def.diffuseColor[2]);
    accumulatedData += std::to_string(def.specularColor[0]);
    accumulatedData += std::to_string(def.specularColor[1]);
    accumulatedData += std::to_string(def.specularColor[2]);
    accumulatedData += std::to_string(def.shininess);

    const XXH32_hash_t hash = XXH32(accumulatedData.c_str(), sizeof(char) * accumulatedData.size(), HASHING_SEED);
    assert(hash != UINT_MAX); // We aren't handling this issue...
    // NOTE: What happens if the hash value overflows? def.ambientMap is a random uint32_t generated by the hasing function, so if you add multiple maps together, it will overflow...

    if (IsMaterialIdValid(hash))
    {
        std::cout << "WARNING: attempting to create a new Material with data identical to an existing one. Returning the id of the existing one instead." << std::endl;
        return hash;
    }
    gl::Material material;
    material.id_ = hash;
    material.ambientMap_ = def.ambientMap;
    material.diffuseMap_ = def.diffuseMap;
    material.specularMap_ = def.specularMap;
    material.normalMap_ = def.normalMap;
    material.ambientColor_ = def.ambientColor;
    material.diffuseColor_ = def.diffuseColor;
    material.specularColor_ = def.specularColor;
    material.shininess_ = def.shininess;

    materials_.insert({ hash, material });
    return hash;
}
gl::Transform3dId gl::ResourceManager::CreateResource(const gl::ResourceManager::Transform3dDefinition def)
{
    // Don't bother hashing transforms, we want there to be duplicates if needed.
    gl::Transform3d transform;
    transform.position_ = def.position;
    transform.cardinalsRotation_ = def.cardinalsRotation;
    transform.scale_ = def.scale;
    transform.quaternion_ = glm::quat(def.cardinalsRotation);
    transform.model_ = glm::scale(IDENTITY_MAT4, def.scale) * glm::toMat4(transform.quaternion_) * glm::translate(IDENTITY_MAT4, def.position);
    transform.id_ = transforms_.size();
    transforms_.insert({ transform.id_, transform });

    return transform.id_;
}
gl::TextureId gl::ResourceManager::CreateResource(const gl::ResourceManager::TextureDefinition def)
{
    // Accumulate all the relevant data in a single string for hashing.
    std::string accumulatedData = "";
    for (const auto& path : def.paths)
    {
        accumulatedData += path;
    }
    accumulatedData += def.samplerTextureUnitPair.first;
    accumulatedData += std::to_string(def.samplerTextureUnitPair.second);

    accumulatedData += std::to_string(def.textureType); // NOTE: ignoring def.flipImage and def.correctGamma for hashing.
    const XXH32_hash_t hash = XXH32(accumulatedData.c_str(), sizeof(char) * accumulatedData.size(), HASHING_SEED);
    assert(hash != UINT_MAX); // We aren't handling this issue...

    if (IsTextureIdValid(hash))
    {
        std::cout << "WARNING: attempting to create a new Texture with data identical to an existing one. Returning the id of the existing one instead." << std::endl;
        return hash;
    }
    gl::Texture texture;
    texture.id_ = hash;
    texture.textureType_ = def.textureType;
    texture.samplerTextureUnitPair_ = def.samplerTextureUnitPair;

    stbi_set_flip_vertically_on_load(def.flipImage);
    int width, height, nrChannels;
    unsigned char* data;

    glGenTextures(1, &texture.TEX_);
    CheckGlError(__FILE__, __LINE__);
    glBindTexture(def.textureType, texture.TEX_);

    if (def.textureType == GL_TEXTURE_2D)
    {
        assert(def.paths.size() == 1);

        data = stbi_load(def.paths[0].c_str(), &width, &height, &nrChannels, 0);
        assert(data);

        int imageColorFormat, gpuColorFormat = 0;
        if (nrChannels == 1)
        {
            imageColorFormat = GL_RED;
            gpuColorFormat = GL_RED;
        }
        else if (nrChannels == 3)
        {
            imageColorFormat = def.correctGamma ? GL_SRGB8 : GL_RGB8;
            gpuColorFormat = GL_RGB;
        }
        else if (nrChannels == 4)
        {
            imageColorFormat = def.correctGamma ? GL_SRGB8_ALPHA8 : GL_RGBA8;
            gpuColorFormat = GL_RGBA;
        }
        else
        {
            std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": nrChannels retrieved from " << def.paths[0] << " is not valid: " << nrChannels << std::endl;
            abort();
        }

        glTexImage2D(GL_TEXTURE_2D, 0, imageColorFormat, width, height, 0, gpuColorFormat, GL_UNSIGNED_BYTE, data);
        CheckGlError(__FILE__, __LINE__);
        stbi_image_free(data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
        CheckGlError(__FILE__, __LINE__);
    }
    else if (def.textureType == GL_TEXTURE_CUBE_MAP)
    {
        assert(def.paths.size() == 6);
        for (unsigned int i = 0; i < 6; i++)
        {
            data = stbi_load(def.paths[i].c_str(), &width, &height, &nrChannels, 0);
            assert(data);

            int imageColorFormat, gpuColorFormat = 0;
            if (nrChannels == 1)
            {
                imageColorFormat = GL_RED;
                gpuColorFormat = GL_RED;
            }
            else if (nrChannels == 3)
            {
                imageColorFormat = def.correctGamma ? GL_SRGB : GL_RGB;
                gpuColorFormat = GL_RGB;
            }
            else if (nrChannels == 4)
            {
                imageColorFormat = def.correctGamma ? GL_SRGB8_ALPHA8 : GL_RGBA;
                gpuColorFormat = GL_RGBA;
            }
            else
            {
                std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": nrChannels retrieved from " << def.paths[0] << " is not valid: " << nrChannels << std::endl;
                abort();
            }

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, imageColorFormat, width, height, 0, gpuColorFormat, GL_UNSIGNED_BYTE, data);
            CheckGlError(__FILE__, __LINE__);
            stbi_image_free(data);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        CheckGlError(__FILE__, __LINE__);
    }
    else
    {
        std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": Trying to create a Texture with an invalid textureType!" << std::endl;
        abort();
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    CheckGlError(__FILE__, __LINE__);

    textures_.insert({ hash, texture });
    return hash;
}
gl::VertexBufferId gl::ResourceManager::CreateResource(const gl::ResourceManager::VertexBufferDefinition def)
{
    // Hash the data of the buffer and check if it's not loaded already.
    const XXH32_hash_t hash = XXH32(def.data.data(), sizeof(float) * def.data.size(), HASHING_SEED);
    assert(hash != UINT_MAX); // This value is reserved for invalid ids.

    if (IsVertexBufferIdValid(hash))
    {
        std::cout << "WARNING: attempting to create a new VertexBuffer with data identical to an existing one. Returning the id of the existing one instead." << std::endl;
        return hash; // Iterator does not point to end of the map, meaning it's pointing to the element that's storing the same data as the one we're trying to initialize.
    }
    // Iterator points to end of the map, meaning no matches were found for this hash, proceed with transfering the data to gpu.
    gl::VertexBuffer buffer;
    buffer.id_ = hash;

    size_t stride = 0; // Compute stride.
    for (size_t i = 0; i < def.dataLayout.size(); i++)
    {
        switch (def.dataLayout[i])
        {
            case gl::ResourceManager::VertexDataTypes::POSITIONS_3D: stride += 3;
                break;
            case gl::ResourceManager::VertexDataTypes::POSITIONS_2D: stride += 2;
                break;
            case gl::ResourceManager::VertexDataTypes::TEXCOORDS_2D: stride += 2;
                break;
            case gl::ResourceManager::VertexDataTypes::TEXCOORDS_3D: stride += 3;
                break;
            case gl::ResourceManager::VertexDataTypes::NORMALS: stride += 3;
                break;
            default:
                std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": invalid VertexDataType!" << std::endl;
                abort();
        }
    }
    buffer.verticesCount_ = def.data.size() / stride;

    // Generate vao/vbo and transfer data to it.
    glGenVertexArrays(1, &buffer.VAO_);
    CheckGlError(__FILE__, __LINE__);
    glBindVertexArray(buffer.VAO_);
    glGenBuffers(1, &buffer.VBO_);
    CheckGlError(__FILE__, __LINE__);
    glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO_);
    glBufferData(GL_ARRAY_BUFFER, def.data.size() * sizeof(float), def.data.data(), GL_STATIC_DRAW);
    CheckGlError(__FILE__, __LINE__);

    // Enable the vertex attribute pointers.
    size_t accumulatedOffset = 0;
    for (size_t i = 0; i < def.dataLayout.size(); i++)
    {
        if (def.dataLayout[i] == VertexDataTypes::POSITIONS_3D ||
            def.dataLayout[i] == VertexDataTypes::TEXCOORDS_3D ||
            def.dataLayout[i] == VertexDataTypes::NORMALS)
        {
            glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), +(void*)accumulatedOffset);
            accumulatedOffset += 3 * sizeof(float);
        }
        else if (def.dataLayout[i] == VertexDataTypes::POSITIONS_2D ||
                def.dataLayout[i] == VertexDataTypes::TEXCOORDS_2D)
        {
            glVertexAttribPointer(i, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), +(void*)accumulatedOffset);
            accumulatedOffset += 2 * sizeof(float);
        }
        else
        {
            std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": invalid VertexDataType!" << std::endl;
            abort();
        }
        glEnableVertexAttribArray(i);
        CheckGlError(__FILE__, __LINE__);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    CheckGlError(__FILE__, __LINE__);

    vertexBuffers_.insert({ hash, buffer });
    return hash;
}

bool gl::ResourceManager::IsModelIdValid(ModelId id) const
{
    if (id == UINT_MAX) return false;
    return (models_.find(id) != models_.end()) ? true : false;
}

bool gl::ResourceManager::IsMeshIdValid(MeshId id) const
{
    if (id == UINT_MAX) return false;
    return (meshes_.find(id) != meshes_.end()) ? true : false;
}

bool gl::ResourceManager::IsShaderIdValid(ShaderId id) const
{
    if (id == UINT_MAX) return false;
    return (shaders_.find(id) != shaders_.end()) ? true : false;
}

bool gl::ResourceManager::IsMaterialIdValid(MaterialId id) const
{
    if (id == UINT_MAX) return false;
    return (materials_.find(id) != materials_.end()) ? true : false;
}

bool gl::ResourceManager::IsTransform3dIdValid(Transform3dId id) const
{
    if (id == UINT_MAX) return false;
    return (id >= 0 && id <= transforms_.size()) ? true : false;
}

bool gl::ResourceManager::IsTextureIdValid(TextureId id) const
{
    if (id == UINT_MAX) return false;
    return (textures_.find(id) != textures_.end()) ? true : false;
}

bool gl::ResourceManager::IsVertexBufferIdValid(VertexBufferId id) const
{
    if (id == UINT_MAX) return false;
    return (vertexBuffers_.find(id) != vertexBuffers_.end()) ? true : false;
}

void gl::ResourceManager::Shutdown()
{
    for (const auto& pair : vertexBuffers_)
    {
        pair.second.Destroy();
    }
    for (const auto& pair : textures_)
    {
        pair.second.Destroy();
    }
    for (const auto& pair : shaders_)
    {
        pair.second.Destroy();
    }
}