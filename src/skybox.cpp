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
        EngineWarning("Skybox has no shader specified. Using default skybox shader.");
        def.shader.vertexPath = SKYBOX_SHADER[0];
        def.shader.fragmentPath = SKYBOX_SHADER[1];
        def.shader.staticInts.insert({CUBEMAP_SAMPLER_NAME, CUBEMAP_TEXTURE_UNIT});
        def.shader.staticMat4s.insert({PROJECTION_MARIX_NAME, PERSPECTIVE});
        def.shader.dynamicMat4s.insert({VIEW_MARIX_NAME, rm.GetCamera().GetViewMatrixPtr()});
        def.shader.dynamicVec3s.insert({VIEW_POSITION_NAME, rm.GetCamera().GetPositionPtr()});
    }
    shader_.Create(def.shader);

    for (const auto& path : def.paths)
    {
        assert(!path.empty());
    }
    cubemap_.CreateCubemap(def.paths, def.flipImages, def.correctGamma);
}

void gl::Skybox::Draw()
{
    glDepthFunc(GL_LEQUAL);
    shader_.Bind();
    cubemap_.Bind();
    vb_.Draw();
    // cubemap_.Unbind();
    // shader_.Unbind();
    glDepthFunc(GL_LESS);
}