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

            // Load floor mesh.
            VertexBuffer::Definition vbdef;
            const auto objData = ResourceManager::ReadObj("C:/Users/admin/Desktop/demoAssets/models/floor/floor.obj");
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

            Material::Definition matdef = ResourceManager::PreprocessMaterialData(objData)[0];
            matdef.shader.vertexPath = "../data/shaders/floor.vert";
            matdef.shader.fragmentPath = "../data/shaders/floor.frag";
            matdef.shader.staticMat4s.insert({ PROJECTION_MARIX_NAME, PERSPECTIVE });
            matdef.shader.dynamicMat4s.insert({ CAMERA_MARIX_NAME, resourceManager_.GetCamera().GetCameraMatrixPtr() });
            matdef.shader.dynamicVec3s.insert({ VIEW_POSITION_NAME, resourceManager_.GetCamera().GetPositionPtr() });

            floor_.Create({ vbdef }, { matdef }, { glm::scale(IDENTITY_MAT4, ONE_VEC3 * 10.0f) });
        }
        void Update(seconds dt) override
        {
            timer_ += dt.count();
            glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            floor_.Draw();
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
