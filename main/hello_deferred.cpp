#include <vector>

#include <glad/glad.h>

#include "engine.h"
#include "model.h"
#include "skybox.h"
#include "framebuffer.h"
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

            const float bounds = 10.0f;
            const float near = 0.0f;
            const float far = 100.0f;
            glm::mat4 lightProjectionMatrix = glm::ortho(-bounds, bounds, -bounds, bounds, near, far);
            const glm::vec3 lightPos = ONE_VEC3 * 10.0f;
            glm::mat4 lightViewMatrix = glm::lookAt(lightPos, ZERO_VEC3, UP_VEC3);

            // Crate model.
            {
                VertexBuffer::Definition vbdef;
                const auto objData = ResourceManager::ReadObj("../data/models/brickSphere/brickSphere.obj");
                for (size_t i = 0; i < objData[0].positions.size(); i++)
                {
                    vbdef.data.push_back(objData[0].positions[i].x);
                    vbdef.data.push_back(objData[0].positions[i].y);
                    vbdef.data.push_back(objData[0].positions[i].z);

                    vbdef.data.push_back(objData[0].uvs[i].x);
                    vbdef.data.push_back(objData[0].uvs[i].y);

                    vbdef.data.push_back(objData[0].normals[i].x);
                    vbdef.data.push_back(objData[0].normals[i].y);
                    vbdef.data.push_back(objData[0].normals[i].z);

                    vbdef.data.push_back(objData[0].tangents[i].x);
                    vbdef.data.push_back(objData[0].tangents[i].y);
                    vbdef.data.push_back(objData[0].tangents[i].z);
                }
                vbdef.dataLayout =
                {
                    3,
                    2,
                    3,
                    3
                };

                Material::Definition matdef;
                matdef.shader.vertexPath = "../data/shaders/illum2_shadowmapped.vert";
                matdef.shader.fragmentPath = "../data/shaders/illum2_shadowmapped.frag";
                matdef.texturePathsAndTypes.push_back({ "../data/textures/brickwall.jpg", Texture::Type::AMBIENT });
                matdef.shader.staticInts.insert({ AMBIENT_SAMPLER_NAME, AMBIENT_TEXTURE_UNIT });
                matdef.texturePathsAndTypes.push_back({ "../data/textures/blank2x2.jpg", Texture::Type::ALPHA });
                matdef.shader.staticInts.insert({ ALPHA_SAMPLER_NAME, ALPHA_TEXTURE_UNIT });
                matdef.texturePathsAndTypes.push_back({ "../data/textures/brickwall.jpg", Texture::Type::DIFFUSE });
                matdef.shader.staticInts.insert({ DIFFUSE_SAMPLER_NAME, DIFFUSE_TEXTURE_UNIT });
                matdef.texturePathsAndTypes.push_back({ "../data/textures/blank2x2.jpg", Texture::Type::SPECULAR });
                matdef.shader.staticInts.insert({ SPECULAR_SAMPLER_NAME, SPECULAR_TEXTURE_UNIT });
                matdef.texturePathsAndTypes.push_back({ "../data/textures/brickwall_normal.jpg", Texture::Type::NORMALMAP });
                matdef.shader.staticInts.insert({ NORMALMAP_SAMPLER_NAME, NORMALMAP_TEXTURE_UNIT });
                matdef.shader.staticInts.insert({ FRAMEBUFFER_SAMPLER0_NAME, FRAMEBUFFER_TEXTURE0_UNIT });
                matdef.shader.staticFloats.insert({ SHININESS_NAME, 64.0f });
                matdef.shader.staticMat4s.insert({ PROJECTION_MARIX_NAME, PERSPECTIVE });
                matdef.shader.staticMat4s.insert({ LIGHT_MATRIX_NAME, lightProjectionMatrix * lightViewMatrix});
                matdef.shader.dynamicMat4s.insert({ VIEW_MARIX_NAME, ResourceManager::Get().GetCamera().GetViewMatrixPtr() });
                matdef.shader.dynamicVec3s.insert({ VIEW_POSITION_NAME, ResourceManager::Get().GetCamera().GetPositionPtr() });

                std::vector<glm::mat4> modelMatrices_ = { IDENTITY_MAT4, IDENTITY_MAT4, IDENTITY_MAT4 };
                model_.Create(vbdef, matdef, modelMatrices_);
            }

            // Floor model.
            {
                VertexBuffer::Definition vbdef;
                const auto objData = ResourceManager::ReadObj("../data/models/cratePlane/cratePlane.obj");
                for (size_t i = 0; i < objData[0].positions.size(); i++)
                {
                    vbdef.data.push_back(objData[0].positions[i].x);
                    vbdef.data.push_back(objData[0].positions[i].y);
                    vbdef.data.push_back(objData[0].positions[i].z);

                    vbdef.data.push_back(objData[0].uvs[i].x);
                    vbdef.data.push_back(objData[0].uvs[i].y);

                    vbdef.data.push_back(objData[0].normals[i].x);
                    vbdef.data.push_back(objData[0].normals[i].y);
                    vbdef.data.push_back(objData[0].normals[i].z);

                    vbdef.data.push_back(objData[0].tangents[i].x);
                    vbdef.data.push_back(objData[0].tangents[i].y);
                    vbdef.data.push_back(objData[0].tangents[i].z);
                }
                vbdef.dataLayout =
                {
                    3,
                    2,
                    3,
                    3
                };

                Material::Definition matdef;
                matdef.shader.vertexPath = "../data/shaders/illum2_shadowmapped.vert";
                matdef.shader.fragmentPath = "../data/shaders/illum2_shadowmapped.frag";
                matdef.texturePathsAndTypes.push_back({ "../data/textures/crate_diffuse.jpg", Texture::Type::AMBIENT });
                matdef.shader.staticInts.insert({ AMBIENT_SAMPLER_NAME, AMBIENT_TEXTURE_UNIT });
                matdef.texturePathsAndTypes.push_back({ "../data/textures/blank2x2.jpg", Texture::Type::ALPHA });
                matdef.shader.staticInts.insert({ ALPHA_SAMPLER_NAME, ALPHA_TEXTURE_UNIT });
                matdef.texturePathsAndTypes.push_back({ "../data/textures/crate_diffuse.jpg", Texture::Type::DIFFUSE });
                matdef.shader.staticInts.insert({ DIFFUSE_SAMPLER_NAME, DIFFUSE_TEXTURE_UNIT });
                matdef.texturePathsAndTypes.push_back({ "../data/textures/crate_specular.jpg", Texture::Type::SPECULAR });
                matdef.shader.staticInts.insert({ SPECULAR_SAMPLER_NAME, SPECULAR_TEXTURE_UNIT });
                matdef.texturePathsAndTypes.push_back({ "../data/textures/crate_normals.png", Texture::Type::NORMALMAP });
                matdef.shader.staticInts.insert({ NORMALMAP_SAMPLER_NAME, NORMALMAP_TEXTURE_UNIT });
                matdef.shader.staticInts.insert({ FRAMEBUFFER_SAMPLER0_NAME, FRAMEBUFFER_TEXTURE0_UNIT });
                matdef.shader.staticFloats.insert({ SHININESS_NAME, 64.0f });
                matdef.shader.staticMat4s.insert({ PROJECTION_MARIX_NAME, PERSPECTIVE });
                matdef.shader.staticMat4s.insert({ LIGHT_MATRIX_NAME, lightProjectionMatrix * lightViewMatrix });
                matdef.shader.dynamicMat4s.insert({ VIEW_MARIX_NAME, ResourceManager::Get().GetCamera().GetViewMatrixPtr() });
                matdef.shader.dynamicVec3s.insert({ VIEW_POSITION_NAME, ResourceManager::Get().GetCamera().GetPositionPtr() });

                std::vector<glm::vec3> positions;
                positions.push_back(glm::vec3(0.0f, -3.0f, 0.0f));
                std::vector<glm::vec3> scales;
                scales.push_back(ONE_VEC3 * 10.0f);
                std::vector<glm::mat4> transformMatrices;
                for (size_t i = 0; i < positions.size(); i++)
                {
                    transformMatrices.push_back(glm::translate(IDENTITY_MAT4, positions[i]));
                    transformMatrices[i] = glm::scale(transformMatrices[i], scales[i]);
                }

                floor_.Create(vbdef, matdef, transformMatrices);
            }

            // Shadowmap shader.
            {
                Shader::Definition sdef;
                sdef.vertexPath = "../data/shaders/shadowmapping.vert";
                sdef.fragmentPath = "../data/shaders/empty.frag";
                sdef.staticMat4s.insert({ LIGHT_MATRIX_NAME, lightProjectionMatrix * lightViewMatrix });
                shaderShadowmap_.Create(sdef);
            }

            // Framebuffer.
            {
                Framebuffer::Definition fbdef;
                fbdef.type = (Framebuffer::Type)((int)Framebuffer::Type::FBO_DEPTH0 | (int)Framebuffer::Type::NO_DRAW | (int)Framebuffer::Type::HDR);
                fbdef.resolution = {1024, 1024};

                fb_.Create(fbdef);
            }
        }
        void Update(seconds dt) override
        {
            timer_ += dt.count();
            glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            auto& modelMatrices = model_.GetModelMatrices();
            modelMatrices[0] = glm::translate(IDENTITY_MAT4, 2.0f * glm::vec3(glm::cos(timer_), glm::sin(timer_), 0.0f));
            modelMatrices[1] = glm::translate(IDENTITY_MAT4, 2.0f * glm::vec3(glm::cos(timer_ + 3.14f), 0.0f, glm::sin(timer_ + 3.14)));
            modelMatrices[2] = glm::translate(IDENTITY_MAT4, 2.0f * glm::vec3(0.0f, glm::cos(timer_), glm::sin(timer_)));

            // Shadow pass.
            fb_.Bind();
            floor_.DrawUsingShader(shaderShadowmap_);
            model_.DrawUsingShader(shaderShadowmap_);
            fb_.Unbind();

            // Final pass.
            fb_.BindTextures();
            floor_.Draw();
            model_.Draw();
            // fb_.UnbindTextures();
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
        float interpolationFactor_ = 0.0f;
        Model model_;
        Model floor_;
        Shader shaderShadowmap_;
        Framebuffer fb_;
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
