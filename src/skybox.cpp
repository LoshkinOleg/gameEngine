#include "skybox.h"

#include <glad/glad.h>

#include "resource_manager.h"

void gl::Skybox::Create(Definition def)
{
    if (shader_.GetPROGRAM() != 0)
    {
        EngineError("Calling Create() a second time...");
    }

    ResourceManager& rm = ResourceManager::Get();

    VertexBuffer::Definition vbdef;
    vbdef.data =
    {
        // Vertex positions AND 3d tex coords simulataneously since it's a cube centered on origin.
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    vbdef.dataLayout =
    {
        3 // Pos / texcoords combined.
    };
    vb_.Create(vbdef);

    if (def.shader.vertexPath.empty())
    {
        EngineError("Skybox has no shader!");
    }
    shader_.Create(def.shader);

    cubemap_.Create(Texture::Type::CUBEMAP, def.path);
}

void gl::Skybox::Draw()
{
    glDepthFunc(GL_LEQUAL);
    shader_.Bind();
    cubemap_.Bind();
    vb_.Draw();
    cubemap_.Unbind();
    shader_.Unbind();
    glDepthFunc(GL_LESS);
}