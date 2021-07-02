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

            // Model creation.
            {
                const auto objData = ResourceManager::ReadObj("../data/models/brickSphere/brickSphere.obj");
                VertexBuffer::Definition vbdef;
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
                matdef.shader.vertexPath = "../data/shaders/hello_deferred_object.vert";
                matdef.shader.fragmentPath = "../data/shaders/hello_deferred_object.frag";
                matdef.shader.dynamicMat4s.insert({ CAMERA_MARIX_NAME, resourceManager_.GetCamera().GetCameraMatrixPtr() });

                std::vector<glm::mat4> transformModels;
                transformModels.push_back(glm::translate(IDENTITY_MAT4, glm::vec3(-3.0f, -0.5f, -3.0f)));
                // transformModels.back() = glm::scale(transformModels.back(), glm::vec3(0.25f));
                transformModels.push_back(glm::translate(IDENTITY_MAT4, glm::vec3(0.0f, -0.5f, -3.0f)));
                // transformModels.back() = glm::scale(transformModels.back(), glm::vec3(0.25f));
                transformModels.push_back(glm::translate(IDENTITY_MAT4, glm::vec3(3.0f, -0.5f, -3.0f)));
                // transformModels.back() = glm::scale(transformModels.back(), glm::vec3(0.25f));
                transformModels.push_back(glm::translate(IDENTITY_MAT4, glm::vec3(-3.0f, -0.5f, 0.0f)));
                // transformModels.back() = glm::scale(transformModels.back(), glm::vec3(0.25f));
                transformModels.push_back(glm::translate(IDENTITY_MAT4, glm::vec3(0.0f, -0.5f, 0.0f)));
                // transformModels.back() = glm::scale(transformModels.back(), glm::vec3(0.25f));
                transformModels.push_back(glm::translate(IDENTITY_MAT4, glm::vec3(3.0f, -0.5f, 0.0f)));
                // transformModels.back() = glm::scale(transformModels.back(), glm::vec3(0.25f));
                transformModels.push_back(glm::translate(IDENTITY_MAT4, glm::vec3(-3.0f, -0.5f, 3.0f)));
                // transformModels.back() = glm::scale(transformModels.back(), glm::vec3(0.25f));
                transformModels.push_back(glm::translate(IDENTITY_MAT4, glm::vec3(0.0f, -0.5f, 3.0f)));
                // transformModels.back() = glm::scale(transformModels.back(), glm::vec3(0.25f));
                transformModels.push_back(glm::translate(IDENTITY_MAT4, glm::vec3(3.0f, -0.5f, 3.0f)));
                // transformModels.back() = glm::scale(transformModels.back(), glm::vec3(0.25f));

                model_.Create({ vbdef }, { matdef }, transformModels);
            }

            // Framebuffer creation.
            Framebuffer::Definition fbdef;
            fbdef.type = (Framebuffer::Type)
                (
                    (int)Framebuffer::Type::FBO_RGBA0 |
                    (int)Framebuffer::Type::FBO_RGBA1 |
                    (int)Framebuffer::Type::FBO_RGBA2 |
                    (int)Framebuffer::Type::RBO
                );
            fb_.Create(fbdef);

            VertexBuffer::Definition vbdef;
            vbdef.data = QUAD_POSITIONS;
            vbdef.dataLayout = { 2 };

            Material::Definition matdef;
            matdef.shader.vertexPath = "../data/shaders/hello_deferred_quad.vert";
            matdef.shader.fragmentPath = "../data/shaders/hello_deferred_quad.frag";
            matdef.shader.staticInts.insert({ FRAMEBUFFER_SAMPLER0_NAME, FRAMEBUFFER_TEXTURE0_UNIT }); // Positions
            matdef.shader.staticInts.insert({ FRAMEBUFFER_SAMPLER1_NAME, FRAMEBUFFER_TEXTURE1_UNIT }); // Albedo + Specular multiplier
            matdef.shader.staticInts.insert({ FRAMEBUFFER_SAMPLER2_NAME, FRAMEBUFFER_TEXTURE2_UNIT }); // Normals
            matdef.shader.dynamicVec3s.insert({VIEW_POSITION_NAME, resourceManager_.GetCamera().GetPositionPtr()});

            // Set up lights.
            const unsigned int NR_LIGHTS = 32;
            srand(13);
            for (unsigned int i = 0; i < NR_LIGHTS; i++)
            {
                // calculate slightly random offsets
                float xPos = ((rand() % 100) / 100.0f) * 6.0f - 3.0f;
                float yPos = ((rand() % 100) / 100.0f) * 6.0f - 4.0f;
                float zPos = ((rand() % 100) / 100.0f) * 6.0f - 3.0f;
                // also calculate random color
                float rColor = ((rand() % 100) / 200.0f) + 0.5f; // between 0.5 and 1.0
                float gColor = ((rand() % 100) / 200.0f) + 0.5f; // between 0.5 and 1.0
                float bColor = ((rand() % 100) / 200.0f) + 0.5f; // between 0.5 and 1.0

                const float linear = 0.7;
                const float quadratic = 1.8;
                const float maxBrightness = std::fmaxf(std::fmaxf(rColor, gColor), bColor);
                const float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (1.0f - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
                matdef.shader.staticVec3s.insert({ "lights[" + std::to_string(i) + "].position", glm::vec3(xPos, yPos, zPos) });
                matdef.shader.staticVec3s.insert({ "lights[" + std::to_string(i) + "].color", glm::vec3(rColor, gColor, bColor) });
                matdef.shader.staticFloats.insert({ "lights[" + std::to_string(i) + "].linear", linear });
                matdef.shader.staticFloats.insert({ "lights[" + std::to_string(i) + "].quadratic", quadratic });
                matdef.shader.staticFloats.insert({ "lights[" + std::to_string(i) + "].radius", radius });
            }

            displayQuad_.Create({ vbdef }, { matdef });

            skybox_.Create({});
        }
        void Update(seconds dt) override
        {
            timer_ += dt.count();
            glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // TODO: find a way around glitchy clear color.

            fb_.Bind();
            skybox_.Draw();
            model_.Draw();
            fb_.Unbind();

            fb_.BindGBuffer();
            displayQuad_.Draw();
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
        Model model_, displayQuad_;
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
