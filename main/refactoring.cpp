#include <vector>
#include <utility>
#include <map>
#include <limits>
#include <functional>
#include <iostream>
#include <array>

#include <glad/glad.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "xxhash.h"

#include "engine.h"
#include "camera.h"

// for all shaders: location = 0 for pos, location = 1 for normals, location = 2 for texCoords
#define IDENTITY_MAT4 glm::mat4(1.0f)
#define ZERO_VEC3 glm::vec3(0.0f)
#define ONE_VEC3 glm::vec3(1.0f)
#define HASHING_SEED 0x1337

using VertexBufferId = unsigned int;
using TextureId = unsigned int;
using ShaderId = unsigned int;
using MaterialId = unsigned int;
using MeshId = unsigned int;
using ModelId = unsigned int;
using Transform3dId = unsigned int;

namespace gl {

void CheckGlError(const char* file, int line)
{
    auto error_code = glGetError();
    if (error_code != GL_NO_ERROR)
    {
        std::cerr
            << error_code
            << " in file: " << file
            << " at line: " << line
            << "\n";
    }
}

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

class VertexBuffer
{
public:
private:
    friend class ResourceManager;

    VertexBufferId id_ = UINT_MAX;
    unsigned int VAO_ = 0, VBO_ = 0;
};

struct TextureDefinition
{
    GLenum textureType = GL_TEXTURE_2D;
    std::string path = "";
    std::vector<std::pair<std::string, int>> samplerTextureUnitPairs = {};
};

class Texture
{
public:
    const std::vector<std::pair<std::string, int>>& GetSamplerTextureUnitPairs() const
    {
        return samplerTextureUnitPairs_;
    }
private:
    friend class ResourceManager;

    TextureId id_ = UINT_MAX;
    unsigned int TEX_ = 0;
    GLenum textureType_ = GL_TEXTURE_2D;
    std::vector<std::pair<std::string, int>> samplerTextureUnitPairs_ = {}; // Can use the same texture for multiple samplers. Ex: ambient and diffuse
};

struct MaterialDefinition
{
    TextureId ambientMap = UINT_MAX;
    TextureId diffuseMap = UINT_MAX;
    TextureId specularMap = UINT_MAX;
    TextureId normalMap = UINT_MAX;
    glm::vec3 ambientColor = ONE_VEC3;
    glm::vec3 diffuseColor = ONE_VEC3;
    glm::vec3 specularColor = ONE_VEC3;
    float shininess = 1.0f;
};

class Material
{
public:
    std::array<TextureId, 4> GetTextures() const
    {
        return std::array<TextureId, 4>{ambientMap_, diffuseMap_, specularMap_, normalMap_};
    }
    const std::array<glm::vec3, 3>& GetColors() const
    {
        return std::array<glm::vec3, 3>{ambientColor_, diffuseColor_, specularColor_};
    }
    float GetShininess() const
    {
        return shininess_;
    }
private:
    friend class ResourceManager;

    MaterialId id_ = UINT_MAX;
    TextureId ambientMap_ = UINT_MAX;
    TextureId diffuseMap_ = UINT_MAX;
    TextureId specularMap_ = UINT_MAX;
    TextureId normalMap_ = UINT_MAX;
    glm::vec3 ambientColor_ = ONE_VEC3;
    glm::vec3 diffuseColor_ = ONE_VEC3;
    glm::vec3 specularColor_ = ONE_VEC3;
    float shininess_ = 1.0f;
};

struct MeshDefinition
{
    VertexBufferId vertexBuffer = UINT_MAX;
    MaterialId material = UINT_MAX;
};

class Mesh
{
public:
private:
    friend class ResourceManager;

    MeshId id_ = UINT_MAX;
    VertexBufferId vertexBuffer_ = UINT_MAX;
    MaterialId material_ = UINT_MAX;
};

struct Transform3dDefinition
{
    glm::vec3 position = ZERO_VEC3;
    glm::vec3 cardinalsRotation = ZERO_VEC3;
    glm::vec3 scale = ONE_VEC3;
};

class Transform3d
{
public:
    glm::mat4 GetModelMatrix() const
    {
        return model_;
    }
private:
    friend class ResourceManager;

    Transform3dId id_ = UINT_MAX;
    glm::mat4 model_ = IDENTITY_MAT4;
    glm::vec3 position_ = ZERO_VEC3;
    glm::vec3 cardinalsRotation_ = ZERO_VEC3;
    glm::vec3 scale_ = ONE_VEC3;
    glm::quat quaternion_ = {};
};

struct ModelDefinition
{
    std::vector<MeshId> meshes = {};
    ShaderId shader = UINT_MAX;
    Transform3dId transform = UINT_MAX;
};

class Model
{
public:
    void Draw() const
    {
        ResourceManager resourceManager = ResourceManager::Get();
        const Shader& shader = resourceManager.GetShader(shader_);
        const std::vector<const Mesh&> meshes = resourceManager.GetMeshes(meshes_);

        for (const auto& mesh : meshes)
        {
            mesh.Draw(shader);
        }
    }
    void UseNewShader(ShaderId id);
    glm::mat4 GetModelMatrix() const
    {
        return ResourceManager::Get().GetTransform(transform_).GetModelMatrix();
    }
private:
    friend class ResourceManager;

    ModelId id_ = UINT_MAX;
    std::vector<MeshId> meshes_ = {};
    ShaderId shader_ = UINT_MAX;
    Transform3dId transform_ = UINT_MAX;
};

class Shader;
struct ShaderDefinition
{
    std::string vertexPath = "";
    std::string fragmentPath = "";
    std::function<void(Shader&, const Model&, const Camera&)> onInit = nullptr;
    std::function<void(Shader&, const Model&, const Camera&)> onDraw = nullptr;
};

class Shader
{
public:
    void SetMaterial(MaterialId materialId)
    {
        const Material& material = ResourceManager::Get().GetMaterial(materialId);

        const std::array<TextureId, 4> textureIds = material.GetTextures();
        for (const TextureId id : textureIds)
        {
            if (id != UINT_MAX) // Don't try setting textures that aren't used.
            {
                SetTexture(id);
            }
        }

        const auto& colors = material.GetColors();
        SetVec3("material.ambientColor", colors[0]); // TODO: make defines for default shader names and use them here
        SetVec3("material.diffuseColor", colors[1]);
        SetVec3("material.specularColor", colors[2]);

        const float shininess = material.GetShininess();
        SetFloat("material.shininess", shininess);
    }
    void SetFloat(const std::string& name, const float value)
    {
        const auto match = uniformNames_.find(name);
        if (match != uniformNames_.end()) // Name of uniform already known, use it.
        {
            glUniform1f(match->second, value);
        }
        else
        {
            const int uniformIntName = glGetUniformLocation(PROGRAM_, name.c_str());
            uniformNames_.insert({ name.c_str(), uniformIntName }); // Add the new entry.
            glUniform1f(uniformIntName, value);
        }
        CheckGlError(__FILE__, __LINE__);
    }
    void SetVec3(const std::string& name, const glm::vec3& value)
    {
        const auto match = uniformNames_.find(name);
        if (match != uniformNames_.end()) // Name of uniform already known, use it.
        {
            glUniform3fv(match->second, 1, &value[0]);
        }
        else
        {
            const int uniformIntName = glGetUniformLocation(PROGRAM_, name.c_str());
            uniformNames_.insert({ name.c_str(), uniformIntName }); // Add the new entry.
            glUniform3fv(uniformIntName, 1, &value[0]);
        }
        CheckGlError(__FILE__, __LINE__);
    }
    void SetInt(const std::string& name, const int value)
    {
        const auto match = uniformNames_.find(name);
        if (match != uniformNames_.end()) // Name of uniform already known, use it.
        {
            glUniform1i(match->second, value);
        }
        else
        {
            const int uniformIntName = glGetUniformLocation(PROGRAM_, name.c_str());
            uniformNames_.insert({ name.c_str(), uniformIntName }); // Add the new entry.
            glUniform1i(uniformIntName, value);
        }
        CheckGlError(__FILE__, __LINE__);
    }
    void SetTexture(TextureId textureId)
    {
        const Texture& tex = ResourceManager::Get().GetTexture(textureId);
        const auto& samplerTextureUnitPairs = tex.GetSamplerTextureUnitPairs();

        for (const auto& pair : samplerTextureUnitPairs)
        {
            SetInt(pair.first, pair.second);
        }
    }
    void SetProjectionMatrix(const glm::mat4 mat)
    {
        SetMat4("projection", mat);
    }
    void SetViewMatrix(const glm::mat4 mat)
    {
        SetMat4("view", mat);
    }
    void SetModelMatrix(const glm::mat4 mat)
    {
        SetMat4("model", mat);
    }
    void SetMat4(const std::string& name, const glm::mat4 mat)
    {
        const auto match = uniformNames_.find(name);
        if (match != uniformNames_.end()) // Name of the uniform already retireved, use it, don't bother the gpu.
        {
            glUniformMatrix4fv(
                match->second,
                1,
                GL_FALSE,
                &mat[0][0]);
        }
        else
        {
            const int uniformIntName = glGetUniformLocation(PROGRAM_, name.c_str());
            uniformNames_.insert({ name, uniformIntName });

            glUniformMatrix4fv(
                uniformIntName,
                1,
                GL_FALSE,
                &mat[0][0]);
        }
        CheckGlError(__FILE__, __LINE__);
    }
private:
    friend class ResourceManager;

    ShaderId id_ = UINT_MAX;
    unsigned int PROGRAM_ = 0;
    std::function<void(Shader&, const Model&, const Camera&)> onInit_ = nullptr;
    std::function<void(Shader&, const Model&, const Camera&)> onDraw_ = nullptr;
    std::map<std::string, int> uniformNames_ = {};
};

struct ObjResult
{
    // vect of pairs of (vertexbuffer ids ; material ids)
    std::vector<std::pair<VertexBufferId, MaterialId>> objects;
};

class ResourceManager
{
    // func: allocateResource()->resource& for initialization.
    // func submitResource(resource&)->ID to check for completeness. Once passed, it is usable. You're only working with IDs from now on.
    // func: draw() to go through all resources automatically, no user input needed.

public:
    static ResourceManager& Get()
    {
        static ResourceManager instance;
        return instance;
    }
    ResourceManager() = default;
    const Transform3d& GetTransform(Transform3dId id) const
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
    const Texture& GetTexture(TextureId id) const
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
    const Material& GetMaterial(MaterialId id) const
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
    Model& GetModel(ModelId id)
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
    const Shader& GetShader(ShaderId id) const
    {
        const auto match = shaders_.find(id);
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
    std::vector<const Mesh&> GetMeshes(const std::vector<MeshId>& ids) const
    {
        std::vector<const Mesh&> returnVal;
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
    ModelId CreateResource(const ModelDefinition def)
    {
        return UINT_MAX;
    }
    MeshId CreateResource(const MeshDefinition def)
    {
        return UINT_MAX;
    }
    ShaderId CreateResource(const ShaderDefinition def)
    {
        return UINT_MAX;
    }
    MaterialId CreateResource(const MaterialDefinition def)
    {
        return UINT_MAX;
    }
    Transform3dId CreateResource(const Transform3dDefinition def)
    {
        return UINT_MAX;
    }
    TextureId CreateResource(const TextureDefinition def)
    {
        return UINT_MAX;
    }
    VertexBufferId CreateResource(const VertexBufferDefinition def)
    {
        // Hash the data of the buffer and check if it's not loaded already.
        const XXH32_hash_t hash = XXH32(def.data.data(), sizeof(float) * def.data.size(), HASHING_SEED);
        assert(hash != UINT_MAX); // This value is reserved for invalid ids.
        const auto match = vertexBuffers_.find(hash);
        if (match != vertexBuffers_.end())
        {
            std::cout << "WARNING: attempting to create a new VertexBuffer with data identical to an existing one. Returning the id of the existing one instead." << std::endl;
            return hash; // Iterator does not point to end of the map, meaning it's pointing to the element that's storing the same data as the one we're trying to initialize.
        }
        // Iterator points to end of the map, meaning no matches were found for this hash, proceed with transfering the data to gpu.
        VertexBuffer buffer;
        buffer.id_ = hash;

        size_t stride = 0; // Compute stride.
        for (size_t i = 0; i < def.dataLayout.size(); i++)
        {
            switch (def.dataLayout[i])
            {
            case gl::VertexDataTypes::POSITIONS_3D: stride += 3;
                break;
            case gl::VertexDataTypes::POSITIONS_2D: stride += 2;
                break;
            case gl::VertexDataTypes::TEXCOORDS_2D: stride += 2;
                break;
            case gl::VertexDataTypes::TEXCOORDS_3D: stride += 3;
                break;
            case gl::VertexDataTypes::NORMALS: stride += 3;
                break;
            default:
                throw;
            }
        }

        // Generate vao/vbo and transfer data to it.
        glGenVertexArrays(1, &buffer.VAO_);
        glBindVertexArray(buffer.VAO_);
        glGenBuffers(1, &buffer.VBO_);
        glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO_);
        glBufferData(GL_ARRAY_BUFFER, def.data.size() * sizeof(float), def.data.data(), GL_STATIC_DRAW);

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
            else if(def.dataLayout[i] == VertexDataTypes::POSITIONS_2D ||
                    def.dataLayout[i] == VertexDataTypes::TEXCOORDS_2D)
            {
                glVertexAttribPointer(i, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), +(void*)accumulatedOffset);
                accumulatedOffset += 2 * sizeof(float);
            }
            else
            {
                throw;
            }
            glEnableVertexAttribArray(i);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        vertexBuffers_.insert({hash, buffer});
        return hash;
    }
private:
    std::map<ModelId, Model> models_ = {};
    std::map<MeshId, Mesh> meshes_ = {};
    std::map<ShaderId, Shader> shaders_ = {};
    std::map<MaterialId, Material> materials_ = {};
    std::map<Transform3dId, Transform3d> transforms_ = {};
    std::map<TextureId, Texture> textures_ = {};
    std::map<VertexBufferId, VertexBuffer> vertexBuffers_ = {};
};

class HelloTriangle : public Program
{
public:
    void Init() override
    {
        glEnable(GL_DEPTH_TEST);

        camera_.Translate(glm::vec3(0.0f, 0.0f, 10.0f)); // Move camera away from origin.

        ResourceManager resourceManager = ResourceManager::Get();

        // Make a model that uses vertex normals as source for light computations.
        VertexBufferId vertexBufferId = UINT_MAX;
        {
            VertexBufferDefinition def;
            def.data =
            {   // Positions            // Normals          // TexCoords
                -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
                 1.0f, -1.0f, 0.0f,     0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
                 1.0f,  1.0f, 0.0f,     0.0f, 0.0f, 1.0f,   1.0f, 1.0f,

                -1.0f, -1.0f, 0.0f,     0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
                 1.0f,  1.0f, 0.0f,     0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
                -1.0f,  1.0f, 0.0f,     0.0f, 0.0f, 1.0f,   0.0f, 1.0f
            };
            def.dataLayout = // Mirrors the layout of the data above.
            {
                VertexDataTypes::POSITIONS_3D,
                VertexDataTypes::NORMALS,
                VertexDataTypes::TEXCOORDS_2D
            };
            vertexBufferId = resourceManager.CreateResource(def); // Resource is now loaded in the gpu.
            assert(vertexBufferId != UINT_MAX); // Make sure the id is valid.

            // Make a model that uses the exact same values for it's vertex data to make sure the resource manager doesn't allocate new ones but directs the user to the existing buffer.
            VertexBufferId duplicate = resourceManager.CreateResource(def);
            assert(vertexBufferId == duplicate);
        }

        TextureId ambientAndDiffuseTexId = UINT_MAX;
        {
            TextureDefinition def;
            def.path = "../data/textures/crate_diffuse.png"; // No difference between diffuse and ambient in most cases.
            def.textureType = GL_TEXTURE_2D;
            def.samplerTextureUnitPairs = { std::pair<std::string, int>{"material.ambientMap", 0}, std::pair<std::string, int>{"material.diffuseMap", 1} };

            ambientAndDiffuseTexId = resourceManager.CreateResource(def);
            assert(ambientAndDiffuseTexId != UINT_MAX);
        }
        TextureId specularCrateTexId = UINT_MAX;
        {
            TextureDefinition def;
            def.path = "../data/textures/crate_specular.png";
            def.textureType = GL_TEXTURE_2D;
            def.samplerTextureUnitPairs = { std::pair<std::string, int>{"material.specularMap", 2} };

            specularCrateTexId = resourceManager.CreateResource(def);
            assert(specularCrateTexId != UINT_MAX);
        }

        MaterialId materialId = UINT_MAX;
        {
            MaterialDefinition def;
            def.ambientMap = ambientAndDiffuseTexId;
            def.diffuseMap = ambientAndDiffuseTexId;
            def.specularMap = specularCrateTexId;
            def.normalMap = UINT_MAX;
            def.ambientColor = glm::vec3(0.1f);
            def.diffuseColor = glm::vec3(0.9f);
            def.specularColor = glm::vec3(1.0f);
            def.shininess = 64.0f;

            materialId = resourceManager.CreateResource(def);
            assert(materialId != UINT_MAX);

            MaterialId duplicate = resourceManager.CreateResource(def);
            assert(materialId == duplicate);
        }

        ShaderId shaderId = UINT_MAX;
        {
            ShaderDefinition def;
            def.vertexPath = "../data/shaders/demo/dirLight.vert";
            def.fragmentPath = "../data/shaders/demo/dirLight.frag";
            def.onInit = [materialId](Shader& shader, const Model& model, const Camera& camera)->void
            {
                shader.SetProjectionMatrix(PERSPECTIVE);
                shader.SetMaterial(materialId);
                shader.SetVec3("dirLight.dir", glm::normalize(-ONE_VEC3));
                shader.SetVec3("dirLight.ambientColor", ONE_VEC3 * 0.1f);
                shader.SetVec3("dirLight.diffuseColor", ONE_VEC3 * 0.9f);
                shader.SetVec3("dirLight.specularColor", ONE_VEC3);
            };
            def.onDraw = [](Shader& shader, const Model& model, const Camera& camera)->void
            {
                shader.SetViewMatrix(camera.GetViewMatrix());
                shader.SetModelMatrix(model.GetModelMatrix());
                shader.SetVec3("viewPos", camera.GetPosition());
            };

            shaderId = resourceManager.CreateResource(def);
            assert(shaderId != UINT_MAX);

            ShaderId duplicate = resourceManager.CreateResource(def);
            assert(shaderId == duplicate);
        }

        MeshId meshId = UINT_MAX;
        {
            MeshDefinition def;
            def.vertexBuffer = vertexBufferId;
            def.material = materialId;

            meshId = resourceManager.CreateResource(def);
            assert(meshId != UINT_MAX);

            ShaderId duplicate = resourceManager.CreateResource(def);
            assert(meshId == duplicate);
        }

        Transform3dId transformId = UINT_MAX;
        {
            Transform3dDefinition def;
            def.cardinalsRotation = glm::vec3(glm::radians(25.0f), glm::radians(25.0f), glm::radians(0.0f));
            def.position = glm::vec3(1.5f, 1.5f, -3.0f);
            def.scale = glm::vec3(2.0f, 2.0f, 2.0f);

            transformId = resourceManager.CreateResource(def);
            assert(transformId != UINT_MAX);

            ShaderId duplicate = resourceManager.CreateResource(def);
            assert(transformId == duplicate);
        }

        ModelId modelId = UINT_MAX;
        {
            ModelDefinition def;
            def.meshes = std::vector<MeshId>{meshId};
            def.shader = shaderId;
            def.transform = transformId;

            modelId = resourceManager.CreateResource(def);
            assert(modelId != UINT_MAX);

            ShaderId duplicate = resourceManager.CreateResource(def);
            assert(modelId == duplicate);
        }

        model_ = &resourceManager.GetModel(modelId);
    }
    void Update(seconds dt) override
    {
        timer_ += dt.count();
        glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        model_.Draw();
    }
    void Destroy() override
    {

    }
    void OnEvent(SDL_Event& event) override
    {
        if ((event.type == SDL_KEYDOWN) &&
            (event.key.keysym.sym == SDLK_ESCAPE))
        {
            exit(0);
        }

        switch (event.type)
        {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_w:
                camera_.ProcessKeyboard(Camera::Camera_Movement::FORWARD);
                break;
            case SDLK_s:
                camera_.ProcessKeyboard(Camera::Camera_Movement::BACKWARD);
                break;
            case SDLK_a:
                camera_.ProcessKeyboard(Camera::Camera_Movement::LEFT);
                break;
            case SDLK_d:
                camera_.ProcessKeyboard(Camera::Camera_Movement::RIGHT);
                break;
            case SDLK_SPACE:
                camera_.ProcessKeyboard(Camera::Camera_Movement::UP);
                break;
            case SDLK_LCTRL:
                camera_.ProcessKeyboard(Camera::Camera_Movement::DOWN);
                break;
            default:
                break;
            }
            break;
        case SDL_MOUSEMOTION:
            if (mouseButtonDown_) camera_.ProcessMouseMovement(event.motion.xrel, event.motion.yrel);
            break;
        case SDL_MOUSEBUTTONDOWN:
            mouseButtonDown_ = true;
            break;
        case SDL_MOUSEBUTTONUP:
            mouseButtonDown_ = false;
            break;
        default:
            break;
        }
    }
    void DrawImGui() override
    {

    }

private:
    Camera camera_ = {};
    float timer_ = 0.0f;
    bool mouseButtonDown_ = false;
    Model* model_ = nullptr;
};

} // End namespace gl.

int main(int argc, char** argv)
{
    gl::HelloTriangle program;
    gl::Engine engine(program);
    engine.Run();
    return EXIT_SUCCESS;
}
