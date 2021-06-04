#include <iostream>

#include "engine.h"
#include "mesh.h"
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
    Model crate_;

    unsigned int quadVAO_ = 0, quadVBO_ = 0;
    Shader framebufferShader_ = Shader();
    unsigned int FBO_ = 0, RBO_ = 0, framebufferTEX_ = 0;

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

    // Objects in the scene.
    crate_ = Model("../data/models/crate/", "crate", "../data/shaders/hello_model/", "model");
    framebufferShader_ = Shader("../data/shaders/hello_framebuffer/framebuffer.vert", "../data/shaders/hello_framebuffer/framebuffer.frag");

    // Quad's data.
    float quad[24] =
    {
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,

        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f
    };
    glGenVertexArrays(1, &quadVAO_);
    glBindVertexArray(quadVAO_);
    glGenBuffers(1, &quadVBO_);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Framebuffer.
    glGenFramebuffers(1, &FBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_);

    glGenTextures(1, &framebufferTEX_);
    glBindTexture(GL_TEXTURE_2D, framebufferTEX_);
    // Note that we're using the texture the size of our screen, although it is not mandatory.
    // TODO: Try to make the rendering happen on a smaller framebuffer resolution than the screen's for a pixelated effect.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLint)SCREEN_RESOLUTION[0], (GLint)SCREEN_RESOLUTION[1], 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTEX_, 0);

    glGenRenderbuffers(1, &RBO_);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLint)SCREEN_RESOLUTION[0], (GLint)SCREEN_RESOLUTION[1]);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO_);

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void HelloTriangle::Update(seconds dt)
{
    // NOTE: If rendering to a smaller renderbuffer than your screen, set glViewport to the texture's size!
    // Render our scene as usual but onto our framebuffer instead of the default one.
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST); // We want to depth test for the regular scene.
    crate_.Draw(camera_);

    // Now draw our quad with the texture of the rendered scene on it onto the default framebuffer.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.6f, 0.6f, 0.6f, 1.0f); // Just to differenciate between our framebuffer's background and the default framebuffer's background.
    glClear(GL_COLOR_BUFFER_BIT); // We will only be writing color data.
    glDisable(GL_DEPTH_TEST);
    framebufferShader_.Bind();
    glBindVertexArray(quadVAO_);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO_);
    glBindTexture(GL_TEXTURE_2D, framebufferTEX_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void HelloTriangle::Destroy()
{
    crate_.Destroy();
    glDeleteFramebuffers(1, &FBO_);
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
