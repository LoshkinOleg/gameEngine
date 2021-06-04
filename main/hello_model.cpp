#include "engine.h"
#include "shader.h"
#include "camera.h"
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
    unsigned int axisVAO_;
    unsigned int axisVBO_;
    unsigned int axisProgram_;

    Shader axisShader_ = Shader();
    Model model_;
    Camera camera_;

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
    // GL testings.
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    IsError(__FILE__, __LINE__);

    // Internal variables.
    axisShader_ = Shader("../data/shaders/plain.vert", "../data/shaders/plain.frag");
    model_ = Model("../data/models/crate/", "crate", "../data/shaders/hello_model/", "model");

    // Data.
    float axisVertices[18] = {
        0.0f, 0.0f, 0.0f, // X
        1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.0f, // Y
        0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f, // Z
        0.0f, 0.0f, 1.0f
    };

    // Fill out buffers.
    glGenVertexArrays(1, &axisVAO_);
    glBindVertexArray(axisVAO_);
    glGenBuffers(1, &axisVBO_);
    glBindBuffer(GL_ARRAY_BUFFER, axisVBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    IsError(__FILE__, __LINE__);

    // Set up uniforms.
    glm::mat4 perspective = glm::perspective(glm::radians(45.0f), SCREEN_RESOLUTION[0] / SCREEN_RESOLUTION[1], 0.1f, 1000.0f);
    glm::mat4 view = camera_.GetViewMatrix();
    axisShader_.Bind();
    axisShader_.SetMat4("projection", perspective);
    axisShader_.SetMat4("view", view);
    axisShader_.SetMat4("model", glm::mat4(1.0f));

    IsError(__FILE__, __LINE__);
}

void HelloTriangle::Update(seconds dt)
{
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    IsError(__FILE__, __LINE__);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    IsError(__FILE__, __LINE__);

    // Draw world axis.
    axisShader_.Bind();
    glBindVertexArray(axisVAO_);
    glBindBuffer(GL_ARRAY_BUFFER, axisVBO_);
    axisShader_.SetMat4("view", camera_.GetViewMatrix());
    axisShader_.SetVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));
    glDrawArrays(GL_LINES, 0, 2);
    axisShader_.SetVec3("color", glm::vec3(0.0f, 1.0f, 0.0f));
    glDrawArrays(GL_LINES, 2, 2);
    axisShader_.SetVec3("color", glm::vec3(0.0f, 0.0f, 1.0f));
    glDrawArrays(GL_LINES, 4, 2);
    IsError(__FILE__, __LINE__);

    model_.Draw(camera_);
    IsError(__FILE__, __LINE__);
}

void HelloTriangle::Destroy()
{
    model_.Destroy();
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
