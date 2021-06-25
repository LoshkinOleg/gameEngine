#include "mesh.h"

#include "resource_manager.h"

void gl::Mesh::Create(Definition def)
{
    // TODO: put the transparent meshes into the proper map.

    // Accumulate relevant data into a string for hashing.
    std::string accumulatedData = "";
    accumulatedData += std::to_string(def.material);
    accumulatedData += std::to_string(def.vertexBuffer);

    const XXH32_hash_t hash = XXH32(accumulatedData.c_str(), sizeof(char) * accumulatedData.size(), HASHING_SEED);
    assert(hash != DEFAULT_ID); // We aren't handling this issue...

    if (IsMeshIdValid(hash))
    {
        EngineWarning("WARNING: attempting to create a new Mesh with data identical to an existing one. Returning the id of the existing one instead.");
        return hash;
    }

    Mesh mesh;
    mesh.vertexBuffer_ = def.vertexBuffer;
    mesh.material_ = def.material;

    opaqueMeshes_.insert({ hash, mesh });

    return hash;
}

void gl::Mesh::Draw(int nrOfInstances) const
{
    gl::ResourceManager& rm = gl::ResourceManager::Get();
    const gl::VertexBuffer& vertexBuffer = rm.GetVertexBuffer(vertexBuffer_);
    const gl::Material& material = rm.GetMaterial(material_);

    material.Bind();
    vertexBuffer.Draw(nrOfInstances);
    material.Unbind();
}

gl::MaterialId gl::Mesh::GetMaterialId() const
{
    return material_;
}
