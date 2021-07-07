#include <functional>
#include <iostream>

#include <glad/glad.h>
#include "imgui.h"
#include <glm/gtc/quaternion.hpp>

#include "engine.h"
#include "model.h"
#include "framebuffer.h"
#include "skybox.h"
#include "resource_manager.h"

// TODO: use basisu to convert to ktx instead of toktx

namespace gl
{
    float RemapToRangeOne(const float inputRangeLower, const float inputRangeUpper, const float value)
    {
        const float outputRangeLower = 0.0f;
        const float outputRangeUpper = 1.0f;
        assert(value >= inputRangeLower && value <= inputRangeUpper);
        return outputRangeLower + (value - inputRangeLower) * (outputRangeUpper - outputRangeLower) / (inputRangeUpper - inputRangeLower);
    }

    class Region
    {
    public:
        Region() = delete;
        Region(const float start, const float end, const std::function<void(const float, const float)> callback) :start_(start), end_(end), callback_(callback) {};

        inline bool IsActive(const float timer) const
        {
            return timer >= start_ && timer < end_;
        }

        void Play() const
        {
            callback_(start_, end_);
        }

    private:
        const float start_;
        const float end_;
        std::function<void(const float start, const float end)> callback_;
    };

    class Demo : public Program
    {
    public:
        void Init() override
        {
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            const std::string dataPath = "C:/Users/admin/Desktop/demoAssets/";

            // Load floor.
            {
                const auto objData = ResourceManager::ReadObj(dataPath + "models/floor/floor.obj");
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
                vbdef.dataLayout = { 3,2,3,3 };

                Material::Definition matdef = ResourceManager::PreprocessMaterialData(objData)[0];
                matdef.shader.vertexPath = "../data/shaders/floor.vert";
                matdef.shader.fragmentPath = "../data/shaders/floor.frag";
                matdef.shader.staticMat4s.insert({ PROJECTION_MARIX_NAME, PERSPECTIVE });
                matdef.shader.dynamicMat4s.insert({ CAMERA_MARIX_NAME, resourceManager_.GetCamera().GetCameraMatrixPtr() });
                matdef.shader.dynamicVec3s.insert({ VIEW_POSITION_NAME, resourceManager_.GetCamera().GetPositionPtr() });

                const float scale = 5.0f;
                const size_t roadLength = 5;
                std::vector<glm::mat4> modelMatrices = std::vector<glm::mat4>(roadLength);
                for (size_t i = 0; i < roadLength; i++)
                {
                    modelMatrices[i] = glm::translate(IDENTITY_MAT4, (float)i * BACK_VEC3 * 2.0f * scale);
                    modelMatrices[i] = glm::scale(modelMatrices[i], ONE_VEC3 * scale);
                }

                floor_.Create({ vbdef }, {matdef}, modelMatrices, scale);
            }

            // Load horse.
            {
                const auto objData0 = ResourceManager::ReadObj(dataPath + "models/horse/horse_base.obj", false);
                const auto objData1 = ResourceManager::ReadObj(dataPath + "models/horse/horse_sphere.obj", false);
                assert(objData0[0].positions.size() == objData1[0].positions.size());
                VertexBuffer::Definition vbdef;
                for (size_t i = 0; i < objData0[0].positions.size(); i++)
                {
                    vbdef.data.push_back(objData0[0].positions[i].x);
                    vbdef.data.push_back(objData0[0].positions[i].y);
                    vbdef.data.push_back(objData0[0].positions[i].z);
                    vbdef.data.push_back(objData1[0].positions[i].x);
                    vbdef.data.push_back(objData1[0].positions[i].y);
                    vbdef.data.push_back(objData1[0].positions[i].z);

                    vbdef.data.push_back(objData0[0].uvs[i].x);
                    vbdef.data.push_back(objData0[0].uvs[i].y);

                    vbdef.data.push_back(objData0[0].normals[i].x);
                    vbdef.data.push_back(objData0[0].normals[i].y);
                    vbdef.data.push_back(objData0[0].normals[i].z);
                    vbdef.data.push_back(objData1[0].normals[i].x);
                    vbdef.data.push_back(objData1[0].normals[i].y);
                    vbdef.data.push_back(objData1[0].normals[i].z);

                    vbdef.data.push_back(objData0[0].tangents[i].x);
                    vbdef.data.push_back(objData0[0].tangents[i].y);
                    vbdef.data.push_back(objData0[0].tangents[i].z);
                    vbdef.data.push_back(objData1[0].tangents[i].x);
                    vbdef.data.push_back(objData1[0].tangents[i].y);
                    vbdef.data.push_back(objData1[0].tangents[i].z);
                }
                vbdef.dataLayout = { 3,3,2,3,3,3,3};

                Material::Definition matdef = ResourceManager::PreprocessMaterialData(objData0)[0];
                matdef.shader.vertexPath = "../data/shaders/horse.vert";
                matdef.shader.fragmentPath = "../data/shaders/horse.frag";
                matdef.shader.staticMat4s.insert({ PROJECTION_MARIX_NAME, PERSPECTIVE });
                matdef.shader.dynamicMat4s.insert({ CAMERA_MARIX_NAME, resourceManager_.GetCamera().GetCameraMatrixPtr() });
                matdef.shader.dynamicVec3s.insert({ VIEW_POSITION_NAME, resourceManager_.GetCamera().GetPositionPtr() });
                matdef.shader.dynamicFloats.insert({"interpolationFactor", &horseMorphingFactor_});

                std::vector<glm::mat4> modelMatrices;
                const glm::vec3 horsePos = RIGHT_VEC3 * 6.0f + UP_VEC3 * 2.0f + BACK_VEC3 * 8.0f;
                const float uniformScale = 5.0f;
                modelMatrices.push_back(glm::translate(IDENTITY_MAT4, horsePos));
                modelMatrices[0] = glm::rotate(modelMatrices[0], glm::radians(-90.0f), UP_VEC3);
                modelMatrices[0] = glm::scale(modelMatrices[0], glm::vec3(uniformScale));

                horse_.Create({ vbdef }, { matdef }, modelMatrices, uniformScale); // TODO: get rid of this uniform scale argument
            }

            Skybox::Definition skdef;
            skdef.path = dataPath + "textures/skybox/skybox.ktx";
            skdef.shader.vertexPath = "../data/shaders/skybox.vert";
            skdef.shader.fragmentPath = "../data/shaders/skybox.frag";
            skdef.shader.staticMat4s.insert({PROJECTION_MARIX_NAME, PERSPECTIVE});
            skdef.shader.dynamicMat4s.insert({VIEW_MARIX_NAME, camera_.GetViewMatrixPtr()});
            skdef.shader.staticInts.insert({CUBEMAP_SAMPLER_NAME, CUBEMAP_TEXTURE_UNIT});
            skybox_.Create(skdef);

            camera_.SetPosition(UP_VEC3);
            camera_.LookAt(FRONT_VEC3 + UP_VEC3, UP_VEC3);

            // Define camera movements.
            regions_.push_back(Region(0.5f, 2.0f, [this](const float start, const float end)->void
            {
                // Turn around.
                const float current = RemapToRangeOne(start, end, timer_);
                const float currentAngle = std::lerp(glm::radians(180.0f), glm::radians(0.0f), current);
                const glm::quat currentRotation = glm::angleAxis(currentAngle, UP_VEC3);
                const glm::vec3 newFront = currentRotation * BACK_VEC3;
                camera_.LookAt(camera_.GetPosition() + newFront, UP_VEC3);
            }));
            regions_.push_back(Region(2.0f, 3.0f, [this](const float start, const float end)->void
            {
                // Turn towards horse.
                const float current = RemapToRangeOne(start, end, timer_);
                const glm::quat startingRot = glm::angleAxis(glm::radians(0.0f), UP_VEC3);
                const glm::quat endingRot = glm::quatLookAt(glm::normalize(horsePosition_ - camera_.GetPosition()), UP_VEC3);
                const glm::quat interpolatedRot = glm::lerp(startingRot, endingRot, current);
                const glm::vec3 newFront = interpolatedRot * BACK_VEC3;
                camera_.LookAt(camera_.GetPosition() + newFront, UP_VEC3);
            }));
            regions_.push_back(Region(3.0f, 13.0f, [this](const float start, const float end)->void
            {
                // Look at horse.
                const float current = RemapToRangeOne(start, end, timer_);
                const glm::vec3 newFront = glm::normalize(horsePosition_ - camera_.GetPosition());
                camera_.LookAt(camera_.GetPosition() + newFront, UP_VEC3);
            }));
            regions_.push_back(Region(13.0f, 15.0f, [this](const float start, const float end)->void
            {
                // Turn back forwards.
                const float current = RemapToRangeOne(start, end, timer_);
                const glm::quat startingRot = glm::quatLookAt(glm::normalize(horsePosition_ - camera_.GetPosition()), UP_VEC3);
                const glm::quat endingRot = glm::angleAxis(glm::radians(0.0f), UP_VEC3);
                const glm::quat interpolatedRot = glm::slerp(startingRot, endingRot, current);
                const glm::vec3 newFront = interpolatedRot * BACK_VEC3;
                camera_.LookAt(camera_.GetPosition() + newFront, UP_VEC3);
            }));
        }
        void Update(seconds dt) override
        {
            const float fdt = dt.count();
            if (fdt > SKIP_FRAME_THRESHOLD_) return;
            timer_ += fdt;

            horseMorphingFactor_ = glm::cos(timer_) * 0.5f + 0.5f;
            horsePosition_ = horse_.GetModelMatrices()[0][3];

            glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (!CONTROL_CAMERA_)
            {
                camera_.SetPosition(camera_.GetPosition() + BACK_VEC3 * dt.count());

                for (const auto& region : regions_)
                {
                    if (region.IsActive(timer_))
                    {
                        region.Play();
                        break;
                    }
                }
            }

            floor_.Draw();
            // TODO: CHANGE ARGUMENTS to prevent accidental casting of size_t to bool -_-
            horse_.Draw(false, HORSE_MODEL_OFFSET_);
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

            if (!CONTROL_CAMERA_) return;

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
        ResourceManager& resourceManager_ = ResourceManager::Get();

        Camera& camera_ = resourceManager_.GetCamera();
        const bool CONTROL_CAMERA_ = false;
        const float SKIP_FRAME_THRESHOLD_ = 1.0f / 30.0f; // Prevents the camera from jumping forward suddently if there's a lag spike like when we start the program.

        std::vector<Region> regions_;

        const size_t HORSE_MODEL_OFFSET_ = 7; // 7 bc there's 7 elements in vbdef.dataLayout
        float horseMorphingFactor_ = 0.0f;
        glm::vec3 horsePosition_ = ZERO_VEC3;

        Skybox skybox_;

        Model
            floor_,
            horse_;
    };

} // End namespace gl.

int main(int argc, char** argv)
{
    gl::Demo program;
    gl::Engine engine(program);
    engine.Run();
    return EXIT_SUCCESS;
}
