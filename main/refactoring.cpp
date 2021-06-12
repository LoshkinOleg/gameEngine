// Checklist: handle UINT_MAX where necessary, use textureType where appropriate, make a separate cubemap class?, check existing code to make sure we didn't miss anything, make sure there CheckGlError() at key points
// make sure we're using "const auto match" everywhere, not a & version, make sure we're not fucking up where we're passing & in arguments, replace all generic errors with descriptive ones: throw() assert()
// make sure we unbind everything once we're done using it, see where the code could fail for every function
// on creation of a resource, check the validity of ids, check on draw that the asset is valid
// move common strings of shader variables to #defines

// std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": Trying to access a non existent Transform3d!" << std::endl;

// for all shaders: location = 0 for pos, location = 1 for normals, location = 2 for texCoords

#include <glad/glad.h>

#include "engine.h"
#include "camera.h"
#include "resource_manager.h"

namespace gl {

class HelloTriangle : public Program
{
public:
    void Init() override
    {
        glEnable(GL_DEPTH_TEST);

        camera_.Translate(glm::vec3(0.0f, 0.0f, 10.0f)); // Move camera away from origin.

        // Make a model that uses vertex normals as source for light computations.
        VertexBufferId vertexBufferId = DEFAULT_ID;
        {
            ResourceManager::VertexBufferDefinition def;
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
                ResourceManager::VertexDataTypes::POSITIONS_3D,
                ResourceManager::VertexDataTypes::NORMALS,
                ResourceManager::VertexDataTypes::TEXCOORDS_2D
            };
            vertexBufferId = resourceManager_.CreateResource(def); // Resource is now loaded in the gpu.
            assert(vertexBufferId != DEFAULT_ID); // Make sure the id is valid.

            // Make a model that uses the exact same values for it's vertex data to make sure the resource manager doesn't allocate new ones but directs the user to the existing buffer.
            VertexBufferId duplicate = resourceManager_.CreateResource(def);
            assert(vertexBufferId == duplicate);
        }

        TextureId ambientAndDiffuseTexId = DEFAULT_ID;
        {
            ResourceManager::TextureDefinition def;
            def.paths = std::vector<std::string>{ "../data/textures/crate_diffuse.png" }; // No difference between diffuse and ambient in most cases.
            def.textureType = GL_TEXTURE_2D;
            def.samplerTextureUnitPairs = { std::pair<std::string, int>{"material.ambientMap", 0}, std::pair<std::string, int>{"material.diffuseMap", 1} };
            def.flipImage = false;
            def.correctGamma = true;

            ambientAndDiffuseTexId = resourceManager_.CreateResource(def);
            assert(ambientAndDiffuseTexId != DEFAULT_ID);

            TextureId duplicate = resourceManager_.CreateResource(def);
            assert(ambientAndDiffuseTexId == duplicate);
        }
        TextureId specularCrateTexId = DEFAULT_ID;
        {
            ResourceManager::TextureDefinition def;
            def.paths = std::vector<std::string>{ "../data/textures/crate_specular.png" };
            def.textureType = GL_TEXTURE_2D;
            def.samplerTextureUnitPairs = { std::pair<std::string, int>{"material.specularMap", 2} };
            def.flipImage = false;
            def.correctGamma = false; // speculars are in linear space already.

            specularCrateTexId = resourceManager_.CreateResource(def);
            assert(specularCrateTexId != DEFAULT_ID);
        }

        MaterialId materialId = DEFAULT_ID;
        {
            ResourceManager::MaterialDefinition def;
            def.ambientMap = ambientAndDiffuseTexId;
            def.diffuseMap = ambientAndDiffuseTexId;
            def.specularMap = specularCrateTexId;
            def.normalMap = DEFAULT_ID;
            def.ambientColor = glm::vec3(0.1f);
            def.diffuseColor = glm::vec3(0.9f);
            def.specularColor = glm::vec3(1.0f);
            def.shininess = 64.0f;

            materialId = resourceManager_.CreateResource(def);
            assert(materialId != DEFAULT_ID);

            MaterialId duplicate = resourceManager_.CreateResource(def);
            assert(materialId == duplicate);
        }

        ShaderId shaderId = DEFAULT_ID;
        {
            ResourceManager::ShaderDefinition def;
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

            shaderId = resourceManager_.CreateResource(def);
            assert(shaderId != DEFAULT_ID);

            ShaderId duplicate = resourceManager_.CreateResource(def);
            assert(shaderId == duplicate);
        }

        MeshId meshId = DEFAULT_ID;
        {
            ResourceManager::MeshDefinition def;
            def.vertexBuffer = vertexBufferId;
            def.material = materialId;

            meshId = resourceManager_.CreateResource(def);
            assert(meshId != DEFAULT_ID);

            MeshId duplicate = resourceManager_.CreateResource(def);
            assert(meshId == duplicate);
        }

        Transform3dId transformId = DEFAULT_ID;
        {
            ResourceManager::Transform3dDefinition def;
            def.cardinalsRotation = glm::vec3(glm::radians(25.0f), glm::radians(25.0f), glm::radians(0.0f));
            def.position = glm::vec3(1.5f, 1.5f, -3.0f);
            def.scale = glm::vec3(2.0f, 2.0f, 2.0f);

            transformId = resourceManager_.CreateResource(def);
            assert(transformId != DEFAULT_ID);

            Transform3dId duplicate = resourceManager_.CreateResource(def);
            assert(transformId != duplicate); // For transforms, we want to have different transforms even if they have the same data.
        }

        ModelId modelId = UINT_MAX;
        {
            ResourceManager::ModelDefinition def;
            def.meshes = std::vector<MeshId>{meshId};
            def.shader = shaderId;
            def.transform = transformId;

            modelId = resourceManager_.CreateResource(def);
            assert(modelId != DEFAULT_ID);

            ModelId duplicate = resourceManager_.CreateResource(def);
            assert(modelId == duplicate);
        }

        model_ = resourceManager_.GetModel(modelId); // TODO: this get deallocated, duh
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
        ResourceManager::Get().Shutdown();
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
    Model model_;
    ResourceManager& resourceManager_ = ResourceManager::Get();
};

} // End namespace gl.

int main(int argc, char** argv)
{
    gl::HelloTriangle program;
    gl::Engine engine(program);
    engine.Run();
    return EXIT_SUCCESS;
}
