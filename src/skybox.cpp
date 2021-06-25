#include "skybox.h"

#include <glad/glad.h>

#include "mesh.h"
#include "resource_manager.h"

void gl::Skybox::Create(Definition def)
{
    // Accumulate all the relevant data in a single string for hashing.
    std::string accumulatedData = "";
    if (def.paths.size() != 6) EngineError("Skybox::Definition::paths does not contain 6 texture paths!");
    accumulatedData += def.paths[0];
    accumulatedData += def.paths[1];
    accumulatedData += def.paths[2];
    accumulatedData += def.paths[3];
    accumulatedData += def.paths[4];
    accumulatedData += def.paths[5];
    accumulatedData += def.shader[0];
    accumulatedData += def.shader[1];
    accumulatedData += std::to_string(def.correctGamma);
    accumulatedData += std::to_string(def.flipImages);

    const XXH32_hash_t hash = XXH32(accumulatedData.c_str(), sizeof(char) * accumulatedData.size(), HASHING_SEED);
    assert(hash != DEFAULT_ID); // We aren't handling this issue...

    if (IsSkyboxValid(hash))
    {
        EngineWarning("WARNING: attempting to create a new Skybox with data identical to an existing one. Returning the id of the existing one instead.");
        return hash;
    }

    VertexBuffer::Definition vbdef;
    VertexBufferId vbId = DEFAULT_ID;
    Material::Definition matdef;
    MaterialId matId = DEFAULT_ID;
    Mesh::Definition mdef;
    MeshId meshId = DEFAULT_ID;

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
        3 // Pos / texcoords
    };
    vbId = CreateResource(vbdef);

    matdef.vertexPath = def.shader[0];
    matdef.fragmentPath = def.shader[1];
    for (const auto& path : def.paths)
    {
        matdef.texturePathsAndTypes.push_back({ path, Texture2D::Type::CUBEMAP });
    }
    matdef.correctGamma = def.correctGamma;
    matdef.flipImages = def.flipImages;
    matdef.useHdr = false; // No sense in using hdr for a skybox texture.
    matdef.staticInts.insert({ CUBEMAP_SAMPLER_NAME.data(), CUBEMAP_TEXTURE_UNIT });
    matdef.staticMat4s.insert({ PROJECTION_MARIX_NAME, PERSPECTIVE });
    matdef.dynamicMat4s.insert({ VIEW_MARIX_NAME, &cameras_[0].GetViewMatrix() }); // NOTE: this means a skybox can only be related to the default camera!
    matId = CreateResource(matdef);

    mdef.vertexBuffer = vbId;
    mdef.material = matId;
    meshId = CreateResource(mdef);

    Skybox skybox;
    skybox.mesh_ = meshId;

    skyboxes_.insert({ hash, skybox });

    return hash;
}

void gl::Skybox::Draw() const
{
    glDepthFunc(GL_LEQUAL);
    ResourceManager::Get().GetMesh(mesh_).Draw();
    glDepthFunc(GL_LESS);
}