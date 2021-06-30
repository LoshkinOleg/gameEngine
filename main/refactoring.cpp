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

#include <vector>

#include <glad/glad.h>

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
        const auto objData = ResourceManager::ReadObj("../data/models/camera/camera.obj");

        std::vector<VertexBuffer::Definition> vbdefs = std::vector<VertexBuffer::Definition>(objData.size());
        for (size_t mesh = 0; mesh < objData.size(); mesh++)
        {
            for (size_t vertex = 0; vertex < objData[mesh].positions.size(); vertex++)
            {
                vbdefs[mesh].data.push_back(objData[mesh].positions[vertex].x);
                vbdefs[mesh].data.push_back(objData[mesh].positions[vertex].y);
                vbdefs[mesh].data.push_back(objData[mesh].positions[vertex].z);

                vbdefs[mesh].data.push_back(objData[mesh].uvs[vertex].x);
                vbdefs[mesh].data.push_back(objData[mesh].uvs[vertex].y);
                     
                vbdefs[mesh].data.push_back(objData[mesh].normals[vertex].x);
                vbdefs[mesh].data.push_back(objData[mesh].normals[vertex].y);
                vbdefs[mesh].data.push_back(objData[mesh].normals[vertex].z);
                     
                vbdefs[mesh].data.push_back(objData[mesh].tangents[vertex].x);
                vbdefs[mesh].data.push_back(objData[mesh].tangents[vertex].y);
                vbdefs[mesh].data.push_back(objData[mesh].tangents[vertex].z);
            }
            vbdefs[mesh].dataLayout =
            {
                3,2,3,3
            };
        }
        Material::Definition matdef;
        {
            matdef.shader.vertexPath = "../data/shaders/illum2.vert";
            matdef.shader.fragmentPath = "../data/shaders/illum2.frag";
            matdef.shader.staticInts.insert({ ALPHA_SAMPLER_NAME, ALPHA_TEXTURE_UNIT });
            matdef.texturePathsAndTypes.push_back({"../data/textures/blank2x2.jpg", Texture::Type::ALPHA});
            matdef.shader.staticInts.insert({ NORMALMAP_SAMPLER_NAME, NORMALMAP_TEXTURE_UNIT });
            matdef.texturePathsAndTypes.push_back({ "../data/textures/brickwall_normal.jpg", Texture::Type::NORMALMAP });
            matdef.shader.staticInts.insert({ AMBIENT_SAMPLER_NAME, AMBIENT_TEXTURE_UNIT });
            matdef.texturePathsAndTypes.push_back({ "../data/textures/brickwall.jpg", Texture::Type::AMBIENT });
            matdef.shader.staticInts.insert({ DIFFUSE_SAMPLER_NAME, DIFFUSE_TEXTURE_UNIT });
            matdef.texturePathsAndTypes.push_back({ "../data/textures/brickwall.jpg", Texture::Type::DIFFUSE });
            matdef.shader.staticInts.insert({ SPECULAR_SAMPLER_NAME, SPECULAR_TEXTURE_UNIT });
            matdef.texturePathsAndTypes.push_back({ "../data/textures/blank2x2.jpg", Texture::Type::SPECULAR });
            matdef.shader.staticMat4s.insert({ PROJECTION_MARIX_NAME, PERSPECTIVE });
            matdef.shader.dynamicMat4s.insert({ VIEW_MARIX_NAME, resourceManager_.GetCamera().GetViewMatrixPtr() });
            matdef.shader.dynamicVec3s.insert({ VIEW_POSITION_NAME, resourceManager_.GetCamera().GetPositionPtr() });
            matdef.shader.staticFloats.insert({ SHININESS_NAME, 64.0f });
        }

        std::vector<glm::mat4> transformModels;
        transformModels.push_back(glm::translate(IDENTITY_MAT4, glm::vec3(0.0f, 1.0f, 0.0f)));
        transformModels.push_back(glm::translate(IDENTITY_MAT4, glm::vec3(3.0f, 4.0f, 0.0f)));
        transformModels.push_back(glm::translate(IDENTITY_MAT4, glm::vec3(3.0f, 4.0f, -4.0f)));
        model_.Create(vbdefs, std::vector<Material::Definition>(objData.size(), matdef), transformModels);

    }
    void Update(seconds dt) override
    {
        timer_ += dt.count();
        glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        model_.Draw();
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
    Model model_;
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
