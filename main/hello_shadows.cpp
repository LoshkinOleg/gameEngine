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

            // Crate model.
            {
                VertexBuffer::Definition vbdef;
                const auto objData = ResourceManager::ReadObj("../data/models/brickCube/brickCube.obj");
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
                matdef.shader.vertexPath = "../data/shaders/hello_shadows.vert";
                matdef.shader.fragmentPath = "../data/shaders/hello_shadows.frag";
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
                matdef.shader.staticFloats.insert({ SHININESS_NAME, 64.0f });
                matdef.shader.staticMat4s.insert({ PROJECTION_MARIX_NAME, PERSPECTIVE });
                matdef.shader.dynamicMat4s.insert({ VIEW_MARIX_NAME, ResourceManager::Get().GetCamera().GetViewMatrixPtr() });
                matdef.shader.dynamicVec3s.insert({ VIEW_POSITION_NAME, ResourceManager::Get().GetCamera().GetPositionPtr() });

                std::vector<glm::vec3> positions;
                positions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
                positions.push_back(glm::vec3(3.0f, 3.0f, 0.0f));
                positions.push_back(glm::vec3(3.0f, 3.0f, -3.0f));
                std::vector<glm::mat4> transformMatrices;
                for (size_t i = 0; i < positions.size(); i++)
                {
                    transformMatrices.push_back(glm::translate(IDENTITY_MAT4, positions[i]));
                }

                model_.Create(vbdef, matdef, transformMatrices);
            }

            // Flood model.
            // Crate model.
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
                matdef.shader.vertexPath = "../data/shaders/hello_shadows.vert";
                matdef.shader.fragmentPath = "../data/shaders/hello_shadows.frag";
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
                matdef.shader.staticFloats.insert({ SHININESS_NAME, 64.0f });
                matdef.shader.staticMat4s.insert({ PROJECTION_MARIX_NAME, PERSPECTIVE });
                matdef.shader.dynamicMat4s.insert({ VIEW_MARIX_NAME, ResourceManager::Get().GetCamera().GetViewMatrixPtr() });
                matdef.shader.dynamicVec3s.insert({ VIEW_POSITION_NAME, ResourceManager::Get().GetCamera().GetPositionPtr() });

                std::vector<glm::vec3> positions;
                positions.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
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

            // Skybox.
            Skybox::Definition skdef;
            skdef.flipImages = false;
            skybox_.Create(skdef);

            // Framebuffer.
            fb_.Create({});
        }
        void Update(seconds dt) override
        {
            timer_ += dt.count();
            glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            floor_.Draw();
            model_.Draw();
            skybox_.Draw();
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
        Skybox skybox_;
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
