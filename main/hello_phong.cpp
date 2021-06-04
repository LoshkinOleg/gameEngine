#include <SDL_main.h>
#include <glad/glad.h>
#include <array>
#include <string>
#include <iostream>
#include <fstream>

#include "engine.h"
#include "shader.h"
#include "camera.h"
#include "texture.h"

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
    unsigned int VAO_;
    unsigned int axisVAO_;
    unsigned int VBO_;
    unsigned int axisVBO_;
    unsigned int program_;
    unsigned int axisProgram_;

    Shader shader_ = Shader();
    Shader axisShader_ = Shader();
    Camera camera_;
    Texture diffuseTex_ = Texture();
    Texture specularTex_ = Texture();

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
    // glEnable(GL_CULL_FACE);

    // Internal variables.
    shader_ = Shader("../data/shaders/hello_phong/phong.vert", "../data/shaders/hello_phong/phong.frag");
    axisShader_ = Shader("../data/shaders/plain.vert", "../data/shaders/plain.frag");
    diffuseTex_ = Texture("../data/textures/crate_diffuse.png");
    specularTex_ = Texture("../data/textures/crate_specular.png");

    // Data.
    float vertices[288] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    float axisVertices[18] = {
        0.0f, 0.0f, 0.0f, // X
        1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.0f, // Y
        0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f, // Z
        0.0f, 0.0f, 1.0f
    };

    // Fill out buffers.
    glGenVertexArrays(1, &VAO_);
    glBindVertexArray(VAO_);
    glGenBuffers(1, &VBO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenVertexArrays(1, &axisVAO_);
    glBindVertexArray(axisVAO_);
    glGenBuffers(1, &axisVBO_);
    glBindBuffer(GL_ARRAY_BUFFER, axisVBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Set up uniforms.
    shader_.Bind();
    glm::mat4 perspective = glm::perspective(glm::radians(45.0f), SCREEN_RESOLUTION[0] / SCREEN_RESOLUTION[1], 0.1f, 1000.0f);
    shader_.SetMat4("perspective", perspective);
    glm::mat4 view = camera_.GetViewMatrix();
    shader_.SetMat4("view", view);
    axisShader_.Bind();
    axisShader_.SetMat4("projection", perspective);
    axisShader_.SetMat4("view", view);
    axisShader_.SetMat4("model", glm::mat4(1.0f));
    shader_.Bind();
    shader_.SetInt("mat.diffuse", 0);
    shader_.SetInt("mat.specular", 1);
    shader_.SetFloat("mat.shininess", 64.0f);
    shader_.SetVec3("dirLight.dir", glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f)));
    shader_.SetVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    shader_.SetVec3("dirLight.diffuse", glm::vec3(0.20f, 0.45f, 0.20f));
    shader_.SetVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    shader_.SetVec3("ptLight.pos", glm::vec3(0.0f, 0.0f, 0.0f));
    shader_.SetVec3("ptLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
    shader_.SetVec3("ptLight.diffuse", glm::vec3(0.85f, 0.85f, 0.85f));
    shader_.SetVec3("ptLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    shader_.SetFloat("ptLight.constant", 1.0f);
    shader_.SetFloat("ptLight.linear", 0.027f);
    shader_.SetFloat("ptLight.quadratic", 0.0028f);
    shader_.SetVec3("spotLight.pos", camera_.GetPos());
    shader_.SetVec3("spotLight.dir", camera_.GetFront());
    shader_.SetVec3("spotLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    shader_.SetVec3("spotLight.diffuse", glm::vec3(0.75f, 0.15f, 0.15f));
    shader_.SetVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    shader_.SetFloat("spotLight.constant", 1.0f);
    shader_.SetFloat("spotLight.linear", 0.027f);
    shader_.SetFloat("spotLight.quadratic", 0.0028f);
    shader_.SetFloat("spotLight.innerCutoff", glm::cos(glm::radians(12.5f)));
    shader_.SetFloat("spotLight.outerCutoff", glm::cos(glm::radians(17.5f)));
    shader_.SetVec3("viewPos", camera_.GetPos());
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

    shader_.Bind();
    IsError(__FILE__, __LINE__);
    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    diffuseTex_.Bind(0);
    specularTex_.Bind(1);
    IsError(__FILE__, __LINE__);

    shader_.SetMat4("view", camera_.GetViewMatrix());
    shader_.SetVec3("viewPos", camera_.GetPos());
    shader_.SetVec3("spotLight.pos", camera_.GetPos());
    shader_.SetVec3("spotLight.dir", camera_.GetFront());

    glm::vec3 cubePositions[10] = {
        glm::vec3( 0.0f,   0.0f,  -3.0f),
        glm::vec3( 3.0f,   0.0f,   0.0f),
        glm::vec3(-3.0f,   0.0f,   0.0f),
        glm::vec3( 3.0f,   3.0f,   0.0f),
        glm::vec3(-3.0f,   3.0f,   0.0f),
        glm::vec3( 3.0f,  -3.0f,   0.0f),
        glm::vec3(-3.0f,  -3.0f,   0.0f),
        glm::vec3( 3.0f,   0.0f,  -3.0f),
        glm::vec3(-3.0f,   0.0f,  -3.0f),
        glm::vec3( 0.0f,  -3.0f,  -3.0f)
    };

    for (size_t i = 0; i < 10; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        shader_.SetMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    IsError(__FILE__, __LINE__);
}

void HelloTriangle::Destroy()
{

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
