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
// TODO: add normalsmaps support
// TODO: add transparency
// TODO: add frustrum culling
// TODO: handle resizing of window? glViewport and framebuffer too

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
        // glEnable(GL_CULL_FACE);
        glEnable(GL_FRAMEBUFFER_SRGB_EXT);

        {
            Camera::Definition def;
            camera_ = resourceManager_.CreateResource(def);
        }

        // Model creation.
        const auto objData = resourceManager_.ReadObjData("../data/models/earth/earth.obj");
        const ModelId id = resourceManager_.CreateResource(objData);
        model_ = resourceManager_.GetModel(id);
        const auto meshIds = model_.GetMesheIds();
        for (size_t i = 0; i < meshIds.size(); i++)
        {
            const auto mesh = resourceManager_.GetMesh(meshIds[i]);
            const auto matId = mesh.GetMaterialId();
            auto& mat = resourceManager_.GetMaterial(matId);
            mat.AddDynamicUniformPair(std::pair<std::string_view, const glm::vec3*>{"lightDir", &lightDir_});
        }

        // Skybox creation.
        {
            Skybox::Definition def;
            def.correctGamma = true;
            def.flipImages = false;
            def.shader = {SKYBOX_SHADER[0], SKYBOX_SHADER[1]};
            def.paths =
            {
                "../data/textures/skybox/right.jpg",
                "../data/textures/skybox/left.jpg",
                "../data/textures/skybox/top.jpg",
                "../data/textures/skybox/bottom.jpg",
                "../data/textures/skybox/front.jpg",
                "../data/textures/skybox/back.jpg"
            };
            skybox_ = resourceManager_.GetSkybox(resourceManager_.CreateResource(def));
        }
    }
    void Update(seconds dt) override
    {
        timer_ += dt.count();
        glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightDir_ = glm::vec3(glm::cos(timer_) * 1.5f, 1.5f, glm::sin(timer_) * 1.5f);

        // skybox_.Draw();
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

        Camera& camera = resourceManager_.GetCamera(camera_);
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
    CameraId camera_ = DEFAULT_ID;
    Model model_;
    Skybox skybox_;
    glm::vec3 lightDir_ = -ONE_VEC3;
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
