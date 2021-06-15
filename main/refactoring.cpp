// Checklist: use textureType where appropriate, make a separate cubemap class?, make sure there CheckGlError() at key points
// make sure we're using "const auto match" everywhere, not a & version, make sure we're not fucking up where we're passing & in arguments, replace all generic errors with descriptive ones: throw() assert()
// make sure we unbind everything once we're done using it, see where the code could fail for every function
// on creation of a resource, check the validity of ids, check on draw that the asset is valid
// Refactor shaders.
// TODO: rename any <baseClass> variables into <baseClassId> variables where appropriate to better reflect the type of the variable.
// TODO: remove all the old commented code.
// TODO: rename files names to use CamelCase
// TODO: remove id_ from classes, it's useless
// TODO: make a define for lambda with value of <Shader& shader, const Model& model> and <Shader& shader, const Model& model, const Camera& camera>
// TODO: make sure whenever we bind shit, we unbind it afterwards

// TODO: Do all the above crap, rebuild the demo scene.

#include <glad/glad.h>

#include "engine.h"
#include "resource_manager.h"

namespace gl {

class HelloTriangle : public Program
{
public:
    void Init() override
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_FRAMEBUFFER_SRGB_EXT);

        {
            ResourceManager::CameraDefinition def;
            camera_ = resourceManager_.CreateResource(def);
        }

        // Model creation.
        const auto meshes = resourceManager_.LoadObj("../data/models/tank/tank.obj");

        ModelId modelId = DEFAULT_ID;
        {
            ResourceManager::ModelDefinition def;
            def.meshes = meshes;
            def.transform = resourceManager_.CreateResource(ResourceManager::Transform3dDefinition{});
            modelId = resourceManager_.CreateResource(def);
        }

        model_ = resourceManager_.GetModel(modelId);

        // Skybox creation.
        {
            ResourceManager::SkyboxDefinition def;
            def.correctGamma = true;
            def.paths = Skybox::Paths{};
            SkyboxId id = resourceManager_.CreateResource(def);
            assert(id != DEFAULT_ID);
            skybox_ = resourceManager_.GetSkybox(id);
        }
    }
    void Update(seconds dt) override
    {
        timer_ += dt.count();
        glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        model_.Draw();
        skybox_.Draw();
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

        Camera& camera = resourceManager_.GetCamera(camera_);
        switch (event.type)
        {
        case SDL_KEYDOWN:

            switch (event.key.keysym.sym)
            {
            case SDLK_w:
                camera.ProcessKeyboard(Camera::Camera_Movement::FORWARD);
                break;
            case SDLK_s:
                camera.ProcessKeyboard(Camera::Camera_Movement::BACKWARD);
                break;
            case SDLK_a:
                camera.ProcessKeyboard(Camera::Camera_Movement::LEFT);
                break;
            case SDLK_d:
                camera.ProcessKeyboard(Camera::Camera_Movement::RIGHT);
                break;
            case SDLK_SPACE:
                camera.ProcessKeyboard(Camera::Camera_Movement::UP);
                break;
            case SDLK_LCTRL:
                camera.ProcessKeyboard(Camera::Camera_Movement::DOWN);
                break;
            default:
                break;
            }
            break;
        case SDL_MOUSEMOTION:
            if (mouseButtonDown_) camera.ProcessMouseMovement(event.motion.xrel, event.motion.yrel);
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
    float timer_ = 0.0f;
    bool mouseButtonDown_ = false;
    CameraId camera_ = DEFAULT_ID;
    Model model_;
    Skybox skybox_;
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
