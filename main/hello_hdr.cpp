#include <iostream>

#include "engine.h"
#include "mesh.h"
#include "framebuffer.h"

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
    Framebuffer fb_;

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

    fb_.Init("hello_hdr/hello_hdr_framebuffer", (int)SCREEN_RESOLUTION[0], (int)SCREEN_RESOLUTION[1], true);

    crate_.Init
    (
        "crate",
        "hello_hdr/hdr",
        [this](Shader shader, Mesh mesh)->void
        {
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
            shader.SetMat4("projection", glm::perspective(glm::radians(45.0f), SCREEN_RESOLUTION[0] / SCREEN_RESOLUTION[1], 0.1f, 100.0f));
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -39.0f));
            model = glm::scale(model, glm::vec3(4.0f, 4.0f, 50.0f));
            shader.SetMat4("model", model);
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
    fb_.Bind();
    crate_.Draw(camera_);
    fb_.UnBind();
    fb_.Draw();
}

void HelloTriangle::Destroy()
{
    crate_.Destroy();
    fb_.Destroy();
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
