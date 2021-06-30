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

            {
                const auto objData = ResourceManager::ReadObj("../data/models/highPolySphere/highPolySphere.obj");
                VertexBuffer::Definition vbdef;
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
                    3,2,3,3
                };

                Material::Definition matdef;
                matdef.shader.vertexPath = "../data/shaders/hello_pbr.vert";
                matdef.shader.fragmentPath = "../data/shaders/hello_pbr.frag";
                matdef.shader.staticInts.insert({ALPHA_SAMPLER_NAME, ALPHA_TEXTURE_UNIT});
                matdef.texturePathsAndTypes.push_back({"../data/textures/blank2x2.jpg", Texture::Type::ALPHA});
                matdef.shader.staticInts.insert({NORMALMAP_SAMPLER_NAME, NORMALMAP_TEXTURE_UNIT});
                matdef.texturePathsAndTypes.push_back({ "../data/textures/PBR/metal_plate_Nor_2k.jpg", Texture::Type::NORMALMAP });
                matdef.shader.staticInts.insert({ALBEDO_SAMPLER_NAME, ALBEDO_TEXTURE_UNIT});
                matdef.texturePathsAndTypes.push_back({ "../data/textures/PBR/metal_plate_diff_2k.jpg", Texture::Type::ALBEDO });
                matdef.shader.staticInts.insert({ROUGHNESS_SAMPLER_NAME, ROUGHNESS_TEXTURE_UNIT});
                matdef.texturePathsAndTypes.push_back({ "../data/textures/PBR/metal_plate_rough_2k.jpg", Texture::Type::ROUGHNESS });
                matdef.shader.staticInts.insert({METALLIC_SAMPLER_NAME, METALLIC_TEXTURE_UNIT});
                matdef.texturePathsAndTypes.push_back({ "../data/textures/PBR/metal_plate_metallic_2k.jpg", Texture::Type::METALLIC });
                matdef.shader.staticMat4s.insert({PROJECTION_MARIX_NAME, PERSPECTIVE});
                matdef.shader.staticMat4s.insert({LIGHT_MATRIX_NAME, IDENTITY_MAT4}); // Not using it, it's for direct shadows.
                matdef.shader.dynamicMat4s.insert({VIEW_MARIX_NAME, resourceManager_.GetCamera().GetViewMatrixPtr()});
                matdef.shader.dynamicVec3s.insert({VIEW_POSITION_NAME, resourceManager_.GetCamera().GetPositionPtr()});

                model_.Create({ vbdef }, { matdef });
            }
        }
        void Update(seconds dt) override
        {
            timer_ += dt.count();
            glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            model_.Draw();
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
