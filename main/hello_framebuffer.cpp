#include <iostream>

#include "engine.h"
#include "shapes.h"

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
    // Box box_;
    Quad quad_;

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

    // box_.Init
    // (
    //     std::array<glm::vec3, 8> // points
    //     {
    //         glm::vec3(0.0f, 0.0f, 0.0f),
    //         glm::vec3(1.0f, 0.0f, 0.0f),
    //         glm::vec3(1.0f, 1.0f, 0.0f),
    //         glm::vec3(0.0f, 1.0f, 0.0f),
    // 
    //         glm::vec3(0.0f, 0.0f, 1.0f),
    //         glm::vec3(1.0f, 0.0f, 1.0f),
    //         glm::vec3(1.0f, 1.0f, 1.0f),
    //         glm::vec3(0.0f, 1.0f, 1.0f)
    //     },
    //     glm::vec3(0.0f), // position
    //     glm::vec3(0.0f), // rotation
    //     "plainColor", // shader to use
    //     [this](Shader shader)->void // shader's on init
    //     {
    //         shader.SetMat4("perspective", glm::perspective(glm::radians(45.0f), SCREEN_RESOLUTION[0] / SCREEN_RESOLUTION[1], 0.01f, 100.0f));
    //         shader.SetVec3("color", glm::vec3(1.0f, 1.0f, 1.0f));
    //     },
    //     [this](Shader shader)->void // shader's on draw
    //     {
    //         shader.SetMat4("view", camera_.GetViewMatrix());
    //         shader.SetMat4("model", box_.GetTransform3D().model);
    //     }
    // );

    quad_.Init
    (
        std::array<glm::vec3, 4> // points
        {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        },
        glm::vec3(0.0f), // position
        glm::vec3(0.0f), // rotation
        "plainColor", // shader to use
        [this](Shader shader)->void // shader's on init
        {
            shader.SetMat4("perspective", glm::perspective(glm::radians(45.0f), SCREEN_RESOLUTION[0] / SCREEN_RESOLUTION[1], 0.01f, 100.0f));
            shader.SetVec3("color", glm::vec3(1.0f, 1.0f, 1.0f));
        },
        [this](Shader shader)->void // shader's on draw
        {
            shader.SetMat4("view", camera_.GetViewMatrix());
            shader.SetMat4("model", quad_.GetTransform3D().model);
        }
    );
}

void HelloTriangle::Update(seconds dt)
{
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    // box_.Draw();
    quad_.Draw();
}

void HelloTriangle::Destroy()
{
    //box_.Destroy();
    quad_.Destroy();
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
            camera_.ProcessKeyboard(Camera::Camera_Movement::BACKWARDS);
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
