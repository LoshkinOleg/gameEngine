// Checklist: handle UINT_MAX where necessary, use textureType where appropriate, make a separate cubemap class?, check existing code to make sure we didn't miss anything, make sure there CheckGlError() at key points
// make sure we're using "const auto match" everywhere, not a & version, make sure we're not fucking up where we're passing & in arguments, replace all generic errors with descriptive ones: throw() assert()
// make sure we unbind everything once we're done using it, see where the code could fail for every function
// on creation of a resource, check the validity of ids, check on draw that the asset is valid
// move common strings of shader variables to #defines
// Refactor shaders.

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
        glEnable(GL_FRAMEBUFFER_SRGB_EXT);

        camera_.Translate(glm::vec3(0.0f, 0.0f, 10.0f)); // Move camera away from origin.

        const auto meshes = resourceManager_.LoadObj("../data/models/crate/crate.obj");
        const MaterialId materialId = resourceManager_.GetMesh(meshes[0]).GetMaterialId();

        ShaderId shaderId = DEFAULT_ID;
        {
            ResourceManager::ShaderDefinition def;
            def.vertexPath = "../data/shaders/demo/dirLight.vert";
            def.fragmentPath = "../data/shaders/demo/dirLight.frag";
            def.onInit = [materialId](Shader& shader, const Model& model)->void
            {
                shader.SetMaterial(materialId);
                shader.SetProjectionMatrix(PERSPECTIVE);
                shader.SetVec3("dirLight.dir", glm::normalize(-ONE_VEC3));
                shader.SetVec3("dirLight.ambientColor", ONE_VEC3);
                shader.SetVec3("dirLight.diffuseColor", ONE_VEC3);
                shader.SetVec3("dirLight.specularColor", ONE_VEC3);
            };
            def.onDraw = [](Shader& shader, const Model& model, const Camera& camera)->void
            {
                shader.SetViewMatrix(camera.GetViewMatrix());
                shader.SetVec3("viewPos", camera.GetPosition());
                shader.SetModelMatrix(model.GetModelMatrix());
            };
            shaderId = resourceManager_.CreateResource(def);
        }

        ModelId modelId = DEFAULT_ID;
        {
            ResourceManager::ModelDefinition def;
            def.meshes = meshes;
            def.shader = shaderId;
            def.transform = resourceManager_.CreateResource(ResourceManager::Transform3dDefinition{});
            modelId = resourceManager_.CreateResource(def);
        }

        model_ = resourceManager_.GetModel(modelId);
    }
    void Update(seconds dt) override
    {
        timer_ += dt.count();
        glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        model_.Draw(camera_);
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