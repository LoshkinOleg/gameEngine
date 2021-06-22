#include "resource_manager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>

#include <glad/glad.h>
#define XXH_INLINE_ALL
#include "xxhash.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "camera.h"
#include "utility.h"

gl::ResourceManager::~ResourceManager()
{
    std::cout << "WARNING at file: " << __FILE__ << ", line: " << __LINE__ << ": ResourceManager has been destroyed. This message should only appear when you close the program!" << std::endl;
}

gl::Transform& gl::ResourceManager::GetTransform(gl::TransformId id)
{
    assert(id != DEFAULT_ID);

    auto match = transforms_.find(id);
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
    if (id == DEFAULT_ID) EngineError("Trying to retireve a non valid Texture!");

    const auto match = textures_.find(id);
    if (match != textures_.end())
    {
        return match->second;
    }
    else
    {
        EngineError("Trying to retireve a non valid Texture!");
    }
}
const std::vector<gl::Texture> gl::ResourceManager::GetTextures(std::vector<TextureId> ids) const
{
    std::vector<Texture> returnVal = std::vector<Texture>(ids.size());
    for (size_t i = 0; i < ids.size(); i++)
    {
        if (ids[i] == DEFAULT_ID) EngineError("Trying to retireve a non valid Texture!");

        returnVal[i] = GetTexture(ids[i]);
    }
    return returnVal;
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
const std::vector<gl::Mesh> gl::ResourceManager::GetMeshes(const std::vector<gl::MeshId>&ids) const
{
    std::vector<Mesh> returnVal = std::vector<Mesh>(ids.size());
    for (size_t i = 0; i < ids.size(); i++)
    {
        if (ids[i] == DEFAULT_ID) EngineError("Trying to retireve a non valid Mesh!");

        returnVal[i] = GetMesh(ids[i]);
    }
    return returnVal;
}
const gl::Mesh gl::ResourceManager::GetMesh(gl::MeshId id) const
{
    assert(id != DEFAULT_ID);

    const auto opaqueMatch = opaqueMeshes_.find(id);
    if (opaqueMatch != opaqueMeshes_.end())
    {
        return opaqueMatch->second;
    }
    else
    {
        const auto transparentMatch = transparentMeshes_.find(id);
        if (transparentMatch != transparentMeshes_.end())
        {
            return transparentMatch->second;
        }
        else
        {
            EngineError("Trying to access a non existent Mesh!");
        }
    }
}
gl::ModelId gl::ResourceManager::CreateResource(const gl::Model::Definition def)
{
    // Accumulate relevant data into a string for hashing.
    std::string accumulatedData = "";
    for (const auto& id : def.meshes)
    {
        accumulatedData += std::to_string(id);
    }
    // Note: we don't take into account the transform models.

    const XXH32_hash_t hash = XXH32(accumulatedData.c_str(), sizeof(char) * accumulatedData.size(), HASHING_SEED);
    assert(hash != UINT_MAX); // We aren't handling this issue...

    if (IsModelIdValid(hash))
    {
        std::cout << "WARNING: attempting to create a new Model with data identical to an existing one. Returning the id of the existing one instead." << std::endl;
        return hash;
    }

    gl::Model model;
    model.meshes_ = def.meshes;
    model.modelMatrices_ = def.modelMatrices;

    // Create a vbo for the model matrices.
    glGenBuffers(1, &model.modelMatricesVBO_);
    glBindBuffer(GL_ARRAY_BUFFER, model.modelMatricesVBO_);
    glBufferData(GL_ARRAY_BUFFER, model.modelMatrices_.size() * sizeof(glm::mat4), &model.modelMatrices_[0], GL_DYNAMIC_DRAW);

    const auto& meshes = GetMeshes(model.meshes_);
    for (size_t i = 0; i < meshes.size(); i++)
    {
        const VertexBuffer& vb = GetVertexBuffer(meshes[i].vertexBuffer_);
        glBindVertexArray(vb.VAO_);
        CheckGlError();
        const size_t vec4Size = sizeof(glm::vec4);
        glEnableVertexAttribArray(MODEL_MATRIX_LOCATION);
        glVertexAttribPointer(MODEL_MATRIX_LOCATION, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
        glEnableVertexAttribArray(MODEL_MATRIX_LOCATION + 1);
        glVertexAttribPointer(MODEL_MATRIX_LOCATION + 1, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)vec4Size);
        glEnableVertexAttribArray(MODEL_MATRIX_LOCATION + 2);
        glVertexAttribPointer(MODEL_MATRIX_LOCATION + 2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
        glEnableVertexAttribArray(MODEL_MATRIX_LOCATION + 3);
        glVertexAttribPointer(MODEL_MATRIX_LOCATION + 3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));
        CheckGlError();
        glVertexAttribDivisor(MODEL_MATRIX_LOCATION, 1);
        glVertexAttribDivisor(MODEL_MATRIX_LOCATION + 1, 1);
        glVertexAttribDivisor(MODEL_MATRIX_LOCATION + 2, 1);
        glVertexAttribDivisor(MODEL_MATRIX_LOCATION + 3, 1);
        CheckGlError();
        glBindVertexArray(0);
        CheckGlError();
    }

    models_.insert({ hash, model });

    return hash;
}
gl::MeshId gl::ResourceManager::CreateResource(const gl::Mesh::Definition def)
{
    // TODO: put the transparent meshes into the proper map.

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
    mesh.vertexBuffer_ = def.vertexBuffer;
    mesh.material_ = def.material;

    opaqueMeshes_.insert({ hash, mesh });

    return hash;
}
gl::MaterialId gl::ResourceManager::CreateResource(const gl::Material::Definition def)
{
    // Accumulate all the relevant data in a single string for hashing.
    std::string accumulatedData = "";
    accumulatedData += def.vertexPath;
    accumulatedData += def.fragmentPath;
    accumulatedData += std::to_string(def.correctGamma);
    accumulatedData += std::to_string(def.flipImages);
    accumulatedData += std::to_string(def.useHdr);

    for (const auto& pair : def.texturePathsAndTypes)
    {
        accumulatedData += pair.first;
        accumulatedData += (int)pair.second;
    }
    for (const auto& pair : def.staticFloats)
    {
        accumulatedData += pair.first; // Name of shader variable.
        accumulatedData += std::to_string(pair.second);
    }
    for (const auto& pair : def.staticInts)
    {
        accumulatedData += pair.first;
        accumulatedData += std::to_string(pair.second);
    }
    for (const auto& pair : def.staticMat4s)
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
    for (const auto& pair : def.staticVec3s)
    {
        accumulatedData += pair.first;
        accumulatedData += std::to_string(pair.second[0]);
        accumulatedData += std::to_string(pair.second[1]);
        accumulatedData += std::to_string(pair.second[2]);
    }
    for (const auto& pair : def.dynamicFloats)
    {
        accumulatedData += pair.first;
        accumulatedData += std::to_string((size_t)&pair.second); // addr of variable being referenced.
    }
    for (const auto& pair : def.dynamicInts)
    {
        accumulatedData += pair.first;
        accumulatedData += std::to_string((size_t)&pair.second);
    }
    for (const auto& pair : def.dynamicMat4s)
    {
        accumulatedData += pair.first;
        accumulatedData += std::to_string((size_t)&pair.second);
    }
    for (const auto& pair : def.dynamicVec3s)
    {
        accumulatedData += pair.first;
        accumulatedData += std::to_string((size_t)&pair.second);
    }

    const XXH32_hash_t hash = XXH32(accumulatedData.c_str(), sizeof(char) * accumulatedData.size(), HASHING_SEED);
    assert(hash != UINT_MAX); // We aren't handling this issue...
    // NOTE: What happens if the hash value overflows? def.ambientMap is a random uint32_t generated by the hasing function, so if you add multiple maps together, it will overflow...

    if (IsMaterialIdValid(hash))
    {
        std::cout << "WARNING: attempting to create a new Material with data identical to an existing one. Returning the id of the existing one instead." << std::endl;
        return hash;
    }
    gl::Material material;
    material.staticFloats_ = def.staticFloats;
    material.staticInts_ = def.staticInts;
    material.staticMat4s_ = def.staticMat4s;
    material.staticVec3s_ = def.staticVec3s;
    material.dynamicFloats_ = def.dynamicFloats;
    material.dynamicInts_ = def.dynamicInts;
    material.dynamicMat4s_ = def.dynamicMat4s;
    material.dynamicVec3s_ = def.dynamicVec3s;
    
    std::vector<TextureId> texIds = std::vector<TextureId>(def.texturePathsAndTypes.size(), DEFAULT_ID);
    for (size_t i = 0; i < def.texturePathsAndTypes.size(); i++)
    {
        if (def.texturePathsAndTypes[i].second == Texture::Type::CUBEMAP)
        {
            if (def.texturePathsAndTypes[i + 1].second != Texture::Type::CUBEMAP ||
                def.texturePathsAndTypes[i + 2].second != Texture::Type::CUBEMAP || 
                def.texturePathsAndTypes[i + 3].second != Texture::Type::CUBEMAP || 
                def.texturePathsAndTypes[i + 4].second != Texture::Type::CUBEMAP || 
                def.texturePathsAndTypes[i + 5].second != Texture::Type::CUBEMAP)
            {
                EngineError("There needs to be 6 textures for a cubemap to function!");
            }
            else
            {
                Texture::Definition tdef;
                tdef.correctGamma = def.correctGamma;
                tdef.flipImage = def.flipImages;
                tdef.useHdr = def.useHdr;
                tdef.paths =
                {
                    def.texturePathsAndTypes[i].first,
                    def.texturePathsAndTypes[i + 1].first,
                    def.texturePathsAndTypes[i + 2].first,
                    def.texturePathsAndTypes[i + 3].first,
                    def.texturePathsAndTypes[i + 4].first,
                    def.texturePathsAndTypes[i + 5].first
                };
                tdef.textureType = Texture::Type::CUBEMAP;
                texIds[i] = CreateResource(tdef);
                i += 6; // Don't try to load the other components of the cubemap again.
                material.staticInts_.insert({CUBEMAP_SAMPLER_NAME, CUBEMAP_TEXTURE_UNIT});
            }
        }
        else
        {
            Texture::Definition tdef;
            tdef.correctGamma = def.correctGamma;
            tdef.flipImage = def.flipImages;
            tdef.useHdr = def.useHdr;
            tdef.paths = { def.texturePathsAndTypes[i].first };
            tdef.textureType = def.texturePathsAndTypes[i].second;
            texIds[i] = CreateResource(tdef);
            /*switch (tdef.textureType)
            {
                case Texture::Type::AMBIENT:
                    {
                        material.staticInts_.insert({ AMBIENT_SAMPLER_NAME, AMBIENT_TEXTURE_UNIT });
                    }break;
                case Texture::Type::ALPHA:
                    {
                        material.staticInts_.insert({ ALPHA_SAMPLER_NAME, ALPHA_TEXTURE_UNIT });
                    }break;
                case Texture::Type::DIFFUSE:
                    {
                        material.staticInts_.insert({ DIFFUSE_SAMPLER_NAME, DIFFUSE_TEXTURE_UNIT });
                    }break;
                case Texture::Type::SPECULAR:
                    {
                        material.staticInts_.insert({ SPECULAR_SAMPLER_NAME, SPECULAR_TEXTURE_UNIT });
                    }break;
                case Texture::Type::NORMAL:
                    {
                        material.staticInts_.insert({ NORMALMAP_SAMPLER_NAME, NORMALMAP_TEXTURE_UNIT });
                    }break;
                case Texture::Type::ROUGHNESS:
                    {
                        material.staticInts_.insert({ ROUGHNESS_SAMPLER_NAME, ROUGHNESS_TEXTURE_UNIT });
                    }break;
                case Texture::Type::METALLIC:
                    {
                        material.staticInts_.insert({ METALLIC_SAMPLER_NAME, METALLIC_TEXTURE_UNIT });
                    }break;
                case Texture::Type::SHEEN:
                    {
                        material.staticInts_.insert({ SHEEN_SAMPLER_NAME, SHEEN_TEXTURE_UNIT });
                    }break;
                case Texture::Type::EMISSIVE:
                    {
                        material.staticInts_.insert({ EMISSIVE_SAMPLER_NAME, EMISSIVE_TEXTURE_UNIT });
                    }break;
                default:
                    Error("Unexpected Texture::Type!");
            }*/
        }
    }
    material.textures_ = texIds;

    // Load shader.
    std::string vertexCode;
    std::string fragmentCode;
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
        std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": vertex shader compilation has thrown an exception: " << infoLog << std::endl;
        abort();
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

    CheckGlError();
    if (!success)
    {
        glGetShaderInfoLog(vertex, 1024, NULL, infoLog);
        std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": fragment shader compilation has thrown an exception: " << infoLog << std::endl;
        abort();
    }

    material.PROGRAM_ = glCreateProgram();
    glAttachShader(material.PROGRAM_, vertex);
    glAttachShader(material.PROGRAM_, fragment);
    glLinkProgram(material.PROGRAM_);
    glGetProgramiv(material.PROGRAM_, GL_LINK_STATUS, &success);
    CheckGlError();
    if (!success)
    {
        glGetShaderInfoLog(material.PROGRAM_, 1024, NULL, infoLog);
        std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": shader linker has thrown an exception: " << infoLog << std::endl;
        abort();
    }

    material.OnInit();

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glUseProgram(0);
    CheckGlError();

    materials_.insert({ hash, material });
    return hash;
}
gl::TransformId gl::ResourceManager::CreateResource(const gl::Transform::Definition def)
{
    // NOTE: we want transforms to be able to have identical data, so no hashing.
    gl::Transform transform;
    transform.position_ = def.position;
    transform.cardinalsRotation_ = def.cardinalsRotation;
    transform.scale_ = def.scale;
    transform.model_ = glm::translate(IDENTITY_MAT4, def.position);
    transform.model_ = glm::rotate(transform.model_, transform.cardinalsRotation_.x, RIGHT_VEC3);
    transform.model_ = glm::rotate(transform.model_, transform.cardinalsRotation_.y, UP_VEC3);
    transform.model_ = glm::rotate(transform.model_, transform.cardinalsRotation_.z, FRONT_VEC3);
    transform.model_ = glm::scale(transform.model_, transform.scale_);

    unsigned int id = (unsigned int)transforms_.size();
    transforms_.insert({id, transform});

    return id;
}
gl::FramebufferId gl::ResourceManager::CreateResource(const gl::Framebuffer::Definition def)
{
    if (!((int)def.attachments & (int)Framebuffer::AttachmentMask::COLOR0)) EngineError("Framebuffer needs at the very least one color attachment!");

    std::string accumulatedData = "";
    accumulatedData += std::to_string((int)def.attachments);
    for (const auto& path : def.shaderPaths)
    {
        accumulatedData += path;
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

    VertexBuffer::Definition vbdef;
    Material::Definition matdef;
    Mesh::Definition mdef;

    vbdef.data =
    {   // Positions   // TexCoords
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f,

        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f,
        -1.0f,  1.0f,   0.0f, 1.0f
    };
    vbdef.dataLayout =
    {
        2, // Positions
        2 // Texcoords
    };

    matdef.correctGamma = false; // Framebuffers have no input.
    matdef.flipImages = false;
    matdef.useHdr = true; // NOTE: we're always using hdr for framebuffers by default.
    matdef.texturePathsAndTypes.push_back({"", Texture::Type::FRAMEBUFFER}); // A framebuffer doesn't have any images to load.
    matdef.vertexPath = def.shaderPaths[0];
    matdef.fragmentPath = def.shaderPaths[1];
    matdef.staticInts.insert({FRAMEBUFFER_SAMPLER_NAME, FRAMEBUFFER_TEXTURE_UNIT}); // To init fbTexture sampler

    glGenFramebuffers(1, &framebuffer.FBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.FBO_);

    // NOTE: It's important to call CreateResource for the texture to be after glBindFramebuffer()!
    mdef.vertexBuffer = CreateResource(vbdef);
    mdef.material = CreateResource(matdef);
    framebuffer.mesh_ = CreateResource(mdef);

    if ((int)framebuffer.attachments_ & (int)Framebuffer::AttachmentMask::DEPTH24 && (int)framebuffer.attachments_ & (int)Framebuffer::AttachmentMask::STENCIL8) // Case color + depth + stencil
    {
        glGenRenderbuffers(1, &framebuffer.RBO_);
        glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.RBO_); // Only viable target is GL_RENDERBUFFER, making this type of argument completely redundant... thanks gl.
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLsizei)SCREEN_RESOLUTION[0], (GLsizei)SCREEN_RESOLUTION[1]);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer.RBO_);
    }
    else if ((int)framebuffer.attachments_ & (int)Framebuffer::AttachmentMask::DEPTH24) // case color + depth
    {
        glGenRenderbuffers(1, &framebuffer.RBO_);
        glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.RBO_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, (GLsizei)SCREEN_RESOLUTION[0], (GLsizei)SCREEN_RESOLUTION[1]);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebuffer.RBO_);
    }
    else if ((int)framebuffer.attachments_ & (int)Framebuffer::AttachmentMask::STENCIL8) // case color + stencil
    {
        glGenRenderbuffers(1, &framebuffer.RBO_);
        glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.RBO_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, (GLsizei)SCREEN_RESOLUTION[0], (GLsizei)SCREEN_RESOLUTION[1]);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer.RBO_);
    }

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    framebuffers_.insert({hash, framebuffer});
    CheckGlError();

    return hash;
}
gl::CameraId gl::ResourceManager::CreateResource(const gl::Camera::Definition def)
{
    // NOTE: no checking for duplicates of cameras, we don't mind them having identical data.
    Camera camera;
    camera.state_.position = def.position;
    camera.state_.front = def.front;
    camera.state_.up = def.up;
    camera.state_.yaw = glm::radians(-90.0f); // We want the camera facing -Z.
    camera.state_.pitch = def.pitch;
    camera.UpdateCameraVectors();

    CameraId id = (unsigned int)cameras_.size();
    cameras_.insert({id, camera});

    return id;
}
gl::TextureId gl::ResourceManager::CreateResource(const gl::Texture::Definition def, unsigned int nrOfFramebufferColorAttachments)
{
    for (const auto& path : def.paths)
    {
        if (path.empty()) EngineError("A path contained in Texture::Definition::paths is empty!");
    }

    // Accumulate all the relevant data in a single string for hashing.
    std::string accumulatedData = "";
    for (const auto& path : def.paths)
    {
        accumulatedData += path;
    }
    accumulatedData += std::to_string(def.flipImage);
    accumulatedData += std::to_string(def.correctGamma);
    accumulatedData += std::to_string((int)def.textureType);
    accumulatedData += std::to_string(def.useHdr);

    const XXH32_hash_t hash = XXH32(accumulatedData.c_str(), sizeof(char) * accumulatedData.size(), HASHING_SEED);
    assert(hash != UINT_MAX); // We aren't handling this issue...

    if (IsTextureIdValid(hash))
    {
        std::cout << "WARNING: attempting to create a new Texture with data identical to an existing one. Returning the id of the existing one instead." << std::endl;
        return hash;
    }
    gl::Texture texture;
    texture.textureType_ = def.textureType;

    stbi_set_flip_vertically_on_load(def.flipImage);
    int width, height, nrChannels;
    unsigned char* data = nullptr;

    glGenTextures(1, &texture.TEX_);
    CheckGlError();
    glBindTexture((texture.textureType_ == Texture::Type::CUBEMAP) ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, texture.TEX_);

    if (texture.textureType_ != Texture::Type::CUBEMAP &&
        texture.textureType_ != Texture::Type::FRAMEBUFFER)
    {
        if (def.paths.size() < 1) EngineError("No texture paths specified for a non Framebuffer texture!");
        
        data = stbi_load(def.paths[0].data(), &width, &height, &nrChannels, 0);
        assert(data);

        if (nrChannels == 1 || nrChannels == 2)
        {
            EngineError("We're not handling monochrome nor bichrome textures!");
        }
        else if (nrChannels == 4)
        {
            EngineError("We're not handling alpha via RGBA! Make a separate alpha map if you want to use transparency and ensure all textures are in RGB!");
        }
        else
        {
            EngineError("Unexpected number of channels in image being loaded!");
        }

        glTexImage2D(GL_TEXTURE_2D, 0, def.correctGamma ? GL_SRGB8 : GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
        CheckGlError();
        stbi_image_free(data);
    }
    else if (texture.textureType_ == Texture::Type::FRAMEBUFFER)
    {
        // NOTE: texture used is of same size as our screen.
        if (nrOfFramebufferColorAttachments < 1 || nrOfFramebufferColorAttachments > 6) EngineError("Unexpected number of framebuffer color attachments!");
        for (unsigned int i = 0; i < nrOfFramebufferColorAttachments; i++) // 6 color attachment max.
        {
            if (def.useHdr)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, (GLsizei)SCREEN_RESOLUTION[0], (GLsizei)SCREEN_RESOLUTION[1], 0, GL_RGB, GL_FLOAT, nullptr);
            }
            else
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)SCREEN_RESOLUTION[0], (GLsizei)SCREEN_RESOLUTION[1], 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            }
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture.TEX_, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            CheckGlError();
        }

    }
    else if (texture.textureType_ == Texture::Type::CUBEMAP)
    {
        if (def.paths.size() != 6) EngineError("Unexpected number of textures for a cubemap!");
        
        for (unsigned int i = 0; i < 6; i++)
        {
            data = stbi_load(def.paths[i].data(), &width, &height, &nrChannels, 0);
            assert(data);

            if (nrChannels == 1 || nrChannels == 2)
            {
                EngineError("We're not handling monochrome nor bichrome textures!");
            }
            else if (nrChannels == 4)
            {
                EngineError("The code is not designed for transparent skyboxes! Ensure the skybox textures are in RGB!");
            }
            else
            {
                EngineError("Unexpected number of channels in image being loaded!");
            }

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, def.correctGamma ? GL_SRGB : GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            CheckGlError();
            stbi_image_free(data);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        CheckGlError();
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    CheckGlError();

    textures_.insert({ hash, texture });

    return hash;
}
gl::SkyboxId gl::ResourceManager::CreateResource(const gl::Skybox::Definition def)
{
    // Accumulate all the relevant data in a single string for hashing.
    std::string accumulatedData = "";
    if (def.paths.size() != 6) EngineError("Skybox::Definition::paths does not contain 6 texture paths!");
    accumulatedData += def.paths[0];
    accumulatedData += def.paths[1];
    accumulatedData += def.paths[2];
    accumulatedData += def.paths[3];
    accumulatedData += def.paths[4];
    accumulatedData += def.paths[5];
    accumulatedData += def.shader[0];
    accumulatedData += def.shader[1];
    accumulatedData += std::to_string(def.correctGamma);
    accumulatedData += std::to_string(def.flipImages);

    const XXH32_hash_t hash = XXH32(accumulatedData.c_str(), sizeof(char) * accumulatedData.size(), HASHING_SEED);
    assert(hash != UINT_MAX); // We aren't handling this issue...

    if (IsSkyboxValid(hash))
    {
        std::cout << "WARNING: attempting to create a new Skybox with data identical to an existing one. Returning the id of the existing one instead." << std::endl;
        return hash;
    }

    VertexBuffer::Definition vbdef;
    VertexBufferId vbId = DEFAULT_ID;
    Material::Definition matdef;
    MaterialId matId = DEFAULT_ID;
    Mesh::Definition mdef;
    MeshId meshId = DEFAULT_ID;

    vbdef.data =
    {
        // Vertex positions AND 3d tex coords simulataneously since it's a cube centered on origin.
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
    vbdef.dataLayout =
    {
        3 // Pos / texcoords
    };
    vbId = CreateResource(vbdef);
    
    matdef.vertexPath = def.paths[0];
    matdef.fragmentPath = def.paths[1];
    for (const auto& path : def.paths)
    {
        matdef.texturePathsAndTypes.push_back({path, Texture::Type::CUBEMAP});
    }
    matdef.correctGamma = def.correctGamma;
    matdef.flipImages = def.flipImages;
    matdef.useHdr = false; // No sense in using hdr for a skybox texture.
    matdef.staticInts.insert({CUBEMAP_SAMPLER_NAME.data(), CUBEMAP_TEXTURE_UNIT});
    matdef.staticMat4s.insert({PROJECTION_MARIX_NAME, PERSPECTIVE});
    matdef.dynamicMat4s.insert({VIEW_MARIX_NAME, cameras_[0].GetViewMatrix()}); // NOTE: this means a skybox can only be related to the default camera!
    matId = CreateResource(matdef);
    
    mdef.vertexBuffer = vbId;
    mdef.material = matId;
    meshId = CreateResource(mdef);

    Skybox skybox;
    skybox.mesh_ = meshId;

    skyboxes_.insert({hash, skybox});

    return hash;
}
gl::VertexBufferId gl::ResourceManager::CreateResource(const gl::VertexBuffer::Definition def)
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

    const size_t stride = std::accumulate(def.dataLayout.begin(), def.dataLayout.end(), 0);
    buffer.verticesCount_ = (int)(def.data.size() / stride);

    // Generate vao/vbo and transfer data to it.
    glGenVertexArrays(1, &buffer.VAO_);
    CheckGlError();
    glBindVertexArray(buffer.VAO_);
    glGenBuffers(1, &buffer.VBO_);
    CheckGlError();
    glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO_);
    glBufferData(GL_ARRAY_BUFFER, def.data.size() * sizeof(float), def.data.data(), GL_STATIC_DRAW);
    CheckGlError();

    // Enable the vertex attribute pointers.
    size_t accumulatedOffset = 0;
    for (size_t i = 0; i < def.dataLayout.size(); i++)
    {
        glVertexAttribPointer((GLuint)i, def.dataLayout[i], GL_FLOAT, GL_FALSE, (GLsizei)(stride * sizeof(float)), (void*)accumulatedOffset);
        accumulatedOffset += def.dataLayout[i] * sizeof(float);
        glEnableVertexAttribArray((GLuint)i);
        CheckGlError();
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    CheckGlError();

    vertexBuffers_.insert({ hash, buffer });
    return hash;
}

std::vector<gl::ResourceManager::ObjData> gl::ResourceManager::ReadObjData(std::string_view path) const
{
    std::vector<ObjData> returnVal = {};

    // TODO: currently shaders don't handle normal mapping.
    // TODO: place transparent meshes last! Right now it's not rendering properly...

    const std::string dir = std::string(path.begin(), path.begin() + path.find_last_of("/") + 1);

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

        returnVal[i].material.ambientMap = materials[matId].ambient_texname;
        returnVal[i].material.alphaMap = materials[matId].alpha_texname;
        returnVal[i].material.diffuseMap = materials[matId].diffuse_texname;
        returnVal[i].material.specularMap = materials[matId].specular_texname;
        returnVal[i].material.normalMap = materials[matId].bump_texname;
        returnVal[i].material.roughnessMap = materials[matId].roughness_texname;
        returnVal[i].material.metallicMap = materials[matId].metallic_texname;
        returnVal[i].material.sheenMap = materials[matId].sheen_texname;
        returnVal[i].material.emissiveMap = materials[matId].emissive_texname;
        returnVal[i].material.shininess = materials[matId].shininess;
        returnVal[i].material.ior = materials[matId].ior;

        if (shapes[i].mesh.indices[0].vertex_index < 0 || shapes[i].mesh.indices[0].texcoord_index < 0) EngineError("Obj doesn't contain either position data or texcoord data or both!");

        const size_t numOfFaces = shapes[i].mesh.num_face_vertices.size();
        for (size_t f = 0; f < (numOfFaces * 3) - 2; f++)
        {
            const tinyobj::index_t idx0 = shapes[i].mesh.indices[f + 0];
            const tinyobj::index_t idx1 = shapes[i].mesh.indices[f + 1];
            const tinyobj::index_t idx2 = shapes[i].mesh.indices[f + 2];

            // Retrieve pos.
            const glm::vec3 pos0 =
            {
                attrib.vertices[3 * (size_t)idx0.vertex_index + 0],
                attrib.vertices[3 * (size_t)idx0.vertex_index + 1],
                attrib.vertices[3 * (size_t)idx0.vertex_index + 2]
            };
            const glm::vec3 pos1 =
            {
                attrib.vertices[3 * (size_t)idx1.vertex_index + 0],
                attrib.vertices[3 * (size_t)idx1.vertex_index + 1],
                attrib.vertices[3 * (size_t)idx1.vertex_index + 2]
            };
            const glm::vec3 pos2 =
            {
                attrib.vertices[3 * (size_t)idx2.vertex_index + 0],
                attrib.vertices[3 * (size_t)idx2.vertex_index + 1],
                attrib.vertices[3 * (size_t)idx2.vertex_index + 2]
            };
            const glm::vec3 deltaPos0 = pos1 - pos0;
            const glm::vec3 deltaPos1 = pos2 - pos1;

            // Retireve uvs.
            const glm::vec2 uv0 =
            {
                attrib.texcoords[2 * (size_t)idx0.texcoord_index + 0],
                attrib.texcoords[2 * (size_t)idx0.texcoord_index + 1]
            };
            const glm::vec2 uv1 =
            {
                attrib.texcoords[2 * (size_t)idx1.texcoord_index + 0],
                attrib.texcoords[2 * (size_t)idx1.texcoord_index + 1]
            };
            const glm::vec2 uv2 =
            {
                attrib.texcoords[2 * (size_t)idx2.texcoord_index + 0],
                attrib.texcoords[2 * (size_t)idx2.texcoord_index + 1]
            };
            const glm::vec2 deltaUv0 = uv1 - uv0;
            const glm::vec2 deltaUv1 = uv2 - uv1;

            // Compute normal.
            const glm::vec3 normal = glm::normalize(glm::cross(deltaPos0, deltaPos1));
            // Compute tangents.
            const float F = 1.0f / (deltaUv0.x * deltaUv1.y - deltaUv1.x * deltaUv0.y);
            const glm::vec3 tangent = // Here be dragons.
            {
                F * (deltaUv1.y * deltaPos0.x - deltaUv0.y * deltaPos1.x),
                F * (deltaUv1.y * deltaPos0.y - deltaUv0.y * deltaPos1.y),
                F * (deltaUv1.y * deltaPos0.z - deltaUv0.y * deltaPos1.z)
            };

            // Add the data to the ObjData struct.
            returnVal[i].positions.push_back(pos0);
            returnVal[i].positions.push_back(pos1);
            returnVal[i].positions.push_back(pos2);
            returnVal[i].texCoords.push_back(uv0);
            returnVal[i].texCoords.push_back(uv1);
            returnVal[i].texCoords.push_back(uv2);
            returnVal[i].normals.push_back(normal);
            returnVal[i].normals.push_back(normal);
            returnVal[i].normals.push_back(normal);
            returnVal[i].tangents.push_back(tangent);
            returnVal[i].tangents.push_back(tangent);
            returnVal[i].tangents.push_back(tangent);
        }
    }
    return returnVal;
}

bool gl::ResourceManager::IsModelIdValid(ModelId id) const
{
    if (id == UINT_MAX) return false;
    return (models_.find(id) != models_.end()) ? true : false;
}

bool gl::ResourceManager::IsMeshIdValid(MeshId id) const
{
    if (id == UINT_MAX) return false;
    return (opaqueMeshes_.find(id) != opaqueMeshes_.end() || transparentMeshes_.find(id) != transparentMeshes_.end()) ? true : false;
}

bool gl::ResourceManager::IsMaterialIdValid(MaterialId id) const
{
    if (id == UINT_MAX) return false;
    return (materials_.find(id) != materials_.end()) ? true : false;
}

bool gl::ResourceManager::IsTransformIdValid(TransformId id) const
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
    for (const auto& pair : materials_)
    {
        glDeleteShader(pair.second.PROGRAM_);
    }
    for (const auto& pair : framebuffers_)
    {
        glDeleteRenderbuffers(1, &pair.second.RBO_);
        glDeleteFramebuffers(1, &pair.second.FBO_);
    }
    for (const auto& pair : models_)
    {
        glDeleteBuffers(1, &pair.second.modelMatricesVBO_);
    }
}

// TODO: make this shit more sensible: one obj should not produce multiple ObjDatas....
gl::ModelId gl::ResourceManager::CreateResource(const std::vector<ObjData> objData, bool flipImages, bool correctGamma)
{
    Model::Definition modef;
    for (size_t m = 0; m < objData.size(); m++)
    {
        if (!(objData[m].positions.size() == objData[m].texCoords.size() && objData[m].texCoords.size() == objData[m].normals.size() && objData[m].normals.size() == objData[m].tangents.size())) EngineError("Inconsistent number of vertex components!");
        if (objData[m].positions.size() < 3 || objData[m].positions.size() % 3 != 0) EngineError("Unexpected number of vertices!"); // % 3 since we're working with triangles.

        // Load vertex data.
        VertexBuffer::Definition vbdef;
        Material::Definition matdef;
        Mesh::Definition mdef;

        // Collapse all the arrays of vertex data into a single array.
        const size_t sizeOfVerticesData = objData[m].positions.size() * sizeof(glm::vec3) + objData[m].texCoords.size() * sizeof(glm::vec2) + objData[m].normals.size() * sizeof(glm::vec3) + objData[m].tangents.size() * sizeof(glm::vec3);
        const size_t stride = (objData[m].positions.size() + objData[m].texCoords.size() + objData[m].normals.size() + objData[m].tangents.size()) / sizeOfVerticesData; // TODO: nope
        std::vector<float> verticesData = std::vector<float>(sizeOfVerticesData);
        for (size_t startOfVertex = 0; startOfVertex < sizeOfVerticesData; startOfVertex += stride)
        {
            // TODO: u sure about this?
            // Positions.
            verticesData[startOfVertex + 0] = objData[m].positions[startOfVertex / stride].x;
            verticesData[startOfVertex + 1] = objData[m].positions[startOfVertex / stride].y;
            verticesData[startOfVertex + 2] = objData[m].positions[startOfVertex / stride].z;
            // Texcoords.
            verticesData[startOfVertex + 3] = objData[m].texCoords[startOfVertex / stride].x;
            verticesData[startOfVertex + 4] = objData[m].texCoords[startOfVertex / stride].y;
            // Normals.
            verticesData[startOfVertex + 5] = objData[m].normals[startOfVertex / stride].x;
            verticesData[startOfVertex + 6] = objData[m].normals[startOfVertex / stride].y;
            verticesData[startOfVertex + 7] = objData[m].normals[startOfVertex / stride].z;
            // Tangents.
            verticesData[startOfVertex + 8] = objData[m].tangents[startOfVertex / stride].x;
            verticesData[startOfVertex + 9] = objData[m].tangents[startOfVertex / stride].y;
            verticesData[startOfVertex + 10] = objData[m].tangents[startOfVertex / stride].z;
        }
        vbdef.data = verticesData;
        vbdef.dataLayout =
        {
            3, // pos
            2, // uv
            3, // normals
            3  // tangents
        };

        matdef.correctGamma = correctGamma;
        matdef.flipImages = flipImages;
        matdef.useHdr = false;
        matdef.vertexPath = ILLUM2_SHADER[0]; // TODO: fix this shit. We need to be able to specify our own shader
        matdef.fragmentPath = ILLUM2_SHADER[1];
        if (!objData[m].material.ambientMap.empty())
        {
            matdef.texturePathsAndTypes.push_back({ objData[m].material.ambientMap, Texture::Type::AMBIENT });
            matdef.staticInts.insert({ AMBIENT_SAMPLER_NAME, AMBIENT_TEXTURE_UNIT });
        }
        if (!objData[m].material.alphaMap.empty())
        {
            matdef.texturePathsAndTypes.push_back({ objData[m].material.alphaMap, Texture::Type::ALPHA });
            matdef.staticInts.insert({ ALPHA_SAMPLER_NAME, ALPHA_TEXTURE_UNIT });
        }
        if (!objData[m].material.diffuseMap.empty())
        {
            matdef.texturePathsAndTypes.push_back({ objData[m].material.diffuseMap, Texture::Type::DIFFUSE });
            matdef.staticInts.insert({ DIFFUSE_SAMPLER_NAME, DIFFUSE_TEXTURE_UNIT });
        }
        if (!objData[m].material.specularMap.empty())
        {
            matdef.texturePathsAndTypes.push_back({ objData[m].material.specularMap, Texture::Type::SPECULAR });
            matdef.staticInts.insert({ SPECULAR_SAMPLER_NAME, SPECULAR_TEXTURE_UNIT });
        }
        if (!objData[m].material.normalMap.empty())
        {
            matdef.texturePathsAndTypes.push_back({ objData[m].material.normalMap, Texture::Type::NORMALMAP });
            matdef.staticInts.insert({ NORMALMAP_SAMPLER_NAME, NORMALMAP_TEXTURE_UNIT });
        }
        if (!objData[m].material.roughnessMap.empty())
        {
            matdef.texturePathsAndTypes.push_back({ objData[m].material.roughnessMap, Texture::Type::ROUGHNESS });
            matdef.staticInts.insert({ ROUGHNESS_SAMPLER_NAME, ROUGHNESS_TEXTURE_UNIT });
        }
        if (!objData[m].material.metallicMap.empty())
        {
            matdef.texturePathsAndTypes.push_back({ objData[m].material.metallicMap, Texture::Type::METALLIC });
            matdef.staticInts.insert({ METALLIC_SAMPLER_NAME, METALLIC_TEXTURE_UNIT });
        }
        if (!objData[m].material.sheenMap.empty())
        {
            matdef.texturePathsAndTypes.push_back({ objData[m].material.sheenMap, Texture::Type::SHEEN });
            matdef.staticInts.insert({ SHEEN_SAMPLER_NAME, SHEEN_TEXTURE_UNIT });
        }
        if (!objData[m].material.emissiveMap.empty())
        {
            matdef.texturePathsAndTypes.push_back({ objData[m].material.emissiveMap, Texture::Type::EMISSIVE });
            matdef.staticInts.insert({ EMISSIVE_SAMPLER_NAME, EMISSIVE_TEXTURE_UNIT });
        }
        matdef.staticFloats.insert({ SHININESS_NAME, objData[m].material.shininess });
        matdef.staticFloats.insert({ IOR_NAME, objData[m].material.ior });
        matdef.staticMat4s.insert({ PROJECTION_MARIX_NAME, PERSPECTIVE });
        matdef.dynamicMat4s.insert({ VIEW_MARIX_NAME, cameras_[0].GetViewMatrix() }); // Note: this makes all shaders dependant on the main camera!
        matdef.dynamicVec3s.insert({ VIEW_POSITION_NAME, cameras_[0].GetPosition() });

        mdef.vertexBuffer = CreateResource(vbdef);
        mdef.material = CreateResource(matdef);
        modef.meshes.push_back(CreateResource(mdef));
    }
    modef.modelMatrices = {IDENTITY_MAT4};

    return CreateResource(modef);
}