#include <glad/glad.h>

#include "engine.h"
#include "resource_manager.h"

namespace gl {

class HelloNormalmapping : public Program
{
public:
    void Init() override
    {
        glEnable(GL_DEPTH_TEST);
        // glEnable(GL_CULL_FACE);
        glEnable(GL_FRAMEBUFFER_SRGB_EXT);

        {
            ResourceManager::CameraDefinition def;
            camera_ = resourceManager_.CreateResource(def);
        }

        // Load a crate.
        {
            // vbo
            VertexBufferId vb = DEFAULT_ID;
            {
                // positions
                glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
                glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
                glm::vec3 pos3(1.0f, -1.0f, 0.0f);
                glm::vec3 pos4(1.0f, 1.0f, 0.0f);
                // texture coordinates
                glm::vec2 uv1(0.0f, 1.0f);
                glm::vec2 uv2(0.0f, 0.0f);
                glm::vec2 uv3(1.0f, 0.0f);
                glm::vec2 uv4(1.0f, 1.0f);
                // normal vector
                glm::vec3 nm(0.0f, 0.0f, 1.0f);

                // calculate tangent/bitangent vectors of both triangles
                glm::vec3 tangent1, bitangent1;
                glm::vec3 tangent2, bitangent2;
                // triangle 1
                // ----------
                glm::vec3 edge1 = pos2 - pos1;
                glm::vec3 edge2 = pos3 - pos1;
                glm::vec2 deltaUV1 = uv2 - uv1;
                glm::vec2 deltaUV2 = uv3 - uv1;

                float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

                tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
                tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
                tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

                bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
                bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
                bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

                // triangle 2
                // ----------
                edge1 = pos3 - pos1;
                edge2 = pos4 - pos1;
                deltaUV1 = uv3 - uv1;
                deltaUV2 = uv4 - uv1;

                f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

                tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
                tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
                tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);


                bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
                bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
                bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

                ResourceManager::VertexBufferDefinition def;
                def.data =
                {
                    pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
                    pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
                    pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

                    pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
                    pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
                    pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
                };
                def.dataLayout =
                {
                    ResourceManager::VertexDataTypes::POSITIONS_3D,
                    ResourceManager::VertexDataTypes::POSITIONS_3D,
                    ResourceManager::VertexDataTypes::POSITIONS_2D,
                    ResourceManager::VertexDataTypes::POSITIONS_3D,
                    ResourceManager::VertexDataTypes::POSITIONS_3D
                };
                vb = resourceManager_.CreateResource(def);
            }
            // tex
            TextureId diffuseTex = DEFAULT_ID;
            {
                ResourceManager::TextureDefinition def;
                def.correctGamma = true;
                def.flipImage = false;
                def.hdr = false;
                def.paths =
                {
                    "../data/textures/brickwall.jpg"
                };
                def.samplerTextureUnitPair =
                {
                    "diffuseMap", 1
                };
                def.textureType = DEFAULT_TEX_TYPE;
                diffuseTex = resourceManager_.CreateResource(def);
            }
            TextureId normalTex = DEFAULT_ID;
            {
                ResourceManager::TextureDefinition def;
                def.correctGamma = false;
                def.flipImage = false;
                def.hdr = false;
                def.paths =
                {
                    "../data/textures/brickwall_normal.jpg"
                };
                def.samplerTextureUnitPair =
                {
                    "normalMap", 4
                };
                def.textureType = DEFAULT_TEX_TYPE;
                normalTex = resourceManager_.CreateResource(def);
            }
            // shad
            ShaderId sh = DEFAULT_ID;
            {
                ResourceManager::ShaderDefinition def;
                def.vertexPath = "../data/shaders/hello_normalmapping.vert";
                def.fragmentPath = "../data/shaders/hello_normalmapping.frag";
                def.onInit = [](Shader& shader, const Model& model)->void
                {
                    shader.SetProjectionMatrix(PERSPECTIVE);
                    shader.SetVec3("lightPos", glm::vec3(0.5f, 1.0f, 0.3f));
                    shader.SetInt("diffuseMap", 1);
                    shader.SetInt("normalMap", 4);
                };
                def.onDraw = [](Shader& shader, const Model& model, const Camera& camera)->void
                {
                    shader.SetViewMatrix(camera.GetViewMatrix());
                    shader.SetVec3("viewPos", camera.GetPosition());
                    shader.SetModelMatrix(model.GetModelMatrix());
                };
                sh = resourceManager_.CreateResource(def);
            }
            // mat
            MaterialId mat = DEFAULT_ID;
            {
                ResourceManager::MaterialDefinition def;
                def.diffuseColor = ONE_VEC3;
                def.diffuseMap = diffuseTex;
                def.normalMap = normalTex;
                def.shader = sh;
                mat = resourceManager_.CreateResource(def);
            }
            // mesh
            MeshId mesh = DEFAULT_ID;
            {
                ResourceManager::MeshDefinition def;
                def.material = mat;
                def.vertexBuffer = vb;
                mesh = resourceManager_.CreateResource(def);
            }
            // model
            ResourceManager::ModelDefinition def;
            def.meshes =
            {
                mesh
            };
            def.transform = resourceManager_.CreateResource
            (
                ResourceManager::Transform3dDefinition
                {
                    ZERO_VEC3,
                    ZERO_VEC3,
                    ONE_VEC3
                }
            );
            modelPlane_ = resourceManager_.CreateResource(def);
        }
    }
    void Update(seconds dt) override
    {
        timer_ += dt.count();
        glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        resourceManager_.GetModel(modelPlane_).Draw();
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
    CameraId camera_ = DEFAULT_ID;
    ResourceManager& resourceManager_ = ResourceManager::Get();
    ModelId modelPlane_ = DEFAULT_ID;
};

} // End namespace gl.

int main(int argc, char** argv)
{
    gl::HelloNormalmapping program;
    gl::Engine engine(program);
    engine.Run();
    return EXIT_SUCCESS;
}
