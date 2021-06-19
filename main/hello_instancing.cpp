#include <glad/glad.h>

#include "engine.h"
#include "resource_manager.h"

#include "utility.h"

namespace gl {

class HelloInstancing : public Program
{
public:
    void Init() override
    {
        glEnable(GL_DEPTH_TEST);
        // glEnable(GL_CULL_FACE);
        glEnable(GL_FRAMEBUFFER_SRGB_EXT);

        // Camera.
        {
            ResourceManager::CameraDefinition def;
            camera_ = resourceManager_.CreateResource(def);
        }

        // Load a crate.
        {
            // vbo
            VertexBufferId vb = DEFAULT_ID;
            {
                ResourceManager::VertexBufferDefinition def;
                def.data =
                {
                    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
                     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
                     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
                    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

                    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
                     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
                    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
                    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

                    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

                     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

                    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
                     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

                    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
                     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
                    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
                };
                def.dataLayout =
                {
                    ResourceManager::VertexDataTypes::POSITIONS_3D,
                    ResourceManager::VertexDataTypes::TEXCOORDS_2D
                };
                vb = resourceManager_.CreateResource(def);
            }
            // tex
            TextureId tex = DEFAULT_ID;
            {
                ResourceManager::TextureDefinition def;
                def.correctGamma = true;
                def.flipImage = false;
                def.hdr = false;
                def.paths =
                {
                    "../data/textures/crate_diffuse.png"
                };
                def.samplerTextureUnitPair =
                {
                    "texture_diffuse1", DIFFUSE_SAMPLER_TEXTURE_UNIT
                };
                def.textureType = DEFAULT_TEX_TYPE;
                tex = resourceManager_.CreateResource(def);
            }
            // shad
            ShaderId sh = DEFAULT_ID;
            {
                ResourceManager::ShaderDefinition def;
                def.vertexPath = "../data/shaders/hello_instancing.vert";
                def.fragmentPath = "../data/shaders/hello_instancing.frag";
                def.onInit = [](Shader& shader, const Model& model)->void
                {
                    shader.SetProjectionMatrix(PERSPECTIVE);
                    shader.SetInt("texture_diffuse1", DIFFUSE_SAMPLER_TEXTURE_UNIT);
                };
                def.onDraw = [](Shader& shader, const Model& model, const Camera& camera)->void
                {
                    shader.SetViewMatrix(camera.GetViewMatrix());
                };
                sh = resourceManager_.CreateResource(def);
            }
            // mat
            MaterialId mat = DEFAULT_ID;
            {
                ResourceManager::MaterialDefinition def;
                def.diffuseColor = ONE_VEC3;
                def.diffuseMap = tex;
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
            modelCrate_ = resourceManager_.CreateResource(def);

            // Init model matrices.
            {
                modelMatrices = new glm::mat4[amount];
                srand(HASHING_SEED); // initialize random seed	
                float radius = 50.0;
                float offset = 25.0f;
                for (unsigned int i = 0; i < amount; i++)
                {
                    glm::mat4 model = glm::mat4(1.0f);
                    // 1. translation: displace along circle with 'radius' in range [-offset, offset]
                    float angle = (float)i / (float)amount * 360.0f;
                    float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
                    float x = sin(angle) * radius + displacement;
                    displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
                    float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
                    displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
                    float z = cos(angle) * radius + displacement;
                    model = glm::translate(model, glm::vec3(x, y, z));

                    // 2. scale: Scale between 0.05 and 0.25f
                    float scale = (rand() % 100) / 100.0f + 0.05;
                    model = glm::scale(model, glm::vec3(scale));

                    // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
                    float rotAngle = (rand() % 360);
                    model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

                    // 4. now add to list of matrices
                    modelMatrices[i] = model;
                }
            }
            const unsigned int VAO = resourceManager_.GetVertexBuffer(vb).GetVAO();
            glBindVertexArray(VAO);
            CheckGlError(__FILE__, __LINE__);
            glGenBuffers(1, &modelMatricesVBO_);
            CheckGlError(__FILE__, __LINE__);
            glBindBuffer(GL_ARRAY_BUFFER, modelMatricesVBO_);
            CheckGlError(__FILE__, __LINE__);
            glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
            CheckGlError(__FILE__, __LINE__);
            std::size_t vec4Size = sizeof(glm::vec4);
            glEnableVertexAttribArray(3);
            CheckGlError(__FILE__, __LINE__);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
            CheckGlError(__FILE__, __LINE__);
            glEnableVertexAttribArray(4);
            CheckGlError(__FILE__, __LINE__);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
            CheckGlError(__FILE__, __LINE__);
            glEnableVertexAttribArray(5);
            CheckGlError(__FILE__, __LINE__);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
            CheckGlError(__FILE__, __LINE__);
            glEnableVertexAttribArray(6);
            CheckGlError(__FILE__, __LINE__);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));
            CheckGlError(__FILE__, __LINE__);
            
            glVertexAttribDivisor(3, 1);
            CheckGlError(__FILE__, __LINE__);
            glVertexAttribDivisor(4, 1);
            CheckGlError(__FILE__, __LINE__);
            glVertexAttribDivisor(5, 1);
            CheckGlError(__FILE__, __LINE__);
            glVertexAttribDivisor(6, 1);
            CheckGlError(__FILE__, __LINE__);
            
            glBindVertexArray(0);
            CheckGlError(__FILE__, __LINE__);
        }
    }
    void Update(seconds dt) override
    {
        timer_ += dt.count();
        glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        CheckGlError(__FILE__, __LINE__);

        const Model& model = resourceManager_.GetModel(modelCrate_);
        const Mesh& mesh = resourceManager_.GetMesh(model.GetMeshIds()[0]);
        const Material& material = resourceManager_.GetMaterial(mesh.GetMaterialId());
        Shader& shader = resourceManager_.GetShader(material.GetShaderId());
        const VertexBuffer& vb = resourceManager_.GetVertexBuffer(mesh.GetVertexBufferId());
        shader.Bind();
        CheckGlError(__FILE__, __LINE__);
        shader.OnDraw(model, resourceManager_.GetCamera(camera_));
        CheckGlError(__FILE__, __LINE__);
        material.Bind();
        CheckGlError(__FILE__, __LINE__);
        glBindVertexArray(vb.GetVAO());
        CheckGlError(__FILE__, __LINE__);
         glDrawArraysInstanced(GL_TRIANGLES, 0, 36, amount);
        CheckGlError(__FILE__, __LINE__);
        shader.Unbind();
    }
    void Destroy() override
    {
        ResourceManager::Get().Shutdown();
        delete modelMatrices;
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
    glm::mat4* modelMatrices = nullptr;
    ModelId modelCrate_ = DEFAULT_ID;
    unsigned int modelMatricesVBO_ = 0;
    const unsigned int amount = 10000;
};

} // End namespace gl.

int main(int argc, char** argv)
{
    gl::HelloInstancing program;
    gl::Engine engine(program);
    engine.Run();
    return EXIT_SUCCESS;
}
