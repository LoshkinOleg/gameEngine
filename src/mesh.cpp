#include "mesh.h"

#include "resource_manager.h"

void gl::Mesh::Draw() const
{
    gl::ResourceManager& resourceManager = gl::ResourceManager::Get();
    const gl::VertexBuffer& vertexBuffer = resourceManager.GetVertexBuffer(vertexBuffer_);
    const gl::Material& material = resourceManager.GetMaterial(material_);

    material.Bind();
    vertexBuffer.Draw();
    material.Unbind();
}
gl::MaterialId gl::Mesh::GetMaterialId() const
{
    return material_;
}