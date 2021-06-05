#include <iostream>

#include "engine.h"
#include "mesh.h"
#include "framebuffer.h"
#include "shapes.h"

// TODO: Not exactly a mirror... it's just a second rendering from another pov instead.

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
    Model skull_;
    Framebuffer framebuffer_;
    Arrow axis_[3];

    unsigned int VAO_ = 0, VBO_ = 0;
    Shader mirrorShader_;
    Shader projectionShader_;
    unsigned int FBO_ = 0, RBO_ = 0, TEX_ = 0;
    glm::vec3 centroid_ = glm::vec3(0.0f);
    glm::vec3 mirrorNormal_ = glm::vec3(0.0f);
    glm::mat4 mirrorModel_ = glm::mat4(1.0f);

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
    // glEnable(GL_CULL_FACE);

    // Objects in the scene.
    skull_.Init
    (
        "skull", // name of obj's dir
        "textured", // name of shaders
        [this](Shader shader)->void // on shader init
        {
            shader.SetMat4("projection", glm::perspective(glm::radians(45.0f), SCREEN_RESOLUTION[0] / SCREEN_RESOLUTION[1], 0.1f, 100.0f));
            shader.SetMat4("model", glm::mat4(1.0f));
        },
        [this](Shader shader, Mesh mesh)->void // on shader draw
        {
            shader.SetInt("ambient", 0);
            shader.SetMat4("view", camera_.GetViewMatrix());
        }
        );
    framebuffer_.Init("framebuffer_unchanged", (int)SCREEN_RESOLUTION[0], (int)SCREEN_RESOLUTION[1]);
    axis_[0].Init(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    axis_[1].Init(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    axis_[2].Init(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 1.0f, glm::vec3(0.0f, 0.0f, 1.0f));

    // Mirror
    float mirror[30] =
    {
        -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                            
        -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f,  1.0f, 0.0f, 0.0f, 1.0f
    };
    centroid_ = glm::vec3(mirror[0], mirror[1], mirror[2]) + glm::vec3(mirror[5], mirror[6], mirror[7]) + glm::vec3(mirror[10], mirror[11], mirror[12]) + glm::vec3(mirror[20], mirror[21], mirror[22]);
    centroid_ /= 4.0f;
    glGenVertexArrays(1, &VAO_);
    glBindVertexArray(VAO_);
    glGenBuffers(1, &VBO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mirror), mirror, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    IsError(__FILE__, __LINE__);

    projectionShader_ = Shader("../data/shaders/framebuffer_unchanged.vert", "../data/shaders/framebuffer_unchanged.frag"); // This shader projects the scene onto the mirror's texture.
    projectionShader_.Bind();
    projectionShader_.UnBind();

    mirrorShader_ = Shader("../data/shaders/hello_mirror/mirror.vert", "../data/shaders/hello_mirror/mirror.frag"); // This shader draws the mirror with the projected texture on.
    mirrorShader_.Bind();
    mirrorShader_.SetMat4("projection", glm::perspective(glm::radians(45.0f), SCREEN_RESOLUTION[0] / SCREEN_RESOLUTION[1], 0.1f, 100.0f)); // Aspect is 1:1 since it's a square mirror.
    mirrorModel_ = glm::translate(mirrorModel_, glm::vec3(2.0f, 1.0f, -2.0f));
    mirrorModel_ = glm::rotate(mirrorModel_, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    centroid_ = mirrorModel_ * glm::vec4(centroid_, 1.0f);
    glm::vec3 mirrorRight = mirrorModel_ * glm::vec4(glm::normalize(glm::vec3(mirror[10], mirror[11], mirror[12]) - glm::vec3(mirror[0], mirror[1], mirror[2])), 1.0f);
    glm::vec3 mirrorUp = mirrorModel_ * glm::vec4(glm::normalize(glm::vec3(mirror[20], mirror[21], mirror[22]) - glm::vec3(mirror[0], mirror[1], mirror[2])), 1.0f);
    mirrorRight -= glm::vec3(2.0f, 1.0f, -2.0f);
    mirrorUp -= glm::vec3(2.0f, 1.0f, -2.0f);
    mirrorNormal_ = glm::cross(mirrorRight, mirrorUp);
    mirrorShader_.SetMat4("model", mirrorModel_);
    mirrorShader_.UnBind();

    // Set up mirror's framebuffer.
    glGenFramebuffers(1, &FBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_);
    glGenTextures(1, &TEX_);
    glBindTexture(GL_TEXTURE_2D, TEX_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLint)SCREEN_RESOLUTION[0], (GLint)SCREEN_RESOLUTION[1], 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TEX_, 0);
    glGenRenderbuffers(1, &RBO_);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLint)SCREEN_RESOLUTION[0], (GLint)SCREEN_RESOLUTION[1]);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO_);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void HelloTriangle::Update(seconds dt)
{
    // Draw scene again onto the mirror's framebuffer.
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_); // Bind mirror's fbo
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    

    // Move camera behind the mirror.
    const Camera::CameraState oldState = camera_.GetCameraState(); // Store current camera state to restore to at the end of mirror pass.
    Camera::CameraState mirrorState = oldState;
    // Find distance to mirror necessary for the projection to function properly.
    float distanceToMirror = glm::tan(glm::radians(45.0f * 0.5f));
    mirrorState.position_ = centroid_ - mirrorNormal_ * distanceToMirror;
    mirrorState.front_ = mirrorNormal_;
    mirrorState.right_ = glm::cross(mirrorState.front_, glm::vec3(0.0f, 1.0f, 0.0f));
    mirrorState.up_ = glm::cross(mirrorState.right_, mirrorState.front_);
    camera_.SetCameraState(mirrorState);

    // Draw from the new view position.
    skull_.Draw(camera_);
    for (size_t i = 0; i < 3; i++)
    {
        axis_[i].Draw(camera_);
    } // Done drawing scene to mirror framebuffer. mirror texture is written to.

    // projectionShader_.Bind();
    // projectionShader_.SetInt("screenTex", 0);
    // glDisable(GL_DEPTH_TEST);
    // glBindVertexArray(VAO_);
    // glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    // glBindTexture(GL_TEXTURE_2D, TEX_);
    // glDrawArrays(GL_TRIANGLES, 0, 6);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);
    // glEnable(GL_DEPTH_TEST);
    // projectionShader_.UnBind();

    camera_.SetCameraState(oldState); // reset camera state to that of our view.
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind mirror's fbo
    glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);






    framebuffer_.Bind(); // Draw to our view's framebuffer.
    skull_.Draw(camera_);
    for (size_t i = 0; i < 3; i++)
    {
        axis_[i].Draw(camera_);
    }

    // Draw mirror as seen from default view
    mirrorShader_.Bind();
    mirrorShader_.SetInt("screenTex", 0);
    mirrorShader_.SetMat4("view", camera_.GetViewMatrix());
    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBindTexture(GL_TEXTURE_2D, TEX_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    mirrorShader_.UnBind();

    framebuffer_.UnBind();


    framebuffer_.Draw(); // Render result to default framebuffer.
}

void HelloTriangle::Destroy()
{
    mirrorShader_.Destroy();
    glDeleteBuffers(1, &VBO_);
    glDeleteVertexArrays(1, &VAO_);

    skull_.Destroy();
    framebuffer_.Destroy();
    for (size_t i = 0; i < 3; i++)
    {
        axis_[i].Destroy();
    }

    mirrorShader_.Destroy();
    glDeleteTextures(1, &TEX_);
    glDeleteRenderbuffers(1, &RBO_);
    glDeleteFramebuffers(1, &FBO_);
    glDeleteBuffers(1, &VBO_);
    glDeleteVertexArrays(1, &VAO_);
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
