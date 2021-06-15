#include "resource_manager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <glad/glad.h>
#define XXH_INLINE_ALL
#include "xxhash.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "camera.h"
#include "utility.h"

const gl::Transform3d& gl::ResourceManager::GetTransform(gl::Transform3dId id) const
{
    assert(id != DEFAULT_ID);

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
    assert(id != DEFAULT_ID);

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
    assert(id != DEFAULT_ID);

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
gl::Camera& gl::ResourceManager::GetCamera(gl::CameraId id)
{
    assert(id != DEFAULT_ID);

    const auto match = cameras_.find(id);
    if (match != cameras_.end())
    {
        return match->second;
    }
    else
    {
        std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": Trying to access a non existent Camera!" << std::endl;
        abort();
    }
}
gl::Model& gl::ResourceManager::GetModel(gl::ModelId id)
{
    assert(id != DEFAULT_ID);

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
const gl::Skybox& gl::ResourceManager::GetSkybox(SkyboxId id) const
{
    assert(id != DEFAULT_ID);

    auto match = skyboxes_.find(id);
    if (match != skyboxes_.end())
    {
        return match->second;
    }
    else
    {
        std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": Trying to access a non existent Skybox!" << std::endl;
        abort();
    }
}
gl::Shader& gl::ResourceManager::GetShader(gl::ShaderId id)
{
    assert(id != DEFAULT_ID);

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
std::vector<gl::Shader> gl::ResourceManager::GetShaders(const std::vector<gl::ShaderId>& ids) const
{
    std::vector<Shader> returnVal;
    for (const auto& id : ids)
    {
        assert(id != DEFAULT_ID);

        const auto match = shaders_.find(id);
        if (match != shaders_.end())
        {
            returnVal.push_back(match->second);
        }
        else
        {
            std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": Trying to access a non existent Shader!" << std::endl;
            abort();
        }
    }
    return returnVal;
}
const gl::VertexBuffer& gl::ResourceManager::GetVertexBuffer(gl::VertexBufferId id) const
{
    if (id == DEFAULT_ID)
    {
        std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": Trying to access a null VertexBuffer!" << std::endl;
        abort();
    }

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
const gl::Framebuffer& gl::ResourceManager::GetFramebuffer(gl::FramebufferId id) const
{
    assert(id != DEFAULT_ID);

    const auto match = framebuffers_.find(id);
    if (match != framebuffers_.end())
    {
        return match->second;
    }
    else
    {
        std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": Trying to access a non existent Framebuffer!" << std::endl;
        abort();
    }
}
std::vector<gl::Mesh> gl::ResourceManager::GetMeshes(const std::vector<gl::MeshId>&ids) const
{
    std::vector<Mesh> returnVal;
    for (const auto& id : ids)
    {
        assert(id != DEFAULT_ID);

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
gl::Mesh gl::ResourceManager::GetMesh(gl::MeshId id) const
{
    assert(id != DEFAULT_ID);

    const auto match = meshes_.find(id);
    if (match != meshes_.end())
    {
        return match->second;
    }
    else
    {
        std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": Trying to access a non existent Mesh!" << std::endl;
        abort();
    }
}
gl::ModelId gl::ResourceManager::CreateResource(const gl::ResourceManager::ModelDefinition def)
{
    // Accumulate relevant data into a string for hashing.
    std::string accumulatedData = "";
    for (const auto& id : def.meshes)
    {
        accumulatedData += std::to_string(id);
    }
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
    model.transform_ = def.transform;

    // Call onInit of all shaders.
    auto shaders = gl::ResourceManager::Get().GetShaders(model.GetShaderIds());
    for (auto& shader : shaders)
    {
        shader.Bind();
        shader.OnInit(model);
    }
    gl::Shader::Unbind();

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
    accumulatedData += std::to_string(def.shader);

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
    material.shader_ = def.shader;
    material.ambientColor_ = def.ambientColor;
    material.diffuseColor_ = def.diffuseColor;
    material.specularColor_ = def.specularColor;
    material.shininess_ = def.shininess;

    materials_.insert({ hash, material });
    return hash;
}
gl::Transform3dId gl::ResourceManager::CreateResource(const gl::ResourceManager::Transform3dDefinition def)
{
    // NOTE: we want transforms to be able to have identical data, so no hashing.
    gl::Transform3d transform;
    transform.position_ = def.position;
    transform.cardinalsRotation_ = def.cardinalsRotation;
    transform.scale_ = def.scale;
    transform.quaternion_ = glm::quat(def.cardinalsRotation);
    transform.model_ = glm::scale(IDENTITY_MAT4, def.scale) * glm::toMat4(transform.quaternion_) * glm::translate(IDENTITY_MAT4, def.position);
    transform.id_ = (unsigned int)transforms_.size();

    transforms_.insert({transform.id_, transform});

    return transform.id_;
}
gl::FramebufferId gl::ResourceManager::CreateResource(const gl::ResourceManager::FramebufferDefinition def)
{
    if (std::find(def.attachments.begin(), def.attachments.end(), Framebuffer::Attachments::COLOR) == def.attachments.end())
    {
        std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": framebuffer needs at the very least one color attachment!" << std::endl;
        abort();
    }

    std::string accumulatedData = "";
    accumulatedData += std::to_string(def.hdr);
    for (const auto& attachment : def.attachments)
    {
        accumulatedData += std::to_string((int)attachment);
    }

    const XXH32_hash_t hash = XXH32(accumulatedData.c_str(), sizeof(char) * accumulatedData.size(), HASHING_SEED);
    assert(hash != UINT_MAX); // We aren't handling this issue...

    if (IsFramebufferValid(hash))
    {
        std::cout << "WARNING: attempting to create a new Framebuffer with data identical to an existing one. Returning the id of the existing one instead." << std::endl;
        return hash;
    }
    Framebuffer framebuffer;
    framebuffer.attachments_ = def.attachments;

    VertexBufferId vertexBufferId = DEFAULT_ID;
    {
        VertexBufferDefinition def;
        def.data =
        {   // Positions   // TexCoords
            -1.0f, -1.0f,   0.0f, 0.0f,
             1.0f, -1.0f,   1.0f, 0.0f,
             1.0f,  1.0f,   1.0f, 1.0f,

            -1.0f, -1.0f,   0.0f, 0.0f,
             1.0f,  1.0f,   1.0f, 1.0f,
            -1.0f,  1.0f,   0.0f, 1.0f
        };
        def.dataLayout =
        {
            VertexDataTypes::POSITIONS_2D,
            VertexDataTypes::TEXCOORDS_2D,
        };
        vertexBufferId = CreateResource(def);
    }
    framebuffer.vertexBuffer_ = vertexBufferId;

    if (def.shader != DEFAULT_ID) // User wants to use own shader.
    {
        framebuffer.shader_ = def.shader;
    }
    else // User wants to use a default shader.
    {
        ShaderId shaderId = DEFAULT_ID;
        {
            ShaderDefinition sdef;
            sdef.vertexPath = def.hdr ? FRAMEBUFFER_HDR_REINHARD_SHADER[0].data() : FRAMEBUFFER_RGB_SHADER[0].data();
            sdef.fragmentPath = def.hdr ? FRAMEBUFFER_HDR_REINHARD_SHADER[1].data() : FRAMEBUFFER_RGB_SHADER[1].data();
            sdef.onInit = [](Shader& shader, const Model& model)->void
            {
                shader.SetInt(FRAMEBUFFER_TEXTURE_NAME.data(), FRAMEBUFFER_SAMPLER_TEXTURE_UNIT);
            };
            shaderId = CreateResource(sdef);
        }
        framebuffer.shader_ = shaderId;
    }
    // Init shader here since it's not linked to any material.
    Shader& shader = shaders_[framebuffer.shader_];
    const Model dummyModel = {};
    shader.Bind();
    shader.OnInit(dummyModel); // Can pass it a dummy model since the lambda doesn't use it anyways.

    glGenFramebuffers(1, &framebuffer.FBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.FBO_);

    // Important that this creation is right after the binding of the framebuffer!
    TextureId textureId = DEFAULT_ID;
    {
        TextureDefinition tdef;
        tdef.samplerTextureUnitPair = { FRAMEBUFFER_TEXTURE_NAME.data(), FRAMEBUFFER_SAMPLER_TEXTURE_UNIT };
        tdef.hdr = def.hdr;
        textureId = CreateResource(tdef);
    }
    framebuffer.texture_ = textureId;

    glGenRenderbuffers(1, &framebuffer.RBO_);
    glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.RBO_); // Only viable target is GL_RENDERBUFFER, making this type of argument completely redundant... thanks gl.
    if (std::find(def.attachments.begin(), def.attachments.end(), Framebuffer::Attachments::DEPTH24_STENCIL8) != def.attachments.end())
    {
        // Note: using a render buffer with dimensions identical to our screen.
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLsizei)SCREEN_RESOLUTION[0], (GLsizei)SCREEN_RESOLUTION[1]);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer.RBO_);
    }
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    framebuffers_.insert({hash, framebuffer});
    CheckGlError(__FILE__, __LINE__);

    return hash;
}
gl::CameraId gl::ResourceManager::CreateResource(const CameraDefinition def)
{
    // NOTE: no checking for duplicates of cameras, we don't mind them having identical data.
    Camera camera;
    camera.state_.position_ = def.position;
    camera.state_.front_ = def.front;
    camera.state_.up_ = def.up;
    camera.state_.yaw_ = glm::radians(-90.0f); // We want the camera facing -Z.
    camera.state_.pitch_ = def.pitch;
    camera.UpdateCameraVectors_();

    CameraId id = cameras_.size();
    cameras_.insert({id, camera});

    return id;
}
gl::TextureId gl::ResourceManager::CreateResource(const gl::ResourceManager::TextureDefinition def)
{
    // TODO: Currently, if the textures differ only in texture units / sampler names, the image is still loaded, resulting in duplicated data on the gpu. Ex: diffuse and ambient

    // Accumulate all the relevant data in a single string for hashing.
    std::string accumulatedData = "";
    for (const auto& path : def.paths)
    {
        accumulatedData += path;
    }
    accumulatedData += def.samplerTextureUnitPair.first;
    accumulatedData += std::to_string(def.samplerTextureUnitPair.second);
    accumulatedData += std::to_string(def.flipImage);
    accumulatedData += std::to_string(def.correctGamma);
    accumulatedData += std::to_string(def.textureType);

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
    unsigned char* data = nullptr;

    glGenTextures(1, &texture.TEX_);
    CheckGlError(__FILE__, __LINE__);
    glBindTexture(def.textureType, texture.TEX_);

    if (def.textureType == GL_TEXTURE_2D)
    {
        if (def.paths.size() > 0)
        {
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
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else // No path for texture means it's a texture being used by a framebuffer.
        {
            // NOTE: texture used is of same size as our screen.
            if (def.hdr)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, (GLsizei)SCREEN_RESOLUTION[0], (GLsizei)SCREEN_RESOLUTION[1], 0, GL_RGB, GL_FLOAT, nullptr);
            }
            else
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)SCREEN_RESOLUTION[0], (GLsizei)SCREEN_RESOLUTION[1], 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            }
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.TEX_, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        stbi_image_free(data);
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
gl::SkyboxId gl::ResourceManager::CreateResource(const SkyboxDefinition def)
{
    // Accumulate all the relevant data in a single string for hashing.
    std::string accumulatedData = "";
    accumulatedData += def.paths.right;
    accumulatedData += def.paths.left;
    accumulatedData += def.paths.top;
    accumulatedData += def.paths.bottom;
    accumulatedData += def.paths.front;
    accumulatedData += def.paths.back;
    accumulatedData += std::to_string(def.shader);

    const XXH32_hash_t hash = XXH32(accumulatedData.c_str(), sizeof(char) * accumulatedData.size(), HASHING_SEED);
    assert(hash != UINT_MAX); // We aren't handling this issue...

    if (IsFramebufferValid(hash))
    {
        std::cout << "WARNING: attempting to create a new Skybox with data identical to an existing one. Returning the id of the existing one instead." << std::endl;
        return hash;
    }
    Skybox skybox;

    {
        VertexBufferDefinition def;
        def.data =
        {
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };
        def.dataLayout =
        {
            VertexDataTypes::POSITIONS_3D
        };
        skybox.vertexBuffer_ = CreateResource(def);
    }

    {
        TextureDefinition tdef;
        tdef.correctGamma = def.correctGamma;
        tdef.flipImage = def.flipImages;
        tdef.hdr = def.hdr;
        tdef.textureType = GL_TEXTURE_CUBE_MAP;
        tdef.paths =
        {
            def.paths.right.data(),
            def.paths.left.data(),
            def.paths.top.data(),
            def.paths.bottom.data(),
            def.paths.front.data(),
            def.paths.back.data()
        };
        tdef.samplerTextureUnitPair = {CUBEMAP_TEXTURE_NAME.data(), CUBEMAP_SAMPLER_TEXTURE_UNIT};
        skybox.texture_ = CreateResource(tdef);
    }

    if (def.shader != DEFAULT_ID)
    {
        skybox.shader_ = def.shader;
    }
    else // Load default skybox shader.
    {
        ShaderDefinition def;
        def.vertexPath = SKYBOX_SHADER[0];
        def.fragmentPath = SKYBOX_SHADER[1];
        def.onInit = [](Shader& shader, const Model& model)->void
        {
            shader.SetProjectionMatrix(PERSPECTIVE);
            shader.SetInt(CUBEMAP_TEXTURE_NAME.data(), CUBEMAP_SAMPLER_TEXTURE_UNIT);
        };
        def.onDraw = [](Shader& shader, const Model& model, const Camera& camera)->void
        {
            shader.SetViewMatrix(glm::mat4(glm::mat3(camera.GetViewMatrix())));
        };
        skybox.shader_ = CreateResource(def);
    }
    Shader& shader = shaders_[skybox.shader_];
    shader.Bind();
    shader.OnInit(Model{});
    shader.Unbind();

    skyboxes_.insert({hash, skybox});

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
    buffer.verticesCount_ = (int)(def.data.size() / stride);

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
            glVertexAttribPointer((GLuint)i, 3, GL_FLOAT, GL_FALSE, (GLsizei)(stride * sizeof(float)), (void*)accumulatedOffset);
            accumulatedOffset += 3 * sizeof(float);
        }
        else if (def.dataLayout[i] == VertexDataTypes::POSITIONS_2D ||
                def.dataLayout[i] == VertexDataTypes::TEXCOORDS_2D)
        {
            glVertexAttribPointer((GLuint)i, 2, GL_FLOAT, GL_FALSE, (GLsizei)(stride * sizeof(float)), (void*)accumulatedOffset);
            accumulatedOffset += 2 * sizeof(float);
        }
        else
        {
            std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": invalid VertexDataType!" << std::endl;
            abort();
        }
        glEnableVertexAttribArray((GLuint)i);
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

bool gl::ResourceManager::IsFramebufferValid(FramebufferId id) const
{
    if (id == UINT_MAX) return false;
    return (framebuffers_.find(id) != framebuffers_.end()) ? true : false;
}

bool gl::ResourceManager::IsSkyboxValid(SkyboxId id) const
{
    if (id == UINT_MAX) return false;
    return (skyboxes_.find(id) != skyboxes_.end()) ? true : false;
}

bool gl::ResourceManager::IsCameraValid(CameraId id) const
{
    if (id == UINT_MAX) return false;
    return (id >= 0 && id <= cameras_.size()) ? true : false;
}

void gl::ResourceManager::Shutdown()
{
    for (const auto& pair : vertexBuffers_)
    {
        glDeleteBuffers(1, &pair.second.VBO_);
        glDeleteVertexArrays(1, &pair.second.VAO_);
    }
    for (const auto& pair : textures_)
    {
        glDeleteTextures(1, &pair.second.TEX_);
    }
    for (const auto& pair : shaders_)
    {
        glDeleteShader(pair.second.PROGRAM_);
    }
    for (const auto& pair : framebuffers_)
    {
        glDeleteRenderbuffers(1, &pair.second.RBO_);
        glDeleteFramebuffers(1, &pair.second.FBO_);
    }
}

std::vector<gl::MeshId> gl::ResourceManager::LoadObj(const std::string_view path, bool flipTextures, bool correctGamma)
{
    // TODO: currently shaders don't handle normal mapping.

    const std::string dir = std::string(path.begin(), path.begin() + path.find_last_of("/") + 1);

    std::vector<gl::MeshId> returnVal;

    tinyobj::ObjReader reader;
    tinyobj::ObjReaderConfig config;
    config.mtl_search_path = dir;
    config.triangulate = true;

    if (!reader.ParseFromFile(path.data(), config))
    {
        std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": could not open obj file at path:\n" << path << "\nAt directory: " << dir << std::endl;
        abort();
    }
    if (!reader.Warning().empty())
    {
        std::cout << "WARNING at file: " << __FILE__ << ", line: " << __LINE__ << " : tinyobjloader has raised a warning: " << reader.Warning() << std::endl;
    }

    const auto& attrib = reader.GetAttrib();
    const auto& shapes = reader.GetShapes();
    const auto& materials = reader.GetMaterials();
    if (materials.size() < 1)
    {
        std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": loaded obj has no material!" << std::endl;
        abort();
    }
    returnVal.resize(shapes.size());

    for (size_t i = 0; i < shapes.size(); i++)
    {
        const int matId = shapes[i].mesh.material_ids[0]; // 1 mesh = 1 material

        TextureId ambientId = DEFAULT_ID;
        if (!materials[matId].ambient_texname.empty())
        {
            TextureDefinition def;
            def.paths = std::vector<std::string>{ dir + materials[matId].ambient_texname };
            def.correctGamma = correctGamma;
            def.flipImage = flipTextures;
            def.textureType = GL_TEXTURE_2D;
            def.samplerTextureUnitPair = std::pair<std::string, int>{AMBIENT_MAP_SAMPLER_NAME, AMBIENT_SAMPLER_TEXTURE_UNIT};
            ambientId = CreateResource(def);
        }
        else
        {
            std::cout << "WARNING: loaded obj has no ambient map!" << std::endl;
        }

        TextureId diffuseId = DEFAULT_ID;
        if (!materials[matId].diffuse_texname.empty())
        {
            TextureDefinition def;
            def.paths = std::vector<std::string>{ dir + "/" + materials[matId].diffuse_texname };
            def.correctGamma = correctGamma;
            def.flipImage = flipTextures;
            def.textureType = GL_TEXTURE_2D;
            def.samplerTextureUnitPair = std::pair<std::string, int>{ DIFFUSE_MAP_SAMPLER_NAME, DIFFUSE_SAMPLER_TEXTURE_UNIT };
            diffuseId = CreateResource(def);
        }
        else
        {
            std::cout << "WARNING: loaded obj has no diffuse map!" << std::endl;
        }

        TextureId specularId = DEFAULT_ID;
        if (!materials[matId].specular_texname.empty())
        {
            TextureDefinition def;
            def.paths = std::vector<std::string>{ dir + "/" + materials[matId].specular_texname };
            def.correctGamma = false;
            def.flipImage = flipTextures;
            def.textureType = GL_TEXTURE_2D;
            def.samplerTextureUnitPair = std::pair<std::string, int>{ SPECULAR_MAP_SAMPLER_NAME, SPECULAR_SAMPLER_TEXTURE_UNIT };
            specularId = CreateResource(def);
        }
        else
        {
            std::cout << "WARNING: loaded obj has no specular map!" << std::endl;
        }

        TextureId normalId = DEFAULT_ID;
        if (!materials[matId].bump_texname.empty())
        {
            TextureDefinition def;
            def.paths = std::vector<std::string>{ dir + "/" + materials[matId].bump_texname };
            def.correctGamma = false;
            def.flipImage = flipTextures;
            def.textureType = GL_TEXTURE_2D;
            def.samplerTextureUnitPair = std::pair<std::string, int>{ NORMAL_MAP_SAMPLER_NAME, NORMAL_SAMPLER_TEXTURE_UNIT };
            normalId = CreateResource(def);
        }
        else
        {
            std::cout << "WARNING: loaded obj has no normal map!" << std::endl;
        }

        ShaderId shaderId = DEFAULT_ID;
        {
            ShaderDefinition def;
            const auto& mat = materials[matId];
            switch (mat.illum)
            {
                case 0:
                    {
                        def.vertexPath = ILLUM0_SHADER[0];
                        def.fragmentPath = ILLUM0_SHADER[1];
                        def.onInit = [mat](Shader& shader, const Model& model)->void
                        {
                            shader.SetProjectionMatrix(PERSPECTIVE);
                            shader.SetVec3(AMBIENT_COLOR_NAME.data(), glm::vec3(mat.ambient[0], mat.ambient[1], mat.ambient[2]));
                        };
                        def.onDraw = [](Shader& shader, const Model& model, const Camera& camera)->void
                        {
                            shader.SetViewMatrix(camera.GetViewMatrix());
                            shader.SetModelMatrix(model.GetModelMatrix());
                        };
                    }break;
                case 1:
                    {
                        def.vertexPath = ILLUM1_SHADER[0];
                        def.fragmentPath = ILLUM1_SHADER[1];
                        def.onInit = [mat](Shader& shader, const Model& model)->void
                        {
                            shader.SetProjectionMatrix(PERSPECTIVE);
                            shader.SetInt(AMBIENT_MAP_SAMPLER_NAME.data(), AMBIENT_SAMPLER_TEXTURE_UNIT);
                            shader.SetVec3(AMBIENT_COLOR_NAME.data(), glm::vec3(mat.ambient[0], mat.ambient[1], mat.ambient[2]));
                        };
                        def.onDraw = [](Shader& shader, const Model& model, const Camera& camera)->void
                        {
                            shader.SetViewMatrix(camera.GetViewMatrix());
                            shader.SetModelMatrix(model.GetModelMatrix());
                        };
                    }break;
                case 2:
                    {
                        def.vertexPath = ILLUM2_SHADER[0];
                        def.fragmentPath = ILLUM2_SHADER[1];
                        def.onInit = [mat](Shader& shader, const Model& model)->void
                        {
                            shader.SetProjectionMatrix(PERSPECTIVE);
                            shader.SetInt(AMBIENT_MAP_SAMPLER_NAME.data(), AMBIENT_SAMPLER_TEXTURE_UNIT);
                            shader.SetInt(DIFFUSE_MAP_SAMPLER_NAME.data(), DIFFUSE_SAMPLER_TEXTURE_UNIT);
                            shader.SetInt(SPECULAR_MAP_SAMPLER_NAME.data(), SPECULAR_SAMPLER_TEXTURE_UNIT);
                            shader.SetVec3(AMBIENT_COLOR_NAME.data(), glm::vec3(mat.ambient[0], mat.ambient[1], mat.ambient[2]));
                            shader.SetVec3(DIFFUSE_COLOR_NAME.data(), glm::vec3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]));
                            shader.SetVec3(SPECULAR_COLOR_NAME.data(), glm::vec3(mat.specular[0], mat.specular[1], mat.specular[2]));
                            shader.SetFloat(SHININESS_NAME.data(), mat.shininess);
                        };
                        def.onDraw = [](Shader& shader, const Model& model, const Camera& camera)->void
                        {
                            shader.SetViewMatrix(camera.GetViewMatrix());
                            shader.SetModelMatrix(model.GetModelMatrix());
                            shader.SetVec3(VIEW_POSITION_NAME.data(), camera.GetPosition());
                        };
                    }break;
                default:
                {
                    std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": obj's material has an invalid illumination mode: " << materials[matId].illum << std::endl;
                    abort();
                }
            }
            shaderId = CreateResource(def);
        }

        bool hasNormalMap = false;
        MaterialId materialId = DEFAULT_ID;
        {
            MaterialDefinition def;
            def.ambientMap = ambientId;
            def.diffuseMap = diffuseId;
            def.specularMap = specularId;
            def.normalMap = normalId;
            def.shader = shaderId;
            hasNormalMap = normalId == DEFAULT_ID ? false : true;
            def.ambientColor = glm::vec3(materials[matId].ambient[0], materials[matId].ambient[1], materials[matId].ambient[2]);
            def.diffuseColor = glm::vec3(materials[matId].diffuse[0], materials[matId].diffuse[1], materials[matId].diffuse[2]);
            def.specularColor = glm::vec3(materials[matId].specular[0], materials[matId].specular[1], materials[matId].specular[2]);
            def.shininess = (materials[matId].shininess >= 1.0f) ? materials[matId].shininess : 1.0f;
            materialId = CreateResource(def);
        }

        VertexBufferId vertexBufferId = DEFAULT_ID;
        {
            VertexBufferDefinition def;
            std::vector<ResourceManager::VertexDataTypes> layout;

            bool hasNormalVertexData = false;
            bool hasTexCoordVertexData = false;

            // NOTE: Warning: there's no check that every mesh'es vertices in the file have the same layout!
            tinyobj::index_t meshIndices = shapes[i].mesh.indices[0];
            if (meshIndices.vertex_index > -1)
            {
                layout.push_back(ResourceManager::VertexDataTypes::POSITIONS_3D);
            }
            else
            {
                std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": obj file doesn't contain any position data!" << std::endl;
                abort();
            }
            if (meshIndices.normal_index > -1)
            {
                layout.push_back(ResourceManager::VertexDataTypes::NORMALS);
                hasNormalVertexData = true;
            }
            else if(!hasNormalMap)
            {
                std::cout << "WARNING at file: " << __FILE__ << ", line: " << __LINE__ << ": obj file doesn't contain any normals data of any kind!" << std::endl;
            }
            if (meshIndices.texcoord_index > -1)
            {
                layout.push_back(ResourceManager::VertexDataTypes::TEXCOORDS_2D);
                hasTexCoordVertexData = true;
            }
            else
            {
                std::cout << "WARNING at file: " << __FILE__ << ", line: " << __LINE__ << ": obj file doesn't contain any texCoord data!" << std::endl;
            }
            def.dataLayout = layout;

            size_t stride = 0; // Compute stride.
            for (size_t i = 0; i < layout.size(); i++)
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

            const size_t numOfFaces = shapes[i].mesh.num_face_vertices.size();
            std::vector<float> vertexData = std::vector<float>(numOfFaces * 3 * stride);
            for (size_t f = 0; f < (numOfFaces * 3) - 2; f++)
            {
                tinyobj::index_t idx0 = shapes[i].mesh.indices[f + 0];
                tinyobj::index_t idx1 = shapes[i].mesh.indices[f + 1];
                tinyobj::index_t idx2 = shapes[i].mesh.indices[f + 2];

                // Retrieve pos.
                vertexData[stride * f + 0] = attrib.vertices[3 * (size_t)idx0.vertex_index + 0];
                vertexData[stride * f + 1] = attrib.vertices[3 * (size_t)idx0.vertex_index + 1];
                vertexData[stride * f + 2] = attrib.vertices[3 * (size_t)idx0.vertex_index + 2];

                vertexData[stride * f + stride * 1 + 0] = attrib.vertices[3 * (size_t)idx1.vertex_index + 0];
                vertexData[stride * f + stride * 1 + 1] = attrib.vertices[3 * (size_t)idx1.vertex_index + 1];
                vertexData[stride * f + stride * 1 + 2] = attrib.vertices[3 * (size_t)idx1.vertex_index + 2];

                vertexData[stride * f + stride * 2 + 0] = attrib.vertices[3 * (size_t)idx2.vertex_index + 0];
                vertexData[stride * f + stride * 2 + 1] = attrib.vertices[3 * (size_t)idx2.vertex_index + 1];
                vertexData[stride * f + stride * 2 + 2] = attrib.vertices[3 * (size_t)idx2.vertex_index + 2];

                if (hasNormalVertexData) // Retrieve normals.
                {
                    vertexData[stride * f + 3] = attrib.normals[3 * (size_t)idx0.normal_index + 0];
                    vertexData[stride * f + 4] = attrib.normals[3 * (size_t)idx0.normal_index + 1];
                    vertexData[stride * f + 5] = attrib.normals[3 * (size_t)idx0.normal_index + 2];

                    vertexData[stride * f + stride * 1 + 3] = attrib.normals[3 * (size_t)idx1.normal_index + 0];
                    vertexData[stride * f + stride * 1 + 4] = attrib.normals[3 * (size_t)idx1.normal_index + 1];
                    vertexData[stride * f + stride * 1 + 5] = attrib.normals[3 * (size_t)idx1.normal_index + 2];

                    vertexData[stride * f + stride * 2 + 3] = attrib.normals[3 * (size_t)idx2.normal_index + 0];
                    vertexData[stride * f + stride * 2 + 4] = attrib.normals[3 * (size_t)idx2.normal_index + 1];
                    vertexData[stride * f + stride * 2 + 5] = attrib.normals[3 * (size_t)idx2.normal_index + 2];
                }
                if (hasTexCoordVertexData)
                {
                    if (hasNormalVertexData)
                    {
                        vertexData[stride * f + 6] = attrib.texcoords[2 * (size_t)idx0.texcoord_index + 0];
                        vertexData[stride * f + 7] = attrib.texcoords[2 * (size_t)idx0.texcoord_index + 1];

                        vertexData[stride * f + stride * 1 + 6] = attrib.texcoords[2 * (size_t)idx1.texcoord_index + 0];
                        vertexData[stride * f + stride * 1 + 7] = attrib.texcoords[2 * (size_t)idx1.texcoord_index + 1];

                        vertexData[stride * f + stride * 2 + 6] = attrib.texcoords[2 * (size_t)idx2.texcoord_index + 0];
                        vertexData[stride * f + stride * 2 + 7] = attrib.texcoords[2 * (size_t)idx2.texcoord_index + 1];
                    }
                    else
                    {
                        // NOTE: if code gets out of bounds here, check your faces data in the obj. If you don't have texcoords in your file, the faces should look something like "v//vn", not "v/vn".
                        vertexData[stride * f + 3] = attrib.texcoords[2 * (size_t)idx0.texcoord_index + 0];
                        vertexData[stride * f + 4] = attrib.texcoords[2 * (size_t)idx0.texcoord_index + 1];

                        vertexData[stride * f + stride * 1 + 3] = attrib.texcoords[2 * (size_t)idx1.texcoord_index + 0];
                        vertexData[stride * f + stride * 1 + 4] = attrib.texcoords[2 * (size_t)idx1.texcoord_index + 1];

                        vertexData[stride * f + stride * 2 + 3] = attrib.texcoords[2 * (size_t)idx2.texcoord_index + 0];
                        vertexData[stride * f + stride * 2 + 4] = attrib.texcoords[2 * (size_t)idx2.texcoord_index + 1];
                    }
                }
                
            }
            def.data = vertexData;
            vertexBufferId = CreateResource(def);
        }

        MeshDefinition def;
        def.material = materialId;
        def.vertexBuffer = vertexBufferId;
        returnVal[i] = CreateResource(def);
    }
    
    return returnVal;
}