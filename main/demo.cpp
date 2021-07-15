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
// TODO: investigate origin of sharp transitions in textures: might be because we're not specifying minification parameters
// TODO: replace particles with little Todd's heads
// TODO: make repo public!!!
// TODO: tidy up the shaders / use generic ones
// TODO: adjust texcoords for cube. They're too close together. Or use a different texture
// TODO: do something about all these constants...
// TODO: move camera movement into Regions' callbacks

namespace gl
{
    const float SKIP_FRAME_THRESHOLD = 1.0f / 30.0f; // Prevents the camera from jumping forward suddently if there's a lag spike like when we start the program.
    const std::string assetsPath = "C:/Users/admin/Desktop/demoAssets/";

    const glm::vec3 LIGHT_DIR = glm::normalize(glm::vec3(1.0, -1.0, -1.0));

    const bool CONTROL_CAMERA = true;
    const glm::vec3 CAMERA_STARTING_POS = UP_VEC3;
    const glm::vec3 CAMERA_STARTING_FRONT = CAMERA_STARTING_POS + FRONT_VEC3;

    const size_t ROAD_LENGTH = 7;

    const glm::vec3 HORSE_POS =
        RIGHT_VEC3 *  6.0f +
        UP_VEC3 *     2.0f +
        FRONT_VEC3 * -10.0f;
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
        RIGHT_VEC3 * 6.0f +
        UP_VEC3 * 1.0f +
        FRONT_VEC3 * -30.0f;

    const glm::vec3 SHADOW_SPHERES_POS =
        RIGHT_VEC3 * 7.0f +
        UP_VEC3 * 1.0f +
        FRONT_VEC3 * -40.0f;
    const float SHADOW_SPHERES_SIZE = 0.5f;
    const glm::mat4 LIGHT_MATRIX = ORTHO * glm::lookAt(SHADOW_SPHERES_POS - LIGHT_DIR * 2.0f, SHADOW_SPHERES_POS, UP_VEC3); // Looking at SHADOW_SPHERES_POS from ~2 units away.

    const glm::vec3 CUBE_POS =
        RIGHT_VEC3 * 6.2f +
        UP_VEC3 * 1.0f +
        FRONT_VEC3 * -50.0f;

    // Regions.
    const float TURN_AROUND_START = 0.1f;
    const float TURN_AROUND_END = 2.0f;
    const float TURN_TOWARDS_HORSE_END = 3.0f;
    const float FOLLOW_HORSE_END = 13.0f;
    const float TURN_TOWARDS_PARTICLES_END = 15.0f;
    const float FOLLOW_PARTICLES_END = 25.0f;

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
    private:
        void InitCube()
        {
            const auto objData = ResourceManager::ReadObj(assetsPath + "models/brickCube/brickCube.obj");
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

            cube_.Create({ vbdef }, { ResourceManager::PreprocessMaterialData(objData)[0] }, { glm::translate(IDENTITY_MAT4, CUBE_POS) });
        }
        void InitSpheres()
        {
            const auto objData = ResourceManager::ReadObj(assetsPath + "models/brickSphere/brickSphere.obj");
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

            Shader::Definition sdef = ResourceManager::PreprocessShaderData(objData)[0];
            sdef.vertexPath = "../data/shaders/sphere.vert";
            sdef.fragmentPath = "../data/shaders/sphere.frag";
            sdef.dynamicMat4s.insert({ CAMERA_MARIX_NAME, resourceManager_.GetCamera().GetCameraMatrixPtr() });
            sdef.staticMat4s.insert({ LIGHT_MATRIX_NAME, LIGHT_MATRIX });
            sdef.staticInts.insert({FRAMEBUFFER_SHADOWMAP_NAME, FRAMEBUFFER_SHADOWMAP_UNIT});
            spheresShader_.Create(sdef);

            sphere_.Create({ vbdef }, { ResourceManager::PreprocessMaterialData(objData)[0] }, {IDENTITY_MAT4, IDENTITY_MAT4, IDENTITY_MAT4});
        }
        void InitDiamond()
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
            matdef.texturePathsAndTypes.push_back({ assetsPath + "textures/skybox/skybox.ktx", Texture::Type::CUBEMAP });
            Shader::Definition sdef = ResourceManager::PreprocessShaderData(objData)[0];
            sdef.vertexPath = "../data/shaders/diamond.vert";
            sdef.fragmentPath = "../data/shaders/diamond.frag";
            sdef.dynamicMat4s.insert({ CAMERA_MARIX_NAME, camera_.GetCameraMatrixPtr() });
            sdef.staticInts.insert({ CUBEMAP_SAMPLER_NAME, CUBEMAP_TEXTURE_UNIT });
            sdef.dynamicVec3s.insert({ VIEW_POSITION_NAME, camera_.GetPositionPtr() });
            diamondShader_.Create(sdef);

            diamond_.Create({ vbdef }, { matdef }, { glm::translate(IDENTITY_MAT4, DIAMOND_POS) });
        }
        void InitParticles()
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
            particleMaterial_.Create(matdef);
            Shader::Definition sdef = ResourceManager::PreprocessShaderData(objData)[0];
            sdef.vertexPath = "../data/shaders/particles.vert";
            sdef.fragmentPath = "../data/shaders/particles.frag";
            sdef.dynamicMat4s.insert({ CAMERA_MARIX_NAME, resourceManager_.GetCamera().GetCameraMatrixPtr() });
            particleShader_.Create(sdef);

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
        void InitHorse()
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
            vbdef.dataLayout = { 3,3,2,3,3,3,3 };

            Shader::Definition sdef = ResourceManager::PreprocessShaderData(objData0)[0];
            sdef.vertexPath = "../data/shaders/horse.vert";
            sdef.fragmentPath = "../data/shaders/horse.frag";
            sdef.dynamicMat4s.insert({ CAMERA_MARIX_NAME, resourceManager_.GetCamera().GetCameraMatrixPtr() });
            sdef.dynamicFloats.insert({ "interpolationFactor", &morphingFactor_ });
            horseShader_.Create(sdef);

            std::vector<glm::mat4> modelMatrices;
            modelMatrices.push_back(glm::translate(IDENTITY_MAT4, HORSE_POS));
            modelMatrices[0] = glm::rotate(modelMatrices[0], glm::radians(-90.0f), UP_VEC3);
            modelMatrices[0] = glm::scale(modelMatrices[0], glm::vec3(HORSE_SIZE));

            horse_.Create({ vbdef }, { ResourceManager::PreprocessMaterialData(objData0)[0] }, modelMatrices, 7); // TODO: remove the model matrix offset argument?
        }
        void InitFloor()
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

            Shader::Definition sdef = ResourceManager::PreprocessShaderData(objData)[0];
            sdef.vertexPath = "../data/shaders/floor.vert";
            sdef.fragmentPath = "../data/shaders/floor.frag";
            sdef.dynamicMat4s.insert({ CAMERA_MARIX_NAME, resourceManager_.GetCamera().GetCameraMatrixPtr() });
            floorShader_.Create(sdef);

            const float scale = 5.0f;
            std::vector<glm::mat4> modelMatrices = std::vector<glm::mat4>(ROAD_LENGTH);
            for (size_t i = 0; i < ROAD_LENGTH; i++)
            {
                modelMatrices[i] = glm::translate(IDENTITY_MAT4, (float)i * BACK_VEC3 * 2.0f * scale);
                modelMatrices[i] = glm::scale(modelMatrices[i], ONE_VEC3 * scale);
            }

            floor_.Create({ vbdef }, { ResourceManager::PreprocessMaterialData(objData)[0] }, modelMatrices);
        }
        void InitModels()
        {
            InitFloor();
            InitHorse();
            InitParticles();
            InitDiamond();
            InitSpheres();
            InitCube();
        }
        void InitFramebuffers()
        {
            Framebuffer::Definition fbdef;
            fbdef.type = (Framebuffer::Type) // TODO: add a specular color intensity, floor is too shiny, maybe add a noise to the specular?
                (
                    Framebuffer::Type::FBO_RGBA0 | // Albedo
                    Framebuffer::Type::FBO_RGBA1 | // Positions
                    Framebuffer::Type::FBO_RGBA2 | // Normals
                    Framebuffer::Type::FBO_RGBA3 | // x: shininess, y: shadowmap, z: Fragpos'es .w component for perspective divide.
                    Framebuffer::Type::RBO
                );
            deferredFb_.Create(fbdef);
            fbdef.type = (Framebuffer::Type)
                (
                    Framebuffer::Type::FBO_RGBA0 | // Color.
                    Framebuffer::Type::FBO_RGBA1   // Bright colors for bloom.
                );
            postprocessFb_.Create(fbdef);
            fbdef.type = (Framebuffer::Type)
                (
                    Framebuffer::Type::FBO_DEPTH_NO_DRAW
                );
            fbdef.resolution = {1024, 1024};
            shadowpassFb_.Create(fbdef);

            // Init fbQuad_.
            VertexBuffer::Definition vbdef;
            vbdef.data = QUAD_POSITIONS;
            vbdef.dataLayout = { 2 };
            vbdef.generateBoundingSphereRadius = false; // This is a 2D object. No frustum culling.

            Shader::Definition sdef;
            sdef.vertexPath = "../data/shaders/deferred_fb.vert";
            sdef.fragmentPath = "../data/shaders/deferred_fb.frag";
            sdef.staticInts.insert({ FRAMEBUFFER_SAMPLER0_NAME, FRAMEBUFFER_TEXTURE0_UNIT }); // Albedo + shininess
            sdef.staticInts.insert({ FRAMEBUFFER_SAMPLER1_NAME, FRAMEBUFFER_TEXTURE1_UNIT }); // FragPos( + shadowmap val?)
            sdef.staticInts.insert({ FRAMEBUFFER_SAMPLER2_NAME, FRAMEBUFFER_TEXTURE2_UNIT }); // normals
            sdef.staticInts.insert({ FRAMEBUFFER_SAMPLER3_NAME, FRAMEBUFFER_TEXTURE3_UNIT }); // shininess
            sdef.dynamicVec3s.insert({ VIEW_POSITION_NAME, camera_.GetPositionPtr() });
            sdef.staticVec3s.insert({"lightDir", LIGHT_DIR});
            deferredShader_.Create(sdef);

            fbQuad_.Create({ vbdef }, { Material::Definition() });

            // Init post process shader.
            sdef = Shader::Definition();
            sdef.vertexPath = "../data/shaders/postprocess_fb.vert";
            sdef.fragmentPath = "../data/shaders/postprocess_fb.frag";
            sdef.staticInts.insert({ FRAMEBUFFER_SAMPLER0_NAME, FRAMEBUFFER_TEXTURE0_UNIT });
            sdef.staticInts.insert({ FRAMEBUFFER_SAMPLER1_NAME, FRAMEBUFFER_TEXTURE1_UNIT });

            postprocessShader_.Create(sdef);

            // Init shadowpass shader.
            sdef = Shader::Definition();
            sdef.vertexPath = "../data/shaders/shadowmapping.vert";
            sdef.fragmentPath = "../data/shaders/empty.frag";
            sdef.staticMat4s.insert({LIGHT_MATRIX_NAME, LIGHT_MATRIX});
            shadowpassShader_.Create(sdef);
        }
        void InitSkybox()
        {
            // Init skybox.
            Skybox::Definition skdef;
            skdef.path = assetsPath + "textures/skybox/skybox.ktx";
            skdef.shader.vertexPath = "../data/shaders/skybox.vert";
            skdef.shader.fragmentPath = "../data/shaders/skybox.frag";
            skdef.shader.staticMat4s.insert({ PROJECTION_MARIX_NAME, PERSPECTIVE });
            skdef.shader.dynamicMat4s.insert({ VIEW_MARIX_NAME, camera_.GetViewMatrixPtr() });
            skdef.shader.staticInts.insert({ CUBEMAP_SAMPLER_NAME, CUBEMAP_TEXTURE_UNIT });
            skybox_.Create(skdef);
        }
        void InitCamera()
        {
            // TODO: figure out the glitchy fuckery that happens when you control the camera yourself. Something to do with the usage of pitch and yaw in the camera I think.
            camera_.SetPosition(CAMERA_STARTING_POS);
            camera_.LookAt(CAMERA_STARTING_FRONT, UP_VEC3);
        }
        void InitCameraMovements()
        {
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
        
        void UpdateParticles(const float dt)
        {
#ifdef TRACY_ENABLE
            ZoneNamedN(demoUpdateParticlesUpdateParticlesPos, "Demo::Update(): UpdateParticlesPos", true);
#endif
            for (size_t i = 0; i < NR_OF_PARTICLES; i++)
            {
                float uselessIntegerPart; // Mandatory out argument for std::modf().
                const float elementOffset = (PARTICLE_SPACING * i) / NR_OF_PARTICLES;
                const float linearVal = RemapToRange(0.0f, 1.0f, 0.0f, PI, std::modf(timer_ + elementOffset, &uselessIntegerPart)); // Variable that increments linearly between 0 and PI.
                const float lastLinearVal = RemapToRange(0.0f, 1.0f, 0.0f, PI, std::modf((timer_ - dt) + elementOffset, &uselessIntegerPart));
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
        void UpdateCamera(const float dt)
        {
            // TODO: move camera movements to regions too?
            if (!CONTROL_CAMERA)
            {
                camera_.SetPosition(camera_.GetPosition() + BACK_VEC3 * dt);

                for (const auto& region : regions_)
                {
                    if (region.IsActive(timer_))
                    {
                        region.Play();
                        break;
                    }
                }
            }
        }
        void UpdateSpheres()
        {
            auto& sphereModels = sphere_.GetModelMatrices();
            sphereModels[0] = glm::translate(IDENTITY_MAT4, SHADOW_SPHERES_POS + glm::vec3(glm::cos(timer_), glm::sin(timer_), 0.0f));
            sphereModels[0] = glm::scale(sphereModels[0], ONE_VEC3 * SHADOW_SPHERES_SIZE);
            sphereModels[1] = glm::translate(IDENTITY_MAT4, SHADOW_SPHERES_POS + glm::vec3(glm::cos(timer_ + 2.0f * PI * 0.33f), 0.0f, glm::sin(timer_ + 2.0f * PI * 0.33f)));
            sphereModels[1] = glm::scale(sphereModels[1], ONE_VEC3 * SHADOW_SPHERES_SIZE);
            sphereModels[2] = glm::translate(IDENTITY_MAT4, SHADOW_SPHERES_POS + glm::vec3(0.0f, glm::sin(timer_ + 2.0f * PI * 0.66f), glm::cos(timer_ + 2.0f * PI * 0.66f)));
            sphereModels[2] = glm::scale(sphereModels[2], ONE_VEC3 * SHADOW_SPHERES_SIZE);
        }

        void RenderParticles()
        {
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
            particleShader_.Bind();
            particleMaterial_.Bind();
            particleVertexBuffer_.Draw(NR_OF_PARTICLES);
            particleMaterial_.Unbind();
            particleShader_.Unbind();
            glEnable(GL_CULL_FACE);
        }
        void Render()
        {
            // Shadow pass.
            {
#ifdef TRACY_ENABLE
                ZoneNamedN(shadowPass, "Shadow Pass", true);
                TracyGpuNamedZone(gpushadowPass, "Shadow Pass", true);
#endif
                glCullFace(GL_FRONT);
                shadowpassFb_.Bind();
                sphere_.Draw(shadowpassShader_, true); // We want shadows to be drawn even if the object itself is out of the frustum.
                shadowpassFb_.Unbind();
                glCullFace(GL_BACK);
            }

            // Fill gbuffer.
            {
#ifdef TRACY_ENABLE
                ZoneNamedN(geometryPass, "Geometry Pass", true);
                TracyGpuNamedZone(gpugeometryPass, "Geometry Pass", true);
#endif
                shadowpassFb_.BindGBuffer();
                deferredFb_.Bind();
                diamond_.Draw(diamondShader_);
                horse_.Draw(horseShader_);
                sphere_.Draw(spheresShader_);
                cube_.Draw(floorShader_); // TODO: use generic shaders
                floor_.Draw(floorShader_);
                RenderParticles();
                skybox_.Draw();
                deferredFb_.Unbind();
                shadowpassFb_.UnbindGBuffer();
            }

            // Apply shading.
            {
#ifdef TRACY_ENABLE
                ZoneNamedN(shadingPass, "Shading Pass", true);
                TracyGpuNamedZone(gpushadingPass, "Shading Pass", true);
#endif
                postprocessFb_.Bind();
                deferredFb_.BindGBuffer();
                fbQuad_.Draw(deferredShader_, true);
                deferredFb_.UnbindGBuffer();
                postprocessFb_.Unbind();
            }

            // Apply post processing and draw to backbuffer.
            {
#ifdef TRACY_ENABLE
                ZoneNamedN(postprocessPass, "Postprocess Pass", true);
                TracyGpuNamedZone(gpupostprocessPass, "Postprocess Pass", true);
#endif
                postprocessFb_.BindGBuffer();
                fbQuad_.Draw(postprocessShader_, true);
                postprocessFb_.UnbindGBuffer();
            }
        }
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

            InitFramebuffers();
            InitSkybox();
            InitModels();
            InitCamera();
            InitCameraMovements();
        }
        void Update(seconds dt) override
        {
#ifdef TRACY_ENABLE
            ZoneNamedN(demoUpdate, "Demo::Update()", true);
            TracyGpuNamedZone(gpuDemoUpdate, "Demo::Update()", true);
#endif
            // Update timer.
            const float fdt = dt.count();
            if (fdt > SKIP_FRAME_THRESHOLD) return; // Skip if the dt was unusually large. Ex: we've resumed execution of program after breakpoint.
            timer_ += fdt;

            // Clear screen.
            glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Update uniforms and transform models.
            morphingFactor_ = glm::cos(timer_) * 0.5f + 0.5f;
            diamond_.Rotate(glm::vec3(0.0f, glm::radians(90.0f) * fdt, 0.0f));
            UpdateParticles(fdt);
            UpdateSpheres();

            UpdateCamera(fdt);

            Render();
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

        Skybox skybox_;
        Model
            floor_,
            horse_,
            diamond_,
            sphere_,
            cube_,
            fbQuad_;
        Framebuffer
            deferredFb_,
            postprocessFb_,
            shadowpassFb_; // TODO: throw error if trying to use a shader that hasn't been .Create()'ed. Throw error when not all uniforms have been initialized
        Shader
            postprocessShader_,
            deferredShader_,
            shadowpassShader_,
            floorShader_,
            horseShader_,
            particleShader_,
            diamondShader_,
            spheresShader_;

        Camera& camera_ = resourceManager_.GetCamera();
        std::vector<Region> regions_;

        // Horse variables.
        float morphingFactor_ = 0.0f;

        // Particles variables.
        unsigned int particleModelsVBO_ = 0;
        glm::vec3* particlePositions_ = new glm::vec3[NR_OF_PARTICLES];
        glm::vec2* particleXzPositions_ = new glm::vec2[NR_OF_PARTICLES];
        VertexBuffer particleVertexBuffer_;
        Material particleMaterial_;
    };

}//!gl

int main(int argc, char** argv)
{
    gl::Demo program;
    gl::Engine engine(program);
    engine.Run();
    return EXIT_SUCCESS;
}
