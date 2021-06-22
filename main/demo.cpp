#include <iostream>

#include <glad/glad.h>
#include "imgui.h"

#include "engine.h"
#include "resource_manager.h"

// TODO: asd
// spaceship flight: controls infobox
// obj loading: input box for file path
// transform operations: input box for move, rotate, scale
// hdr filter: on/off switch
// ambient occlusion: on/off
// post processing: dropbox to select filter
// gamma correction: dropbox to select filter
// phong lighting: checkboxes to select lights
// blending: input to rearrange ordering of objects, make one red, one green and one blue

// std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": " << std::endl;
// abort();

namespace gl {

    class Demo : public Program
    {
    public:
        enum class Scene : int
        {
            SPACESHIP_FLIGHT = 0,
            OBJ_LOADING = 1,
            TRANSFORM_OPERATIONS = 2,
            HDR_FILTER = 3,
            SKYBOX_AND_AMBIENTMAP = 4,
            NORMAL_MAPS = 5,
            AMBIENT_OCCLUSION = 6,
            POST_PROCESSING = 7,
            SHADOWS = 8,
            GPU_INSTANCING = 9,
            GAMMA_CORRECTION = 10,
            BLINN_PHONG_LIGHTING = 11,
            PBR_LIGHTING = 12,
            DEFERRED_RENDERING = 13,
            BLENDING = 14
        };

        void Init() override
        {
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_CULL_FACE);
            glEnable(GL_FRAMEBUFFER_SRGB_EXT);

            {
                ResourceManager::CameraDefinition def;
                camera_ = resourceManager_.CreateResource(def);
            }

            ResetScene((Scene)selectedScene_); // To set the glEnables right for debugging purposes.

            // Obj loading scene.
            {
                std::vector<MeshId> meshes = resourceManager_.LoadObj("../data/models/crate/crate.obj", false);
                ResourceManager::ModelDefinition def;
                def.meshes = meshes;
                def.transform = resourceManager_.CreateResource(ResourceManager::Transform3dDefinition{});
                modelSpaceship_ = resourceManager_.CreateResource(def);
            }

            // Transform scene.
            {
                VertexBufferId vertexBufferId = DEFAULT_ID;
                {
                    ResourceManager::VertexBufferDefinition vbdef;
                    vbdef.data =
                    {
                        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
                         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
                         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
                         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
                        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
                        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

                        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
                         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
                         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
                         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
                        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
                        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

                        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
                        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
                        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
                        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
                        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
                        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

                         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
                         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
                         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
                         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
                         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
                         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

                        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
                         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
                         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
                         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
                        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
                        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

                        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
                         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
                         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
                         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
                        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
                        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
                    };
                    vbdef.dataLayout =
                    {
                        ResourceManager::VertexDataTypes::POSITIONS_3D,
                        ResourceManager::VertexDataTypes::NORMALS,
                        ResourceManager::VertexDataTypes::TEXCOORDS_2D
                    };
                    vertexBufferId = resourceManager_.CreateResource(vbdef);
                }

                TextureId textureId = DEFAULT_ID;
                {
                    ResourceManager::TextureDefinition tdef;
                    tdef.samplerTextureUnitPair = {AMBIENT_SAMPLER_NAME.data(), AMBIENT_TEXTURE_UNIT};
                    tdef.correctGamma = true;
                    tdef.paths =
                    {
                        "../data/textures/crate_diffuse.png"
                    };
                    tdef.textureType = GL_TEXTURE_2D;
                    textureId = resourceManager_.CreateResource(tdef);
                }

                ShaderId shaderId = DEFAULT_ID;
                {
                    ResourceManager::ShaderDefinition sdef;
                    sdef.vertexPath = ILLUM1_SHADER[0];
                    sdef.fragmentPath = ILLUM1_SHADER[1];
                    sdef.onInit = [](Shader& shader, const Model& model)->void
                    {
                        shader.SetProjectionMatrix(PERSPECTIVE);
                        shader.SetVec3(AMBIENT_COLOR_NAME.data(), ONE_VEC3);
                        shader.SetInt(AMBIENT_SAMPLER_NAME.data(), AMBIENT_TEXTURE_UNIT);
                    };
                    sdef.onDraw = [](Shader& shader, const Model& model, const Camera& camera)->void
                    {
                        shader.SetViewMatrix(camera.GetViewMatrix());
                        shader.SetModelMatrix(model.GetModelMatrix());
                    };
                    shaderId = resourceManager_.CreateResource(sdef);
                }

                MaterialId materialId = DEFAULT_ID;
                {
                    ResourceManager::MaterialDefinition matdef;
                    matdef.ambientMap = textureId;
                    matdef.ambientColor = ONE_VEC3;
                    matdef.shader = shaderId;
                    materialId = resourceManager_.CreateResource(matdef);
                }

                MeshId meshId = DEFAULT_ID;
                {
                    ResourceManager::MeshDefinition mdef;
                    mdef.vertexBuffer = vertexBufferId;
                    mdef.material = materialId;
                    meshId = resourceManager_.CreateResource(mdef);
                }

                {
                    ResourceManager::ModelDefinition def;
                    def.meshes =
                    {
                        meshId
                    };
                    def.transform = resourceManager_.CreateResource(ResourceManager::Transform3dDefinition{});
                    modelCrate_ = resourceManager_.CreateResource(def);
                    resourceManager_.GetShader(resourceManager_.GetModel(modelCrate_).GetShaderIds()[0]).OnInit(resourceManager_.GetModel(modelCrate_)); // Call shader's init since we've created it manually above.
                }
            }

            // HDR scene.
            {
                VertexBufferId vertexBufferId = DEFAULT_ID;
                {
                    ResourceManager::VertexBufferDefinition vbdef;
                    vbdef.data = // Same cube as above but with flipped normals.
                    {
                        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
                         0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
                         0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
                         0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
                        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
                        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

                        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
                         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
                         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
                         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
                        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
                        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

                        -0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
                        -0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
                        -0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
                        -0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
                        -0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
                        -0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

                         0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
                         0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
                         0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
                         0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
                         0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
                         0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

                        -0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
                         0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
                         0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
                         0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
                        -0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
                        -0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,

                        -0.5f,  0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
                         0.5f,  0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
                         0.5f,  0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
                         0.5f,  0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
                        -0.5f,  0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
                        -0.5f,  0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f
                    };
                    vbdef.dataLayout =
                    {
                        ResourceManager::VertexDataTypes::POSITIONS_3D,
                        ResourceManager::VertexDataTypes::NORMALS,
                        ResourceManager::VertexDataTypes::TEXCOORDS_2D
                    };
                    vertexBufferId = resourceManager_.CreateResource(vbdef);
                }

                TextureId ambientId = DEFAULT_ID;
                {
                    ResourceManager::TextureDefinition tdef;
                    tdef.samplerTextureUnitPair = { AMBIENT_SAMPLER_NAME.data(), AMBIENT_TEXTURE_UNIT };
                    tdef.correctGamma = true;
                    tdef.paths =
                    {
                        "../data/textures/crate_diffuse.png"
                    };
                    tdef.textureType = GL_TEXTURE_2D;
                    ambientId = resourceManager_.CreateResource(tdef);
                }

                TextureId diffuseId = DEFAULT_ID;
                {
                    ResourceManager::TextureDefinition tdef;
                    tdef.samplerTextureUnitPair = { DIFFUSE_SAMPLER_NAME.data(), DIFFUSE_TEXTURE_UNIT };
                    tdef.correctGamma = true;
                    tdef.paths =
                    {
                        "../data/textures/crate_diffuse.png"
                    };
                    tdef.textureType = GL_TEXTURE_2D;
                    diffuseId = resourceManager_.CreateResource(tdef);
                }

                TextureId specularId = DEFAULT_ID;
                {
                    ResourceManager::TextureDefinition tdef;
                    tdef.samplerTextureUnitPair = { SPECULAR_SAMPLER_NAME.data(), SPECULAR_TEXTURE_UNIT };
                    tdef.correctGamma = false;
                    tdef.paths =
                    {
                        "../data/textures/blank2x2.png"
                    };
                    tdef.textureType = GL_TEXTURE_2D;
                    specularId = resourceManager_.CreateResource(tdef);
                }

                ShaderId shaderId = DEFAULT_ID;
                {
                    ResourceManager::ShaderDefinition sdef;
                    sdef.vertexPath = "../data/shaders/hdr_crate.vert";
                    sdef.fragmentPath = "../data/shaders/hdr_crate.frag";
                    sdef.onInit = [ambientId, diffuseId, specularId](Shader& shader, const Model& model)->void
                    {
                        shader.SetProjectionMatrix(PERSPECTIVE);
                        shader.SetModelMatrix(model.GetModelMatrix());
                        shader.SetTexture(ambientId);
                        shader.SetTexture(diffuseId);
                        shader.SetTexture(specularId);
                        shader.SetVec3(AMBIENT_COLOR_NAME.data(), ONE_VEC3 * 0.1f);
                        shader.SetVec3(DIFFUSE_COLOR_NAME.data(), ONE_VEC3 * 0.9f);
                        shader.SetVec3(SPECULAR_COLOR_NAME.data(), ONE_VEC3);
                        shader.SetFloat(SHININESS_NAME.data(), 1.0f);

                        shader.SetVec3("light0.pos", glm::vec3(0.0f, 0.0f, -24.0f));
                        shader.SetVec3("light0.ambient", ZERO_VEC3);
                        shader.SetVec3("light0.diffuse", ONE_VEC3 * 1000.0f);
                        shader.SetVec3("light0.specular", ZERO_VEC3);
                        shader.SetFloat("light0.constant", 1.0f);
                        shader.SetFloat("light0.linear", 0.35f);
                        shader.SetFloat("light0.quadratic", 0.44f);

                        shader.SetVec3("light1.pos", glm::vec3(0.7f, -0.7f, 3.0));
                        shader.SetVec3("light1.ambient", ZERO_VEC3);
                        shader.SetVec3("light1.diffuse", ONE_VEC3);
                        shader.SetVec3("light1.specular", ZERO_VEC3);
                        shader.SetFloat("light1.constant", 1.0f);
                        shader.SetFloat("light1.linear", 0.7f);
                        shader.SetFloat("light1.quadratic", 1.8f);
                    };
                    sdef.onDraw = [](Shader& shader, const Model& model, const Camera& camera)->void
                    {
                        shader.SetViewMatrix(camera.GetViewMatrix());
                        shader.SetVec3(VIEW_POSITION_NAME.data(), camera.GetPosition());
                    };
                    shaderId = resourceManager_.CreateResource(sdef);
                }

                MaterialId materialId = DEFAULT_ID;
                {
                    ResourceManager::MaterialDefinition matdef;
                    matdef.ambientMap = ambientId;
                    matdef.diffuseMap = diffuseId;
                    matdef.specularMap = specularId;
                    matdef.ambientColor = ONE_VEC3 * 0.1f;
                    matdef.diffuseColor = ONE_VEC3 * 0.9f;
                    matdef.specularColor = ONE_VEC3;
                    matdef.shader = shaderId;
                    materialId = resourceManager_.CreateResource(matdef);
                }

                MeshId meshId = DEFAULT_ID;
                {
                    ResourceManager::MeshDefinition mdef;
                    mdef.material = materialId;
                    mdef.vertexBuffer = vertexBufferId;
                    meshId = resourceManager_.CreateResource(mdef);
                }

                {
                    ResourceManager::ModelDefinition def;
                    def.transform = resourceManager_.CreateResource(
                        ResourceManager::Transform3dDefinition
                        {
                            glm::vec3(0.0f, 0.0f, 0.0f), // position
                            ZERO_VEC3, // rotation
                            glm::vec3(2.0f, 2.0f, 50.0f), // scale
                        });
                    def.meshes =
                    {
                        meshId
                    };
                    modelInvertedCrate_ = resourceManager_.CreateResource(def);
                    // TODO: Get rid of this eldrich horror...
                    resourceManager_.GetShader(resourceManager_.GetModel(modelInvertedCrate_).GetShaderIds()[0]).OnInit(resourceManager_.GetModel(modelInvertedCrate_));
                }
            }

            // Reinhard HDR framebuffer.
            {
                ShaderId shaderId = DEFAULT_ID;
                {
                    ResourceManager::ShaderDefinition sdef;
                    sdef.vertexPath = FRAMEBUFFER_HDR_REINHARD_SHADER[0];
                    sdef.fragmentPath = FRAMEBUFFER_HDR_REINHARD_SHADER[1];
                    sdef.onInit = [](Shader& shader, const Model& model)->void
                    {
                        shader.SetInt(FRAMEBUFFER_SAMPLER_NAME.data(), FRAMEBUFFER_TEXTURE_UNIT);
                    };
                    shaderId = resourceManager_.CreateResource(sdef);
                }

                {
                    ResourceManager::FramebufferDefinition def;
                    def.hdr = true;
                    def.attachments =
                    {
                        Framebuffer::AttachmentType::COLOR
                    };
                    def.shader = shaderId;
                    // TODO: inconsistency in use: don't need to call onInit manually when using a non default shader for framebuffer, but do have to do it for Models!
                    framebufferHdr_ = resourceManager_.CreateResource(def);
                }
            }

            // Skybox and reflection scene.
            {
                ResourceManager::SkyboxDefinition def;
                def.correctGamma = true;
                def.flipImages = false;
                skybox_ = resourceManager_.CreateResource(def);
            }
        }

        void Update(seconds dt) override
        {
            timeSinceStart_ += dt.count();
            glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            switch ((Scene)selectedScene_)
            {
                case Scene::SPACESHIP_FLIGHT:
                    {

                    }break;
                case Scene::OBJ_LOADING:
                    {
                        const Model& model = resourceManager_.GetModel(modelSpaceship_);
                        model.Draw();
                    }break;
                case Scene::TRANSFORM_OPERATIONS:
                    {
                        const Model& crate = resourceManager_.GetModel(modelCrate_);
                        Transform3d& transform = resourceManager_.GetTransform(crate.GetTransform());

                        const float cos = glm::cos(timeSinceStart_);
                        const float sin = glm::sin(timeSinceStart_);
                        transform.SetPosition(glm::vec3(cos, sin, 0.0f));
                        transform.SetRotation(glm::vec3(cos, cos, cos));
                        transform.SetScale(glm::vec3(cos));
                        crate.Draw();
                    }break;
                case Scene::HDR_FILTER:
                    {
                        const Model& model = resourceManager_.GetModel(modelInvertedCrate_);

                        if (enableHdrFilter_)
                        {
                            const Framebuffer& fb = resourceManager_.GetFramebuffer(framebufferHdr_);
                            fb.Bind();
                            model.Draw();
                            fb.UnBind();
                            fb.Draw();
                        }
                        else
                        {
                            model.Draw();
                        }
                    }break;
                case Scene::SKYBOX_AND_AMBIENTMAP:
                    {
                        const Skybox& skybox = resourceManager_.GetSkybox(skybox_);
                        skybox.Draw();
                    }break;
                case Scene::NORMAL_MAPS:
                    {

                    }break;
                case Scene::AMBIENT_OCCLUSION:
                    {

                    }break;
                case Scene::POST_PROCESSING:
                    {

                    }break;
                case Scene::SHADOWS:
                    {

                    }break;
                case Scene::GPU_INSTANCING:
                    {

                    }break;
                case Scene::GAMMA_CORRECTION:
                    {

                    }break;
                case Scene::BLINN_PHONG_LIGHTING:
                    {

                    }break;
                case Scene::PBR_LIGHTING:
                    {

                    }break;
                case Scene::DEFERRED_RENDERING:
                    {

                    }break;
                case Scene::BLENDING:
                    {

                    }break;
                default:
                    {
                        std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": invalid currently selected scene!" << std::endl;
                        abort();
                    }
            }
        }

        void Destroy() override
        {
            resourceManager_.Shutdown();
        }

        void OnEvent(SDL_Event& event) override
        {
            if ((event.type == SDL_KEYDOWN) &&
                (event.key.keysym.sym == SDLK_ESCAPE))
            {
                exit(0);
            }
            
            Camera& camera = resourceManager_.GetCamera(camera_);
            switch (event.type)
            {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_w:
                    camera.ProcessKeyboard(Camera::Camera_Movement::FORWARD);
                    break;
                case SDLK_s:
                    camera.ProcessKeyboard(Camera::Camera_Movement::BACKWARD);
                    break;
                case SDLK_a:
                    camera.ProcessKeyboard(Camera::Camera_Movement::LEFT);
                    break;
                case SDLK_d:
                    camera.ProcessKeyboard(Camera::Camera_Movement::RIGHT);
                    break;
                case SDLK_SPACE:
                    camera.ProcessKeyboard(Camera::Camera_Movement::UP);
                    break;
                case SDLK_LCTRL:
                    camera.ProcessKeyboard(Camera::Camera_Movement::DOWN);
                    break;
                default:
                    break;
                }
                break;
            case SDL_MOUSEMOTION:
                if (mouseButtonDown_) camera.ProcessMouseMovement(event.motion.xrel, event.motion.yrel);
                break;
            case SDL_MOUSEWHEEL:
            
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
            const Scene prevousScene = (Scene)selectedScene_;

            ImGui::Begin("Demo selection");
            const char* const items[15] =
            {
                "Spaceship flight",
                "Obj loading",
                "Transform operations",
                "HDR filter",
                "Skybox and ambientmap",
                "Normalmap",
                "Ambient occlusion",
                "Post processing filter",
                "Direct shadows",
                "GPU instancing",
                "Gamma correction",
                "Bling-Phong rendering",
                "PBR lighting",
                "Deferred rendering",
                "Blending"
            };
            ImGui::ListBox("", &selectedScene_, items, 15);
            ImGui::End();

            switch ((Scene)selectedScene_)
            {
                case Scene::SPACESHIP_FLIGHT:
                    {
                        if ((Scene)selectedScene_ != prevousScene)
                        {
                            ResetScene(Scene::SPACESHIP_FLIGHT);
                        }
                    }break;
                case Scene::OBJ_LOADING:
                    {
                        if ((Scene)selectedScene_ != prevousScene)
                        {
                            ResetScene(Scene::OBJ_LOADING);
                        }
                    }break;
                case Scene::TRANSFORM_OPERATIONS:
                    {
                        if ((Scene)selectedScene_ != prevousScene)
                        {
                            ResetScene(Scene::TRANSFORM_OPERATIONS);
                        }
                    }break;
                case Scene::HDR_FILTER:
                    {
                        if ((Scene)selectedScene_ != prevousScene)
                        {
                            ResetScene(Scene::HDR_FILTER);
                        }

                        ImGui::Begin("Scene options:");
                        ImGui::Checkbox("Reinhard HDR filter", &enableHdrFilter_);
                        ImGui::End();
                    }break;
                case Scene::SKYBOX_AND_AMBIENTMAP:
                    {
                        if ((Scene)selectedScene_ != prevousScene)
                        {
                            ResetScene(Scene::SKYBOX_AND_AMBIENTMAP);
                        }
                    }break;
                case Scene::NORMAL_MAPS:
                    {
                        if ((Scene)selectedScene_ != prevousScene)
                        {
                            ResetScene(Scene::NORMAL_MAPS);
                        }
                    }break;
                case Scene::AMBIENT_OCCLUSION:
                    {
                        if ((Scene)selectedScene_ != prevousScene)
                        {
                            ResetScene(Scene::AMBIENT_OCCLUSION);
                        }
                    }break;
                case Scene::POST_PROCESSING:
                    {
                        if ((Scene)selectedScene_ != prevousScene)
                        {
                            ResetScene(Scene::POST_PROCESSING);
                        }
                    }break;
                case Scene::SHADOWS:
                    {
                        if ((Scene)selectedScene_ != prevousScene)
                        {
                            ResetScene(Scene::SHADOWS);
                        }
                    }break;
                case Scene::GPU_INSTANCING:
                    {
                        if ((Scene)selectedScene_ != prevousScene)
                        {
                            ResetScene(Scene::GPU_INSTANCING);
                        }
                    }break;
                case Scene::GAMMA_CORRECTION:
                    {
                        if ((Scene)selectedScene_ != prevousScene)
                        {
                            ResetScene(Scene::GAMMA_CORRECTION);
                        }
                    }break;
                case Scene::BLINN_PHONG_LIGHTING:
                    {
                        if ((Scene)selectedScene_ != prevousScene)
                        {
                            ResetScene(Scene::BLINN_PHONG_LIGHTING);
                        }
                    }break;
                case Scene::PBR_LIGHTING:
                    {
                        if ((Scene)selectedScene_ != prevousScene)
                        {
                            ResetScene(Scene::PBR_LIGHTING);
                        }
                    }break;
                case Scene::DEFERRED_RENDERING:
                    {
                        if ((Scene)selectedScene_ != prevousScene)
                        {
                            ResetScene(Scene::DEFERRED_RENDERING);
                        }
                    }break;
                case Scene::BLENDING:
                    {
                        if ((Scene)selectedScene_ != prevousScene)
                        {
                            ResetScene(Scene::BLENDING);
                        }
                    }break;
                default:
                    {
                        std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": invalid currently selected scene!" << std::endl;
                        abort();
                    }
            }
        }

    private:
        void ResetScene(const Scene scene)
        {
            resourceManager_.GetCamera(camera_).SetPosition(glm::vec3(0.0f, 0.0f, 10.0f));
            glEnable(GL_CULL_FACE); // Certain scenes disable the back face culling.

            switch (scene)
            {
                case Scene::SPACESHIP_FLIGHT:
                    {
                        
                    }break;
                case Scene::OBJ_LOADING:
                    {
                        
                    }break;
                case Scene::TRANSFORM_OPERATIONS:
                    {
                        glDisable(GL_CULL_FACE); // Don't want the cube to show only the back faces when it gets inverted because of the scale.
                        Transform3d& transform = resourceManager_.GetTransform(resourceManager_.GetModel(modelCrate_).GetTransform());
                        transform.SetPosition(ZERO_VEC3);
                        transform.SetRotation(ZERO_VEC3);
                        transform.SetScale(ONE_VEC3);
                    }break;
                case Scene::HDR_FILTER:
                    {
                        glDisable(GL_CULL_FACE); // Camera will be inside the cube, so we don't want the back face culling to get rid of them.
                        enableHdrFilter_ = false;
                    }break;
                case Scene::SKYBOX_AND_AMBIENTMAP:
                    {

                    }break;
                case Scene::NORMAL_MAPS:
                    {

                    }break;
                case Scene::AMBIENT_OCCLUSION:
                    {

                    }break;
                case Scene::POST_PROCESSING:
                    {

                    }break;
                case Scene::SHADOWS:
                    {

                    }break;
                case Scene::GPU_INSTANCING:
                    {

                    }break;
                case Scene::GAMMA_CORRECTION:
                    {

                    }break;
                case Scene::BLINN_PHONG_LIGHTING:
                    {

                    }break;
                case Scene::PBR_LIGHTING:
                    {

                    }break;
                case Scene::DEFERRED_RENDERING:
                    {

                    }break;
                case Scene::BLENDING:
                    {

                    }break;
                default:
                    {
                        std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": invalid currently selected scene!" << std::endl;
                        abort();
                    }
            }
        }

        int selectedScene_ = (int)Scene::SKYBOX_AND_AMBIENTMAP;
        float timeSinceStart_ = 0.0f;
        bool mouseButtonDown_ = false;

        bool enableHdrFilter_ = false;

        CameraId camera_ = DEFAULT_ID;
        ModelId
            modelSpaceship_ = DEFAULT_ID,
            modelCrate_ = DEFAULT_ID,
            modelInvertedCrate_ = DEFAULT_ID,
            modelReflectiveCrate_ = DEFAULT_ID; // Init this.
        FramebufferId framebufferHdr_ = DEFAULT_ID;
        SkyboxId skybox_ = DEFAULT_ID;

        ResourceManager& resourceManager_ = ResourceManager::Get();
    };

} // End namespace gl.

int main(int argc, char** argv)
{
    gl::Demo program;
    gl::Engine engine(program);
    engine.Run();
    return EXIT_SUCCESS;
}
