// Checklist: use textureType where appropriate, make a separate cubemap class?, make sure there CheckGlError() at key points
// make sure we're using "const auto match" everywhere, not a & version, make sure we're not fucking up where we're passing & in arguments, replace all generic errors with descriptive ones: throw() assert()
// make sure we unbind everything once we're done using it, see where the code could fail for every function
// TODO: rename any <baseClass> variables into <baseClassId> variables where appropriate to better reflect the type of the variable.
// TODO: rename files names to use CamelCase
// TODO: make sure whenever we bind shit, we unbind it afterwards
// TODO: add transparency
// TODO: add frustrum culling
// TODO: handle resizing of window? glViewport and framebuffer too

// TODO: fix multi mesh models'es transformModels

// TODO: hdr bloom

// TODO: try out Gouraud shader
// TODO: test multiple multi mesh models with unbinding enabled

#include <vector>

#include <glad/glad.h>
#include "imgui.h"

#include "engine.h"
#include "model.h"
#include "skybox.h"
#include "framebuffer.h"
#include "resource_manager.h"

namespace gl {

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
            matdef.shader.vertexPath = "../data/shaders/hello_bloom.vert";
            matdef.shader.fragmentPath = "../data/shaders/hello_bloom.frag";
            matdef.shader.staticMat4s.insert({PROJECTION_MARIX_NAME, PERSPECTIVE});
            matdef.shader.dynamicMat4s.insert({VIEW_MARIX_NAME, resourceManager_.GetCamera().GetViewMatrixPtr()});
            matdef.shader.dynamicVec3s.insert({VIEW_POSITION_NAME, resourceManager_.GetCamera().GetPositionPtr()});
            matdef.shader.dynamicFloats.insert({"lightMultiplier", &lightMultiplier_});

            model_.Create({ vbdef }, {matdef});
        }

        // Framebuffer creation.
        Framebuffer::Definition fbdef;
        fbdef.type = (Framebuffer::Type)
            (
                (int)Framebuffer::Type::FBO_RGB0 |
                (int)Framebuffer::Type::FBO_RGB1 |
                (int)Framebuffer::Type::RBO
            );
        fb_.Create(fbdef);

        VertexBuffer::Definition vbdef;
        vbdef.data = QUAD_POSITIONS;
        vbdef.dataLayout = {2};

        Material::Definition matdef;
        matdef.shader.vertexPath = "../data/shaders/hello_bloom_quad.vert";
        matdef.shader.fragmentPath = "../data/shaders/hello_bloom_quad.frag";
        matdef.shader.staticInts.insert({FRAMEBUFFER_SAMPLER0_NAME, FRAMEBUFFER_TEXTURE0_UNIT});
        matdef.shader.staticInts.insert({FRAMEBUFFER_SAMPLER1_NAME, FRAMEBUFFER_TEXTURE1_UNIT});

        displayQuad_.Create({ vbdef }, {matdef});
    }
    void Update(seconds dt) override
    {
        timer_ += dt.count();
        glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        fb_.Bind();
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
        ImGui::Begin("Light settings.");
        ImGui::InputFloat("Light Multiplier", &lightMultiplier_);
        ImGui::End();
    }

private:
    float timer_ = 0.0f;
    bool mouseButtonDown_ = false;
    float lightMultiplier_ = 1.0f;
    Model model_, displayQuad_;
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
