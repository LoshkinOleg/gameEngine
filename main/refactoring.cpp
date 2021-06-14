// Checklist: handle UINT_MAX where necessary, use textureType where appropriate, make a separate cubemap class?, check existing code to make sure we didn't miss anything, make sure there CheckGlError() at key points
// make sure we're using "const auto match" everywhere, not a & version, make sure we're not fucking up where we're passing & in arguments, replace all generic errors with descriptive ones: throw() assert()
// make sure we unbind everything once we're done using it, see where the code could fail for every function
// on creation of a resource, check the validity of ids, check on draw that the asset is valid
// move common strings of shader variables to #defines
// Refactor shaders.
// TODO: add framebuffers
// TODO: add skybox
// TODO: add support for cubemaps
// TODO: rename any <baseClass> variables into <baseClassId> variables where appropriate to better reflect the type of the variable.

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
        glEnable(GL_CULL_FACE);
        glEnable(GL_FRAMEBUFFER_SRGB_EXT);

        camera_.Translate(glm::vec3(0.0f, 0.0f, 10.0f)); // Move camera away from origin.

        const auto meshes = resourceManager_.LoadObj("../data/models/crate/crate.obj");

        ModelId modelId = DEFAULT_ID;
        {
            ResourceManager::ModelDefinition def;
            def.meshes = meshes;
            def.transform = resourceManager_.CreateResource(ResourceManager::Transform3dDefinition{});
            modelId = resourceManager_.CreateResource(def);
        }

        model_ = resourceManager_.GetModel(modelId);

        {
            ResourceManager::FramebufferDefinition def;
            def.hdr = true;
            fb_ = resourceManager_.CreateResource(def);
        }
    }
    void Update(seconds dt) override
    {
        timer_ += dt.count();
        glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        model_.Draw(camera_, fb_);
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
    FramebufferId fb_ = DEFAULT_ID;
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
