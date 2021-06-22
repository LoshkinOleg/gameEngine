#include "mesh.h"

#include "resource_manager.h"

void gl::Mesh::Draw() const
{
    gl::ResourceManager& rm = gl::ResourceManager::Get();
    const gl::VertexBuffer& vertexBuffer = rm.GetVertexBuffer(vertexBuffer_);
    const gl::Material& material = rm.GetMaterial(material_);

    material.Bind();
    vertexBuffer.Draw();
    material.Unbind();
}