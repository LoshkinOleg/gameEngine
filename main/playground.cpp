// TODO: add transparency
// TODO: add frustrum culling

// TODO: try out Gouraud shader

#include <glad/glad.h>
#include "imgui.h"

#include "engine.h"
#include "model.h"
#include "framebuffer.h"
#include "skybox.h"
#include "resource_manager.h"

namespace gl
{

    class HelloTriangle : public Program
    {
    public:
        void Init() override
        {
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // Init reflection framebuffers.
            Framebuffer::Definition fbdef;
            fbdef.resolution = { 1024, 1024 };

            fbdef.type = (Framebuffer::Type)
                (
                    Framebuffer::Type::FBO_RGBA0 |
                    Framebuffer::Type::RBO
                );
            fb_[0].Create(fbdef);

            fbdef.type = (Framebuffer::Type)
                (
                    Framebuffer::Type::FBO_RGBA1 |
                    Framebuffer::Type::RBO
                );
            fb_[1].Create(fbdef);
            fbdef.type = (Framebuffer::Type)
                (
                    Framebuffer::Type::FBO_RGBA2 |
                    Framebuffer::Type::RBO
                );
            fb_[2].Create(fbdef);
            fbdef.type = (Framebuffer::Type)
                (
                    Framebuffer::Type::FBO_RGBA3 |
                    Framebuffer::Type::RBO
                );
            fb_[3].Create(fbdef);
            fbdef.type = (Framebuffer::Type)
                (
                    Framebuffer::Type::FBO_RGBA4 |
                    Framebuffer::Type::RBO
                );
            fb_[4].Create(fbdef);
            fbdef.type = (Framebuffer::Type)
                (
                    Framebuffer::Type::FBO_RGBA5 |
                    Framebuffer::Type::RBO
                );
            fb_[5].Create(fbdef);

            // Load sphere mesh.
            VertexBuffer::Definition vbdef;
            const auto objData = ResourceManager::ReadObj("../data/models/brickSphere/brickSphere.obj");
            for (size_t vertex = 0; vertex < objData[0].positions.size(); vertex++)
            {
                vbdef.data.push_back(objData[0].positions[vertex].x);
                vbdef.data.push_back(objData[0].positions[vertex].y);
                vbdef.data.push_back(objData[0].positions[vertex].z);

                vbdef.data.push_back(objData[0].uvs[vertex].x);
                vbdef.data.push_back(objData[0].uvs[vertex].y);

                vbdef.data.push_back(objData[0].normals[vertex].x);
                vbdef.data.push_back(objData[0].normals[vertex].y);
                vbdef.data.push_back(objData[0].normals[vertex].z);

                vbdef.data.push_back(objData[0].tangents[vertex].x);
                vbdef.data.push_back(objData[0].tangents[vertex].y);
                vbdef.data.push_back(objData[0].tangents[vertex].z);
            }
            vbdef.dataLayout =
            {
                3,2,3,3
            };

            // Create reflection models that are drawn to the reflection framebuffers.
            Material::Definition matdef;
            matdef.shader.vertexPath = "../data/shaders/probe.vert";
            matdef.shader.fragmentPath = "../data/shaders/probe.frag";
            matdef.shader.staticMat4s.insert({ PROJECTION_MARIX_NAME, glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f) });
            matdef.shader.dynamicMat4s.insert({VIEW_MARIX_NAME, &probeViewMatrix_});
            matdef.texturePathsAndTypes.push_back({"../data/textures/brickwall.jpg", Texture::Type::ALPHA}); // tex unit 0 is used for alpha
            matdef.shader.staticInts.insert({"tex", 0});

            std::vector<glm::mat4> matrices;
            matrices.push_back(glm::translate(IDENTITY_MAT4, RIGHT_VEC3 * 4.0f));
            matrices.push_back(glm::translate(IDENTITY_MAT4, -RIGHT_VEC3 * 4.0f));
            matrices.push_back(glm::translate(IDENTITY_MAT4, UP_VEC3 * 4.0f));
            matrices.push_back(glm::translate(IDENTITY_MAT4, -UP_VEC3 * 4.0f));
            matrices.push_back(glm::translate(IDENTITY_MAT4, FRONT_VEC3 * 4.0f));
            matrices.push_back(glm::translate(IDENTITY_MAT4, -FRONT_VEC3 * 4.0f));

            modelReflection_.Create({ vbdef }, { matdef }, matrices);

            // Create real models that are being reflected.
            Material::Definition realMatdef;
            realMatdef.shader.vertexPath = "../data/shaders/textured.vert";
            realMatdef.shader.fragmentPath = "../data/shaders/textured.frag";
            realMatdef.shader.dynamicMat4s.insert({ CAMERA_MARIX_NAME, resourceManager_.GetCamera().GetCameraMatrixPtr() });
            realMatdef.texturePathsAndTypes.push_back({ "../data/textures/brickwall.jpg", Texture::Type::AMBIENT_OR_ALBEDO });
            realMatdef.shader.staticInts.insert({ AMBIENT_SAMPLER_NAME, AMBIENT_TEXTURE_UNIT });


            // Create a model that has a reflective surface.
            Material::Definition reflMatdef;
            reflMatdef.shader.vertexPath = "../data/shaders/environmentmap.vert";
            reflMatdef.shader.fragmentPath = "../data/shaders/environmentmap.frag";
            reflMatdef.shader.dynamicMat4s.insert({ CAMERA_MARIX_NAME, resourceManager_.GetCamera().GetCameraMatrixPtr() });
            reflMatdef.shader.dynamicVec3s.insert({ VIEW_POSITION_NAME, resourceManager_.GetCamera().GetPositionPtr() });
            reflMatdef.shader.staticInts.insert({ "PosX", FRAMEBUFFER_TEXTURE0_UNIT });
            reflMatdef.shader.staticInts.insert({ "NegX", FRAMEBUFFER_TEXTURE1_UNIT });
            reflMatdef.shader.staticInts.insert({ "PosY", FRAMEBUFFER_TEXTURE2_UNIT });
            reflMatdef.shader.staticInts.insert({ "NegY", FRAMEBUFFER_TEXTURE3_UNIT });
            reflMatdef.shader.staticInts.insert({ "PosZ", FRAMEBUFFER_TEXTURE4_UNIT });
            reflMatdef.shader.staticInts.insert({ "NegZ", FRAMEBUFFER_TEXTURE5_UNIT });

            modelReal_.Create({ vbdef }, { realMatdef }, matrices);
            modelReflective_.Create({ vbdef }, { reflMatdef });

            Skybox::Definition sbdef;
            sbdef.shader.vertexPath = "../data/shaders/skybox.vert";
            sbdef.shader.fragmentPath = "../data/shaders/skybox.frag";
            sbdef.shader.staticMat4s.insert({ PROJECTION_MARIX_NAME, glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f) });
            sbdef.shader.dynamicMat4s.insert({ VIEW_MARIX_NAME, &probeViewMatrix_ });
            sbdef.shader.staticInts.insert({CUBEMAP_SAMPLER_NAME, CUBEMAP_TEXTURE_UNIT});
            skybox_.Create(sbdef);
        }
        void Update(seconds dt) override
        {
            timer_ += dt.count();
            glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Reflection pass.
            fb_[0].Bind();
            probeViewMatrix_ = glm::lookAt(glm::vec3(-1.0f, 0.0f, 0.0f), ZERO_VEC3, UP_VEC3);
            modelReflection_.Draw();
            skybox_.Draw();
            fb_[0].Unbind();
            
            fb_[1].Bind();
            probeViewMatrix_ = glm::lookAt(glm::vec3(1.0f, 0.0f, 0.0f), ZERO_VEC3, UP_VEC3);
            modelReflection_.Draw();
            skybox_.Draw();
            fb_[1].Unbind();
            
            fb_[2].Bind();
            probeViewMatrix_ = glm::lookAt(glm::vec3(0.0f, -1.0f, 0.0f), ZERO_VEC3, FRONT_VEC3);
            modelReflection_.Draw();
            skybox_.Draw();
            fb_[2].Unbind();
            
            fb_[3].Bind();
            probeViewMatrix_ = glm::lookAt(glm::vec3(0.0f, 1.0f, 0.0f), ZERO_VEC3, FRONT_VEC3);
            modelReflection_.Draw();
            skybox_.Draw();
            fb_[3].Unbind();
            
            fb_[4].Bind();
            probeViewMatrix_ = glm::lookAt(glm::vec3(0.0f, 0.0f, -1.0f), ZERO_VEC3, UP_VEC3);
            modelReflection_.Draw();
            skybox_.Draw();
            fb_[4].Unbind();
            
            fb_[5].Bind();
            probeViewMatrix_ = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), ZERO_VEC3, UP_VEC3);
            modelReflection_.Draw();
            skybox_.Draw();
            fb_[5].Unbind();

            // Render onto backbuffer.

            skybox_.Draw();
            modelReal_.Draw();
            modelReflective_.Draw();
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

            Camera& camera = resourceManager_.GetCamera();
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
        Model modelReflection_, modelReal_, modelReflective_;
        Skybox skybox_;
        Framebuffer fb_[6];
        int faceToRender_ = 0;
        glm::mat4 probeViewMatrix_ = IDENTITY_MAT4;
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
