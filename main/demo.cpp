#include <functional>
#include <math.h>

#include <glad/glad.h>
#include "imgui.h"
#include <glm/gtc/quaternion.hpp>
#ifdef TRACY_ENABLE
#include <Tracy.hpp>
#include <TracyOpenGL.hpp>
#endif//!TRACY_ENABLE

#include "engine.h"
#include "model.h"
#include "framebuffer.h"
#include "skybox.h"
#include "resource_manager.h"

// TODO: see what the nsight errors are all about
// TODO: see what the camera fuckery is all about
// TODO: investigate unnecessary binding of transform models?
// TODO: add anti-aliasing, investigate origin of sharp transitions in textures
// TODO: profile framebuffers

// Q: @Elias: why is my vec4 framebuffer data getting.... perspective devided?... or something else?

namespace gl
{
    const bool CONTROL_CAMERA = true;
    const glm::vec3 CAMERA_STARTING_POS = UP_VEC3;
    const glm::vec3 CAMERA_STARTING_FRONT = CAMERA_STARTING_POS + FRONT_VEC3;

    const glm::vec3 HORSE_POS =
        RIGHT_VEC3 *  6.0f +
        UP_VEC3 *     2.0f +
        FRONT_VEC3 * -8.0f;
    const glm::vec3 HORSE_SIZE =
        RIGHT_VEC3 * 5.0f +
        UP_VEC3 *    5.0f +
        FRONT_VEC3 * 5.0f;

    const size_t NR_OF_PARTICLES = 512;
    const float PARTICLE_SPACING = 0.5f;
    const float EMITTER_RADIUS = 2.5f;
    const glm::vec3 EMITTER_POS =
        RIGHT_VEC3 * 4.0f +
        UP_VEC3 *    0.0f +
        FRONT_VEC3 * -20.0f;

    const glm::vec3 DIAMOND_POS =
        RIGHT_VEC3 * 5.0f +
        UP_VEC3 * 1.0f +
        FRONT_VEC3 * 0.0f;

    // Regions.
    const float TURN_AROUND_START = 0.1f;
    const float TURN_AROUND_END = 2.0f;
    const float TURN_TOWARDS_HORSE_END = 3.0f;
    const float FOLLOW_HORSE_END = 13.0f;
    const float TURN_TOWARDS_PARTICLES_END = 15.0f;
    const float FOLLOW_PARTICLES_END = 25.0f;

    float RemapToRange(const float inputRangeLower, const float inputRangeUpper, const float outputRangeLower, const float outputRangeUpper, const float value)
    {
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
#ifdef TRACY_ENABLE
            ZoneNamedN(demoInit, "Demo::Init()", true);
            TracyGpuNamedZone(gpuDemoInit, "Demo::Init()", true);
#endif

            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            const std::string assetsPath = "C:/Users/admin/Desktop/demoAssets/";

            // Load floor.
            {
                const auto objData = ResourceManager::ReadObj(assetsPath + "models/floor/floor.obj");
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
                matdef.shader.dynamicMat4s.insert({ CAMERA_MARIX_NAME, resourceManager_.GetCamera().GetCameraMatrixPtr() });

                const float scale = 5.0f;
                const size_t roadLength = 5;
                std::vector<glm::mat4> modelMatrices = std::vector<glm::mat4>(roadLength);
                for (size_t i = 0; i < roadLength; i++)
                {
                    modelMatrices[i] = glm::translate(IDENTITY_MAT4, (float)i * BACK_VEC3 * 2.0f * scale);
                    modelMatrices[i] = glm::scale(modelMatrices[i], ONE_VEC3 * scale);
                }

                floor_.Create({ vbdef }, {matdef}, modelMatrices);
            }

            // Load horse.
            {
                const auto objData0 = ResourceManager::ReadObj(assetsPath + "models/horse/horse_base.obj", false);
                const auto objData1 = ResourceManager::ReadObj(assetsPath + "models/horse/horse_sphere.obj", false);
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
                matdef.shader.dynamicMat4s.insert({ CAMERA_MARIX_NAME, resourceManager_.GetCamera().GetCameraMatrixPtr() });
                matdef.shader.dynamicFloats.insert({"interpolationFactor", &morphingFactor_});

                std::vector<glm::mat4> modelMatrices;
                modelMatrices.push_back(glm::translate(IDENTITY_MAT4, HORSE_POS));
                modelMatrices[0] = glm::rotate(modelMatrices[0], glm::radians(-90.0f), UP_VEC3);
                modelMatrices[0] = glm::scale(modelMatrices[0], glm::vec3(HORSE_SIZE));

                horse_.Create({ vbdef }, { matdef }, modelMatrices, 7); // TODO: remove the model matrix offset argument?
            }

            // Load particle model.
            {
                const auto objData = ResourceManager::ReadObj(assetsPath + "models/particle/particle.obj");
                VertexBuffer::Definition vbdef;
                for (size_t i = 0; i < objData[0].positions.size(); i++)
                {
                    vbdef.data.push_back(objData[0].positions[i].x);
                    vbdef.data.push_back(objData[0].positions[i].y);
                    vbdef.data.push_back(objData[0].positions[i].z);

                    vbdef.data.push_back(objData[0].uvs[i].x);
                    vbdef.data.push_back(objData[0].uvs[i].y);
                }
                for (size_t i = 0; i < objData[1].positions.size(); i++)
                {
                    vbdef.data.push_back(objData[1].positions[i].x);
                    vbdef.data.push_back(objData[1].positions[i].y);
                    vbdef.data.push_back(objData[1].positions[i].z);

                    vbdef.data.push_back(objData[1].uvs[i].x);
                    vbdef.data.push_back(objData[1].uvs[i].y);
                }
                for (size_t i = 0; i < objData[2].positions.size(); i++)
                {
                    vbdef.data.push_back(objData[2].positions[i].x);
                    vbdef.data.push_back(objData[2].positions[i].y);
                    vbdef.data.push_back(objData[2].positions[i].z);

                    vbdef.data.push_back(objData[2].uvs[i].x);
                    vbdef.data.push_back(objData[2].uvs[i].y);
                }
                vbdef.dataLayout = { 3,2 };
                particleVertexBuffer_.Create(vbdef);

                Material::Definition matdef = ResourceManager::PreprocessMaterialData(objData)[0];
                matdef.shader.vertexPath = "../data/shaders/particles.vert";
                matdef.shader.fragmentPath = "../data/shaders/particles.frag";
                matdef.shader.dynamicMat4s.insert({ CAMERA_MARIX_NAME, resourceManager_.GetCamera().GetCameraMatrixPtr() });
                particleMaterial_.Create(matdef);

                glBindVertexArray(particleVertexBuffer_.GetVAOandVBO()[0]);
                CheckGlError();
                glGenBuffers(1, &particleModelsVBO_);
                glBindBuffer(GL_ARRAY_BUFFER, particleModelsVBO_);
                glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * NR_OF_PARTICLES, particlePositions_, GL_DYNAMIC_DRAW);
                CheckGlError();
                glEnableVertexAttribArray(2);
                glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
                glVertexAttribDivisor(2, 1);
                CheckGlError();
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
            }

            // Load diamond model.
            {
                const auto objData = ResourceManager::ReadObj(assetsPath + "models/diamond/diamond.obj");
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
                matdef.shader.vertexPath = "../data/shaders/diamond.vert";
                matdef.shader.fragmentPath = "../data/shaders/diamond.frag";
                matdef.shader.dynamicMat4s.insert({CAMERA_MARIX_NAME, camera_.GetCameraMatrixPtr()});
                matdef.shader.staticInts.insert({CUBEMAP_SAMPLER_NAME, CUBEMAP_TEXTURE_UNIT});
                matdef.shader.dynamicVec3s.insert({VIEW_POSITION_NAME, camera_.GetPositionPtr()});
                matdef.texturePathsAndTypes.push_back({assetsPath + "textures/skybox/skybox.ktx", Texture::Type::CUBEMAP});

                diamond_.Create({ vbdef }, { matdef }, {glm::translate(IDENTITY_MAT4, DIAMOND_POS)});
            }

            // Init skybox.
            Skybox::Definition skdef;
            skdef.path = assetsPath + "textures/skybox/skybox.ktx";
            skdef.shader.vertexPath = "../data/shaders/skybox.vert";
            skdef.shader.fragmentPath = "../data/shaders/skybox.frag";
            skdef.shader.staticMat4s.insert({PROJECTION_MARIX_NAME, PERSPECTIVE});
            skdef.shader.dynamicMat4s.insert({VIEW_MARIX_NAME, camera_.GetViewMatrixPtr()});
            skdef.shader.staticInts.insert({CUBEMAP_SAMPLER_NAME, CUBEMAP_TEXTURE_UNIT});
            skybox_.Create(skdef);

            // Init frambuffer.
            {
                Framebuffer::Definition fbdef;
                fbdef.type = (Framebuffer::Type) // TODO: add a specular color intensity, floor is too shiny, maybe add a noise to the specular?
                    (
                        Framebuffer::Type::FBO_RGBA0 | // Albedo
                        Framebuffer::Type::FBO_RGBA1 | // Positions( + shadowmap value?)
                        Framebuffer::Type::FBO_RGBA2 | // Normals
                        Framebuffer::Type::FBO_RGBA3 | // shininess
                        Framebuffer::Type::RBO
                    );
                deferredFb_.Create(fbdef);
                fbdef.type = (Framebuffer::Type)
                    (
                        Framebuffer::Type::FBO_RGBA0 | // Color.
                        Framebuffer::Type::FBO_RGBA1   // Bright colors for bloom.
                    );
                postprocessFb_.Create(fbdef);

                // Init fbQuad_.
                VertexBuffer::Definition vbdef;
                vbdef.data = QUAD_POSITIONS;
                vbdef.dataLayout = {2};
                vbdef.generateBoundingSphereRadius = false; // This is a 2D object. No frustum culling.

                Material::Definition matdef;
                matdef.shader.vertexPath = "../data/shaders/deferred_fb.vert";
                matdef.shader.fragmentPath = "../data/shaders/deferred_fb.frag";
                matdef.shader.staticInts.insert({FRAMEBUFFER_SAMPLER0_NAME, FRAMEBUFFER_TEXTURE0_UNIT}); // Albedo + shininess
                matdef.shader.staticInts.insert({FRAMEBUFFER_SAMPLER1_NAME, FRAMEBUFFER_TEXTURE1_UNIT}); // FragPos( + shadowmap val?)
                matdef.shader.staticInts.insert({FRAMEBUFFER_SAMPLER2_NAME, FRAMEBUFFER_TEXTURE2_UNIT}); // normals
                matdef.shader.staticInts.insert({FRAMEBUFFER_SAMPLER3_NAME, FRAMEBUFFER_TEXTURE3_UNIT}); // shininess
                matdef.shader.dynamicVec3s.insert({VIEW_POSITION_NAME, camera_.GetPositionPtr()});

                fbQuad_.Create({ vbdef }, {matdef});

                // Init post process shader.
                Shader::Definition sdef;
                sdef.vertexPath = "../data/shaders/postprocess_fb.vert";
                sdef.fragmentPath = "../data/shaders/postprocess_fb.frag";
                sdef.staticInts.insert({FRAMEBUFFER_SAMPLER0_NAME, FRAMEBUFFER_TEXTURE0_UNIT});
                sdef.staticInts.insert({FRAMEBUFFER_SAMPLER1_NAME, FRAMEBUFFER_TEXTURE1_UNIT});

                postprocessShader_.Create(sdef);
            }

            camera_.SetPosition(CAMERA_STARTING_POS);
            camera_.LookAt(CAMERA_STARTING_FRONT, UP_VEC3);

            // Define camera movements.
            regions_.push_back(Region(TURN_AROUND_START, TURN_AROUND_END, [this](const float start, const float end)->void
            {
                // Turn around.
                const float current = RemapToRange(start, end, 0.0f, 1.0f, timer_);
                const float currentAngle = std::lerp(glm::radians(180.0f), glm::radians(0.0f), current);
                const glm::quat currentRotation = glm::angleAxis(currentAngle, UP_VEC3);
                const glm::vec3 newFront = currentRotation * BACK_VEC3;
                camera_.LookAt(camera_.GetPosition() + newFront, UP_VEC3);
            }));
            regions_.push_back(Region(TURN_AROUND_END, TURN_TOWARDS_HORSE_END, [this](const float start, const float end)->void
            {
                // Turn towards horse.
                const float current = RemapToRange(start, end, 0.0f, 1.0f, timer_);
                const glm::quat startingRot = glm::angleAxis(glm::radians(0.0f), UP_VEC3);
                const glm::quat endingRot = glm::quatLookAt(glm::normalize(HORSE_POS - camera_.GetPosition()), UP_VEC3);
                const glm::quat interpolatedRot = glm::lerp(startingRot, endingRot, current);
                const glm::vec3 newFront = interpolatedRot * BACK_VEC3;
                camera_.LookAt(camera_.GetPosition() + newFront, UP_VEC3);
            }));
            regions_.push_back(Region(TURN_TOWARDS_HORSE_END, FOLLOW_HORSE_END, [this](const float start, const float end)->void
            {
                // Look at horse.
                const float current = RemapToRange(start, end, 0.0f, 1.0f, timer_);
                const glm::vec3 newFront = glm::normalize(HORSE_POS - camera_.GetPosition());
                camera_.LookAt(camera_.GetPosition() + newFront, UP_VEC3);
            }));
            regions_.push_back(Region(FOLLOW_HORSE_END, TURN_TOWARDS_PARTICLES_END, [this](const float start, const float end)->void
            {
                // Turn towards particles.
                const float current = RemapToRange(start, end, 0.0f, 1.0f, timer_);
                const glm::quat startingRot = glm::quatLookAt(glm::normalize(HORSE_POS - camera_.GetPosition()), UP_VEC3);
                const glm::quat endingRot = glm::quatLookAt(glm::normalize(EMITTER_POS + UP_VEC3 * 1.5f - camera_.GetPosition()), UP_VEC3);
                const glm::quat interpolatedRot = glm::slerp(startingRot, endingRot, current);
                const glm::vec3 newFront = interpolatedRot * BACK_VEC3;
                camera_.LookAt(camera_.GetPosition() + newFront, UP_VEC3);
            }));
            regions_.push_back(Region(TURN_TOWARDS_PARTICLES_END, FOLLOW_PARTICLES_END, [this](const float start, const float end)->void
            {
                // Look at particles.
                const float current = RemapToRange(start, end, 0.0f, 1.0f, timer_);
                const glm::vec3 newFront = glm::normalize(EMITTER_POS + UP_VEC3 * 1.5f - camera_.GetPosition());
                camera_.LookAt(camera_.GetPosition() + newFront, UP_VEC3);
            }));
        }
        void Update(seconds dt) override
        {
#ifdef TRACY_ENABLE
            ZoneNamedN(demoUpdate, "Demo::Update()", true);
            TracyGpuNamedZone(gpuDemoUpdate, "Demo::Update()", true);
#endif

            const float fdt = dt.count();
            if (fdt > SKIP_FRAME_THRESHOLD_) return;
            timer_ += fdt;

            morphingFactor_ = glm::cos(timer_) * 0.5f + 0.5f;

            { // Update particles positions.
#ifdef TRACY_ENABLE
                ZoneNamedN(demoUpdateParticlesUpdateParticlesPos, "Demo::Update(): UpdateParticlesPos", true);
#endif
                for (size_t i = 0; i < NR_OF_PARTICLES; i++)
                {
                    float uselessIntegerPart; // Mandatory out argument for std::modf().
                    const float elementOffset = (PARTICLE_SPACING * i) / NR_OF_PARTICLES;
                    const float linearVal = RemapToRange(0.0f, 1.0f, 0.0f, PI, std::modf(timer_ + elementOffset, &uselessIntegerPart)); // Variable that increments linearly between 0 and PI.
                    const float lastLinearVal = RemapToRange(0.0f, 1.0f, 0.0f, PI, std::modf((timer_ - fdt) + elementOffset, &uselessIntegerPart));
                    if (linearVal < lastLinearVal) // Means we've wrapped around. Went from approaching PI to leaving 0.
                    {
                        // Define new direction in which the particle will fly away from emitter for the next sinusoidal half-period.
                        particleXzPositions_[i].x = (rand() % 200 - 100) * 0.01f;
                        particleXzPositions_[i].y = (rand() % 200 - 100) * 0.01f;
                    }
             
                    const float yVal = (linearVal < PI * 0.5f) ? glm::sin(linearVal) : glm::sin(linearVal - PI * 0.5f); // Particles move from emitter and outwards only.
                    const glm::vec3 position =
                    {
                        EMITTER_POS.x + (particleXzPositions_[i].x * yVal * yVal), // *yval^2 to give the particles trumpet like paths rather than straight ones.
                        EMITTER_POS.y + (yVal * EMITTER_RADIUS),
                        EMITTER_POS.z + (particleXzPositions_[i].y * yVal * yVal)
                    };
                    particlePositions_[i].x = position.x;
                    particlePositions_[i].y = position.y;
                    particlePositions_[i].z = position.z;
                }
            }

            // Update camera position and rotation.
            if (!CONTROL_CAMERA)
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

            // Clear screen.
            glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            diamond_.Rotate(glm::vec3(0.0f, glm::radians(90.0f) * fdt, 0.0f));

            deferredFb_.Bind();
            diamond_.Draw();
            // TODO: CHANGE ARGUMENTS to prevent accidental casting of size_t to bool -_-
            horse_.Draw();
            floor_.Draw();
            { // Draw particles.
#ifdef TRACY_ENABLE
                ZoneNamedN(demoUpdateDrawParticles, "Demo::Update(): DrawParticles", true);
                TracyGpuNamedZone(gpudemoUpdateDrawParticles, "Demo::Update(): DrawParticles", true);
#endif
                glDisable(GL_CULL_FACE); // We want to draw all 3 quads composing the particle, even if they're facing away.
                {
#ifdef TRACY_ENABLE
                    ZoneNamedN(demoUpdateParticlesUploadParticlesPos, "Demo::Update(): UploadParticlesPos", true);
                    TracyGpuNamedZone(gpudemoUpdateUploadParticlesPos, "Demo::Update(): UploadParticlesPos", true);
#endif
                    const auto vaoAndVbo = particleVertexBuffer_.GetVAOandVBO();
                    glBindVertexArray(vaoAndVbo[0]);
                    glBindBuffer(GL_ARRAY_BUFFER, particleModelsVBO_);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * NR_OF_PARTICLES, particlePositions_);
                }
                particleMaterial_.Bind();
                particleVertexBuffer_.Draw(NR_OF_PARTICLES);
                particleMaterial_.Unbind();
                glEnable(GL_CULL_FACE);
            }
            skybox_.Draw();
            deferredFb_.Unbind();

            postprocessFb_.Bind();
            deferredFb_.BindGBuffer();
            fbQuad_.Draw(true);
            deferredFb_.UnbindGBuffer();
            postprocessFb_.Unbind();

            postprocessFb_.BindGBuffer();
            fbQuad_.DrawUsingShader(postprocessShader_);
            postprocessFb_.UnbindGBuffer();
        }
        void Destroy() override
        {
            delete particlePositions_;
            delete particleXzPositions_;
            ResourceManager::Get().Shutdown();
        }
        void OnEvent(SDL_Event& event) override
        {
#ifdef TRACY_ENABLE
            ZoneNamedN(demoEvent, "Demo::OnEvent()", true);
            TracyGpuNamedZone(gpuDemoEvent, "Demo::OnEvent()", true);
#endif
            if ((event.type == SDL_KEYDOWN) &&
                (event.key.keysym.sym == SDLK_ESCAPE))
            {
                exit(0);
            }

            if (!CONTROL_CAMERA) return;

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
        const float SKIP_FRAME_THRESHOLD_ = 1.0f / 30.0f; // Prevents the camera from jumping forward suddently if there's a lag spike like when we start the program.

        std::vector<Region> regions_;

        float morphingFactor_ = 0.0f;

        unsigned int particleModelsVBO_ = 0;
        glm::vec3* particlePositions_ = new glm::vec3[NR_OF_PARTICLES];
        glm::vec2* particleXzPositions_ = new glm::vec2[NR_OF_PARTICLES];
        VertexBuffer particleVertexBuffer_;
        Material particleMaterial_;

        Skybox skybox_;

        Model
            floor_,
            horse_,
            diamond_,
            fbQuad_; // TODO: remove default shader for meshes. Shader should be passed as argument on draw.

        Framebuffer deferredFb_, postprocessFb_;
        Shader postprocessShader_; // TODO: get rid of this sheit...
    };

} // End namespace gl.

int main(int argc, char** argv)
{
    gl::Demo program;
    gl::Engine engine(program);
    engine.Run();
    return EXIT_SUCCESS;
}
