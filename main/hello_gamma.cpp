#include <iostream>

#include "engine.h"
#include "mesh.h"

namespace gl {

class HelloTriangle : public Program
{
public:
    void Init() override;
    void Update(seconds dt) override;
    void Destroy() override;
    void OnEvent(SDL_Event& event) override;
    void DrawImGui() override;

protected:
    Camera camera_;
    Model crate_;

    const float SCREEN_RESOLUTION[2] = { 1024.0f, 720.0f };

    void IsError(const std::string& file, int line);
};

void HelloTriangle::IsError(const std::string& file, int line)
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

void HelloTriangle::Init()
{
    glEnable(GL_DEPTH_TEST);

    crate_.Init
    (
        "crate",
        "hello_gamma/gamma",
        [this](Shader shader, Mesh mesh)->void
        {
            shader.SetVec3("mat.ambientColor", mesh.GetAmbientColor());
            shader.SetVec3("mat.diffuseColor", mesh.GetDiffuseColor());
            shader.SetVec3("mat.specularColor", mesh.GetSpecularColor());
            shader.SetFloat("mat.shininess", mesh.GetShininess());
            shader.SetVec3("dirLight.dir", glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f)));
            shader.SetVec3("dirLight.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
            shader.SetVec3("dirLight.diffuse", glm::vec3(0.9f, 0.9f, 0.9f));
            shader.SetVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
            shader.SetMat4("projection", glm::perspective(glm::radians(45.0f), SCREEN_RESOLUTION[0] / SCREEN_RESOLUTION[1], 0.1f, 100.0f));
            shader.SetMat4("model", glm::mat4(1.0f));
        },
        [this](Shader shader, Mesh mesh)->void
        {
            shader.SetInt("mat.ambientMap", 0);
            shader.SetInt("mat.diffuseMap", 1);
            shader.SetInt("mat.specularMap", 2);
            shader.SetVec3("viewPos", camera_.GetPosition());
            shader.SetMat4("view", camera_.GetViewMatrix());
        }
    );
}

void HelloTriangle::Update(seconds dt)
{
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    crate_.Draw(camera_);
}

void HelloTriangle::Destroy()
{
    crate_.Destroy();
}

void HelloTriangle::OnEvent(SDL_Event& event)
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
        camera_.ProcessMouseMovement(event.motion.xrel, event.motion.yrel);
        break;
    case SDL_MOUSEWHEEL:

        break;
    default:
        break;
    }
}

void HelloTriangle::DrawImGui()
{
}

} // End namespace gl.

int main(int argc, char** argv)
{
    gl::HelloTriangle program;
    gl::Engine engine(program);
    engine.Run();
    return EXIT_SUCCESS;
}