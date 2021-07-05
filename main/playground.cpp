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

            Framebuffer::Definition fbdef;
            fbdef.resolution = { 1024, 1024 };
            fbdef.type = (Framebuffer::Type)
                (
                    Framebuffer::Type::FBO_DEPTH0_NO_DRAW
                );
            fb_.Create(fbdef);

            const glm::mat4 lightMatrix = glm::mat4(glm::mat3((ORTHO * glm::lookAt(ONE_VEC3, ZERO_VEC3, UP_VEC3))));

            {
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

                Material::Definition matdef = resourceManager_.PreprocessMaterialData(objData)[0];
                matdef.shader.vertexPath = "../data/shaders/illum2_shadowmapped.vert";
                matdef.shader.fragmentPath = "../data/shaders/illum2_shadowmapped.frag";
                matdef.shader.staticMat4s.insert({ LIGHT_MATRIX_NAME, lightMatrix });
                matdef.shader.dynamicMat4s.insert({ CAMERA_MARIX_NAME, resourceManager_.GetCamera().GetCameraMatrixPtr() });
                matdef.shader.dynamicVec3s.insert({ VIEW_POSITION_NAME, resourceManager_.GetCamera().GetPositionPtr() });
                matdef.shader.staticInts.insert({ FRAMEBUFFER_SAMPLER0_NAME, FRAMEBUFFER_TEXTURE0_UNIT });

                std::vector<glm::mat4> matrices;
                const float spacing = 5.0f;
                matrices.push_back(glm::translate(IDENTITY_MAT4, UP_VEC3));
                matrices.push_back(glm::translate(IDENTITY_MAT4, UP_VEC3 + RIGHT_VEC3 * spacing));
                matrices.push_back(glm::translate(IDENTITY_MAT4, UP_VEC3 + LEFT_VEC3  * spacing));
                matrices.push_back(glm::translate(IDENTITY_MAT4, UP_VEC3 + FRONT_VEC3 * spacing));
                matrices.push_back(glm::translate(IDENTITY_MAT4, UP_VEC3 + BACK_VEC3  * spacing));
                matrices.push_back(glm::translate(IDENTITY_MAT4, UP_VEC3 + RIGHT_VEC3 * spacing + FRONT_VEC3 * spacing));
                matrices.push_back(glm::translate(IDENTITY_MAT4, UP_VEC3 + RIGHT_VEC3 * spacing + BACK_VEC3  * spacing));
                matrices.push_back(glm::translate(IDENTITY_MAT4, UP_VEC3 + LEFT_VEC3  * spacing + FRONT_VEC3 * spacing));
                matrices.push_back(glm::translate(IDENTITY_MAT4, UP_VEC3 + LEFT_VEC3  * spacing + BACK_VEC3  * spacing));

                model_.Create({ vbdef }, { matdef }, matrices);
            }

            // Load floor mesh.
            VertexBuffer::Definition vbdef;
            const auto objData = ResourceManager::ReadObj("../data/models/cratePlane/cratePlane.obj");
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

            Material::Definition matdef = resourceManager_.PreprocessMaterialData(objData)[0];
            matdef.shader.vertexPath = "../data/shaders/illum2_shadowmapped.vert";
            matdef.shader.fragmentPath = "../data/shaders/illum2_shadowmapped.frag";
            matdef.shader.staticMat4s.insert({ LIGHT_MATRIX_NAME, lightMatrix });
            matdef.shader.dynamicMat4s.insert({ CAMERA_MARIX_NAME, resourceManager_.GetCamera().GetCameraMatrixPtr() });
            matdef.shader.dynamicVec3s.insert({ VIEW_POSITION_NAME, resourceManager_.GetCamera().GetPositionPtr() });
            matdef.shader.staticInts.insert({ FRAMEBUFFER_SAMPLER0_NAME, FRAMEBUFFER_TEXTURE0_UNIT });

            floor_.Create({ vbdef }, { matdef }, { glm::scale(IDENTITY_MAT4, ONE_VEC3 * 10.0f) });

            Shader::Definition sdef;
            sdef.vertexPath = "../data/shaders/shadowmapping.vert";
            sdef.fragmentPath = "../data/shaders/empty.frag";
            sdef.staticMat4s.insert({ LIGHT_MATRIX_NAME, lightMatrix });
            shaderShadowpass_.Create(sdef);

            skybox_.Create(Skybox::Definition());
        }
        void Update(seconds dt) override
        {
            timer_ += dt.count();
            glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            fb_.Bind();
            glCullFace(GL_FRONT);
            model_.DrawUsingShader(shaderShadowpass_);
            floor_.DrawUsingShader(shaderShadowpass_);
            glCullFace(GL_BACK);
            fb_.Unbind();

            fb_.BindGBuffer();
            model_.Draw();
            floor_.Draw(true);
            skybox_.Draw();
            fb_.UnbindGBuffer();
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
        Model model_, floor_;
        Skybox skybox_;
        Framebuffer fb_;
        Shader shaderShadowpass_;
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
