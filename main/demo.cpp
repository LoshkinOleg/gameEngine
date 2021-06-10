#include <iostream>
#include <array>

#include <glad/glad.h>
#include "imgui.h"
// #include "imgui_impl_opengl3.h"
// #include "imgui_impl_sdl.h"

#include "engine.h"
#include "camera.h"
#include "shader.h"
#include "mesh.h"
#include "framebuffer.h"
#include "skybox.h"

#define PERSPECTIVE glm::perspective(glm::radians(45.0f), 1024.0f / 720.0f, 0.1f, 100.0f)
#define IDENTITY_MAT4 glm::mat4(1.0f)

namespace gl {

class Cube
{
public:
    Cube() = default;
    void Init(const std::string& shaderName, std::function<void(Shader)> shaderOnInit, std::function<void(Shader)> shaderOnDraw, std::array<const char*, 6> cubemap_names)
    {
        shaderOnDraw_ = shaderOnDraw;
        shader_ = Shader("../data/shaders/" + shaderName + ".vert", "../data/shaders/" + shaderName + ".frag");
        cubemap_ = Cubemap(cubemap_names);

        float cubeVertices[] = {
            // positions          // normals
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
        };
        glGenVertexArrays(1, &VAO_);
        glGenBuffers(1, &VBO_);
        glBindVertexArray(VAO_);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        shader_.Bind();
        try
        {
            shaderOnInit(shader_);
        }
        catch (const std::exception& e)
        {
            std::cerr << "ERROR: exception occured during invokation of shaderOnInit in Cube::Init(): " << e.what() << std::endl;
            abort();
        }
        shader_.UnBind();
    }
    void Draw()
    {
        shader_.Bind();
        try
        {
            shaderOnDraw_(shader_);
        }
        catch (const std::exception& e)
        {
            std::cerr << "ERROR: exception occured during invokation of shaderOnInit in Cube::Init(): " << e.what() << std::endl;
            abort();
        }
        glBindVertexArray(VAO_);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        cubemap_.Bind(0);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        cubemap_.UnBind();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        shader_.UnBind();
    }
    void Destroy()
    {
        shader_.Destroy();
        cubemap_.Destroy();
        glDeleteBuffers(1, &VBO_);
        glDeleteVertexArrays(1, &VAO_);
    }
private:
    unsigned int VAO_ = 0, VBO_ = 0;
    Cubemap cubemap_ = {};
    Shader shader_ = {};
    std::function<void(Shader)> shaderOnDraw_ = nullptr;
};

class Demo : public Program
{
public:
    void Init() override
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
        modelCrateTransforming_.Init
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
        
        // Init stuff for skybox and ambientmap scene.
        cubeAmbientmap_.Init
        (
            "demo/environmentmap",
            [](Shader shader)->void
            {
                shader.SetMat4("projection", PERSPECTIVE);
                shader.SetMat4("model", IDENTITY_MAT4);
                shader.SetInt("environmentMap", 0);
            },
            [this](Shader shader)->void
            {
                shader.SetMat4("view", camera_.GetViewMatrix());
                shader.SetVec3("cameraPos", camera_.GetPosition());
            },
            std::array<const char*, 6>
            {
                "../data/textures/skybox/right.jpg",
                "../data/textures/skybox/left.jpg",
                "../data/textures/skybox/top.jpg",
                "../data/textures/skybox/bottom.jpg",
                "../data/textures/skybox/front.jpg",
                "../data/textures/skybox/back.jpg"
            }
        );
        skybox_.Init
        (
            "demo/skybox",
            std::array<const char*, 6>
            {
                "../data/textures/skybox/right.jpg",
                "../data/textures/skybox/left.jpg",
                "../data/textures/skybox/top.jpg",
                "../data/textures/skybox/bottom.jpg",
                "../data/textures/skybox/front.jpg",
                "../data/textures/skybox/back.jpg"
            }
        );
        
        // Init stuff for post process scene.
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
            }
        );
        framebufferPostProcessing_.Init("demo/framebuffer_postprocess", (int)SCREEN_RESOLUTION[0], (int)SCREEN_RESOLUTION[1]);
        
        // Init stuff for gamma scene.
        modelGammaCrate_.Init
        (
            "crate",
            "demo/gamma",
            [](Shader shader, Mesh mesh)->void
            {
                shader.SetVec3("mat.ambientColor", mesh.GetAmbientColor());
                shader.SetVec3("mat.diffuseColor", mesh.GetDiffuseColor());
                shader.SetVec3("mat.specularColor", mesh.GetSpecularColor());
                shader.SetFloat("mat.shininess", mesh.GetShininess());
                shader.SetVec3("dirLight.dir", glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f)));
                shader.SetVec3("dirLight.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
                shader.SetVec3("dirLight.diffuse", glm::vec3(0.9f, 0.9f, 0.9f));
                shader.SetVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
                shader.SetMat4("projection", PERSPECTIVE);
                shader.SetMat4("model", IDENTITY_MAT4);
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

        // Init stuff for phong scene.
        modelPhongCrate_.Init
        (
            "crate",
            "demo/phong",
            [this](Shader shader, Mesh mesh)->void
            {
                shader.SetMat4("perspective", PERSPECTIVE);
                shader.SetInt("mat.diffuse", 0);
                shader.SetInt("mat.specular", 1);
                shader.SetFloat("mat.shininess", 64.0f);
                shader.SetVec3("dirLight.dir", glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f)));
                shader.SetVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
                shader.SetVec3("dirLight.diffuse", glm::vec3(0.20f, 0.45f, 0.20f));
                shader.SetVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
                shader.SetVec3("ptLight.pos", glm::vec3(0.0f, 0.0f, 0.0f));
                shader.SetVec3("ptLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
                shader.SetVec3("ptLight.diffuse", glm::vec3(0.85f, 0.85f, 0.85f));
                shader.SetVec3("ptLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
                shader.SetFloat("ptLight.constant", 1.0f);
                shader.SetFloat("ptLight.linear", 0.027f);
                shader.SetFloat("ptLight.quadratic", 0.0028f);
                shader.SetVec3("spotLight.pos", camera_.GetPosition());
                shader.SetVec3("spotLight.dir", camera_.GetFront());
                shader.SetVec3("spotLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
                shader.SetVec3("spotLight.diffuse", glm::vec3(0.75f, 0.15f, 0.15f));
                shader.SetVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
                shader.SetFloat("spotLight.constant", 1.0f);
                shader.SetFloat("spotLight.linear", 0.027f);
                shader.SetFloat("spotLight.quadratic", 0.0028f);
                shader.SetFloat("spotLight.innerCutoff", glm::cos(glm::radians(12.5f)));
                shader.SetFloat("spotLight.outerCutoff", glm::cos(glm::radians(17.5f)));
            },
            [this](Shader shader, Mesh mesh)->void
            {
                shader.SetMat4("view", camera_.GetViewMatrix());
                shader.SetVec3("viewPos", camera_.GetPosition());
                shader.SetVec3("spotLight.pos", camera_.GetPosition());
                shader.SetVec3("spotLight.dir", camera_.GetFront());
            }
        );

        // Init stuff for blending scene.
        modelStainedGlass_.Init
        (
            "stainedGlass",
            "demo/blending",
            [this](Shader shader, Mesh mesh)->void
            {
                shader.SetMat4("projection", PERSPECTIVE);
            },
            [this](Shader shader, Mesh mesh)->void
            {
                shader.SetInt("ambient", 0);
                shader.SetMat4("view", camera_.GetViewMatrix());
            }
        );
    }
    void Update(seconds dt) override
    {
        timeSinceStart_ += dt.count();
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        switch (selectedScene_)
        {
        case 0: // obj loader
        {
            modelSpaceship_.Draw(camera_);
        }break;
        case 1: // transforms
        {   
            modelCrateTransforming_.Draw(camera_);
        }break;
        case 2: // hdr
        {
            framebufferHdr_.Bind();
            modelHdr_.Draw(camera_);
            framebufferHdr_.UnBind();
            framebufferHdr_.Draw();
        }break;
        case 3: // skybox and ambientmap
        {
            cubeAmbientmap_.Draw();
            skybox_.Draw(camera_);
        }break;
        case 4:
        {

        }break;
        case 5:
        {

        }break;
        case 6: // Post processing
        {
            framebufferPostProcessing_.Bind();
            modelCrate_.Draw(camera_);
            framebufferPostProcessing_.UnBind();
            framebufferPostProcessing_.Draw();
        }break;
        case 7:
        {

        }break;
        case 8:
        {

        }break;
        case 9: // Gamma correction.
        {
            modelGammaCrate_.Draw(camera_);
        }break;
        case 10: // phong
        {
            glm::vec3 cubePositions[10] = {
                    glm::vec3(0.0f,   0.0f,  -3.0f),
                    glm::vec3(3.0f,   0.0f,   0.0f),
                    glm::vec3(-3.0f,   0.0f,   0.0f),
                    glm::vec3(3.0f,   3.0f,   0.0f),
                    glm::vec3(-3.0f,   3.0f,   0.0f),
                    glm::vec3(3.0f,  -3.0f,   0.0f),
                    glm::vec3(-3.0f,  -3.0f,   0.0f),
                    glm::vec3(3.0f,   0.0f,  -3.0f),
                    glm::vec3(-3.0f,   0.0f,  -3.0f),
                    glm::vec3(0.0f,  -3.0f,  -3.0f)
            };

            for (size_t i = 0; i < 10; i++)
            {
                modelPhongCrate_.GetShader(0).Bind();
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, cubePositions[i]);
                float angle = 20.0f * i;
                model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                modelPhongCrate_.GetShader(0).SetMat4("model", model);
                modelPhongCrate_.Draw(camera_);
            }
            Shader::UnBind();
        }break;
        case 11:
        {

        }break;
        case 12:
        {

        }break;
        case 13: // Blending.
        {
            modelCrate_.Draw(camera_);

            glm::vec3 glassPositions[3] = {
                    glm::vec3(1.0f, 0.0f, -2.0f),
                    glm::vec3(1.0f, 0.0f, 2.0f),
                    glm::vec3(0.0f, 0.0f, 3.0f)
            };

            for (size_t i = 0; i < 3; i++)
            {
                modelStainedGlass_.GetShader(0).Bind();
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glassPositions[i]);
                modelStainedGlass_.GetShader(0).SetMat4("model", model);
                modelStainedGlass_.Draw(camera_);
            }
            Shader::UnBind();
        }break;
        default:
            break;
        }
    }
    void Destroy() override
    {
        modelSpaceship_.Destroy();
        modelCrateTransforming_.Destroy();
        modelHdr_.Destroy();
        framebufferHdr_.Destroy();
        cubeAmbientmap_.Destroy();
        skybox_.Destroy();
        modelCrate_.Destroy();
        framebufferPostProcessing_.Destroy();
        modelGammaCrate_.Destroy();
        modelPhongCrate_.Destroy();
        modelStainedGlass_.Destroy();
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
        const char* const items[14] =
        {
            "Obj loading", // 0 OK
            "Transform operations", // 1 OK
            "HDR", // 2 OK
            "Skybox and ambientmap", // 3 OK
            "Normalmap", // 4
            "Ambient occlusion", // 5
            "Post processing", // 6 OK
            "Shadows", // 7
            "GPU instancing", // 8
            "Gamma correction", // 9 OK
            "Phong rendering", // 10 OK
            "PBR rendering", // 11
            "Deferred rendering", // 12
            "Blending" // 13
        };
        ImGui::ListBox("", &selectedScene_, items, 14);
        ImGui::End();
    }

protected:
    Camera camera_;
    float timeSinceStart_ = 0.0f;
    int selectedScene_ = 0;
    Model modelSpaceship_, modelCrateTransforming_, modelHdr_, modelAmbientmap_, modelCrate_, modelGammaCrate_, modelPhongCrate_, modelStainedGlass_;
    bool mouseButtonDown_ = false;
    Framebuffer framebufferHdr_, framebufferPostProcessing_;
    Cube cubeAmbientmap_;
    Skybox skybox_;

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