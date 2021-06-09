#include <iostream>

#include <glad/glad.h>
#include "imgui.h"
// #include "imgui_impl_opengl3.h"
// #include "imgui_impl_sdl.h"

#include "engine.h"
#include "camera.h"
#include "shader.h"
#include "mesh.h"
#include "framebuffer.h"

#define PERSPECTIVE glm::perspective(glm::radians(45.0f), 1024.0f / 720.0f, 0.1f, 100.0f)
#define IDENTITY_MAT4 glm::mat4(1.0f)

namespace gl {

class Demo : public Program
{
public:
    void Init() override
    {
        glEnable(GL_DEPTH_TEST);

        // Init stuff for Obj loading scene.
        modelSpaceship_.Init
        (
            "spaceship",
            "demo/ambientTexOnly",
            [](Shader shader, Mesh mesh)->void
            {
                shader.SetMat4("projection", PERSPECTIVE);
                shader.SetMat4("model", IDENTITY_MAT4);
                shader.SetInt("mat.diffuseMap", 1);
                shader.SetInt("mat.specularMap", 2);
                shader.SetVec3("mat.diffuseColor", mesh.GetDiffuseColor());
                shader.SetVec3("mat.specularColor", mesh.GetSpecularColor());
                shader.SetFloat("mat.shininess", mesh.GetShininess());
                shader.SetVec3("dirLight.dir", glm::vec3(-1.0f, -1.0f, -1.0f));
                shader.SetVec3("dirLight.diffuseColor", glm::vec3(1.0f, 1.0f, 1.0f));
                shader.SetVec3("dirLight.specularColor", glm::vec3(1.0f, 1.0f, 1.0f));
            },
            [this](Shader shader, Mesh mesh)->void
            {
                shader.SetMat4("view", camera_.GetViewMatrix());
                shader.SetVec3("viewPos", camera_.GetPosition());
            }
        );

        // Init stuff for transform operations scene.
        modelCrate_.Init
        (
            "crate",
            "demo/ambientTexOnly",
            [](Shader shader, Mesh mesh)->void
            {
                shader.SetMat4("projection", PERSPECTIVE);
                shader.SetMat4("model", IDENTITY_MAT4);
                shader.SetInt("ambient", 0);
            },
            [this](Shader shader, Mesh mesh)->void
            {
                shader.SetMat4("view", camera_.GetViewMatrix());

                glm::mat4 model = glm::mat4(1.0f);
                glm::vec3 pos = glm::vec3(glm::cos(timeSinceStart_), glm::sin(timeSinceStart_), 0.0f);
                glm::vec3 scale = glm::vec3(glm::cos(timeSinceStart_), glm::cos(timeSinceStart_), glm::cos(timeSinceStart_));
                shader.SetMat4("model", glm::scale(glm::rotate(glm::translate(model, pos), glm::cos(timeSinceStart_), glm::vec3(1.0f, 1.0f, 1.0f)), scale));
            }
        );

        // Init stuff for hdr scene.
        modelHdr_.Init
        (
            "crate",
            "demo/hdr_crate",
            [](Shader shader, Mesh mesh)->void
            {
                shader.SetInt("mat.ambientMap", 0);
                shader.SetInt("mat.diffuseMap", 1);
                shader.SetInt("mat.specularMap", 2);
                shader.SetVec3("mat.ambientColor", mesh.GetAmbientColor());
                shader.SetVec3("mat.diffuseColor", mesh.GetDiffuseColor());
                shader.SetVec3("mat.specularColor", mesh.GetSpecularColor());
                shader.SetFloat("mat.shininess", mesh.GetShininess());
                shader.SetVec3("light0.pos", glm::vec3(0.0f, 0.0f, -80.0f));
                shader.SetVec3("light0.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
                shader.SetVec3("light0.diffuse", glm::vec3(100.0f, 100.0f, 100.0f));
                shader.SetVec3("light0.specular", glm::vec3(0.0f, 0.0f, 0.0f));
                shader.SetFloat("light0.constant", 1.0f);
                shader.SetFloat("light0.linear", 0.09f);
                shader.SetFloat("light0.quadratic", 0.032f);
                shader.SetVec3("light1.pos", glm::vec3(3.5f, -3.5f, -3.0f));
                shader.SetVec3("light1.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
                shader.SetVec3("light1.diffuse", glm::vec3(0.0f, 1.0f, 1.0f));
                shader.SetVec3("light1.specular", glm::vec3(0.0f, 0.0f, 0.0f));
                shader.SetFloat("light1.constant", 1.0f);
                shader.SetFloat("light1.linear", 0.7f);
                shader.SetFloat("light1.quadratic", 1.8f);
                shader.SetMat4("projection", PERSPECTIVE);
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -39.0f));
                model = glm::scale(model, glm::vec3(4.0f, 4.0f, 50.0f));
                shader.SetMat4("model", model);
            },
            [this](Shader shader, Mesh mesh)->void
            {
                shader.SetVec3("viewPos", camera_.GetPosition());
                shader.SetMat4("view", camera_.GetViewMatrix());
            }
        );
        framebufferHdr_.Init("demo/hdr_framebuffer", (int)SCREEN_RESOLUTION[0], (int)SCREEN_RESOLUTION[1], true);
    }
    void Update(seconds dt) override
    {
        timeSinceStart_ += dt.count();
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        switch (selectedScene_)
        {
        case 0:
        {
            modelSpaceship_.Draw(camera_);
        }break;
        case 1:
        {   
            modelCrate_.Draw(camera_);
        }break;
        case 2:
        {
            framebufferHdr_.Bind();
            modelHdr_.Draw(camera_);
            framebufferHdr_.UnBind();
            framebufferHdr_.Draw();
        }break;
        default:
            break;
        }
    }
    void Destroy() override
    {
        modelSpaceship_.Destroy();
        modelCrate_.Destroy();
        modelHdr_.Destroy();
        framebufferHdr_.Destroy();
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
        case SDL_MOUSEWHEEL:

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
        ImGui::Begin("Demo selection");
        const char* const items[15] =
        {
            "Obj loading",
            "Transform operations",
            "HDR",
            "Skybox",
            "Ambientmap",
            "Normalmap",
            "Ambient occlusion",
            "Framebuffer",
            "Shadows",
            "GPU instancing",
            "Gamma correction",
            "Phong rendering",
            "PBR rendering",
            "Deferred rendering",
            "Blending"
        };
        ImGui::ListBox("", &selectedScene_, items, 15);
        ImGui::End();
    }

protected:
    Camera camera_;
    float timeSinceStart_ = 0.0f;
    int selectedScene_ = 0;
    Model modelSpaceship_, modelCrate_, modelHdr_;
    bool mouseButtonDown_ = false;
    Framebuffer framebufferHdr_;

    const float SCREEN_RESOLUTION[2] = { 1024.0f, 720.0f };

    void IsError(const std::string& file, int line)
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
    void UpdateObjLoading()
    {

    }
};

} // End namespace gl.

int main(int argc, char** argv)
{
    gl::Demo program;
    gl::Engine engine(program);
    engine.Run();
    return EXIT_SUCCESS;
}
