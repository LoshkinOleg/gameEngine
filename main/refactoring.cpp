// Checklist: use textureType where appropriate, make a separate cubemap class?, make sure there CheckGlError() at key points
// make sure we're using "const auto match" everywhere, not a & version, make sure we're not fucking up where we're passing & in arguments, replace all generic errors with descriptive ones: throw() assert()
// make sure we unbind everything once we're done using it, see where the code could fail for every function
// TODO: rename any <baseClass> variables into <baseClassId> variables where appropriate to better reflect the type of the variable.
// TODO: rename files names to use CamelCase
// TODO: make sure whenever we bind shit, we unbind it afterwards
// TODO: add transparency
// TODO: add frustrum culling
// TODO: handle resizing of window? glViewport and framebuffer too

// TODO: Do all the above crap, rebuild the demo scene.
// TODO: try to instanciate a multi mesh model

#include <glad/glad.h>

#include "engine.h"
#include "model.h"
#include "skybox.h"
#include "framebuffer.h"
#include "resource_manager.h"

namespace gl {

class HelloTriangle : public Program
{
public:
    void Init() override
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // Model creation.
        model_.Create("../data/models/brickSphere/brickSphere.obj");

        // Skybox.
        Skybox::Definition skdef;
        skdef.flipImages = false;
        skybox_.Create(skdef);

        // Framebuffer.
        fb_.Create({});
    }
    void Update(seconds dt) override
    {
        timer_ += dt.count();
        glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        fb_.Bind();
        model_.Draw();
        skybox_.Draw();
        fb_.Unbind();
        fb_.Draw();
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

        Camera& camera = resourceManager_.GetCamera();
        switch (event.type)
        {
        case SDL_KEYDOWN:

            switch (event.key.keysym.sym)
            {
            case SDLK_w:
                camera.ProcessKeyboard(FRONT_VEC3);
                break;
            case SDLK_s:
                camera.ProcessKeyboard(BACK_VEC3);
                break;
            case SDLK_a:
                camera.ProcessKeyboard(LEFT_VEC3);
                break;
            case SDLK_d:
                camera.ProcessKeyboard(RIGHT_VEC3);
                break;
            case SDLK_SPACE:
                camera.ProcessKeyboard(UP_VEC3);
                break;
            case SDLK_LCTRL:
                camera.ProcessKeyboard(DOWN_VEC3);
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
    Model model_;
    Skybox skybox_;
    Framebuffer fb_;
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
