#include "model.h"

#include "resource_manager.h"

void gl::Model::Draw(CameraId id) const
{
    ResourceManager& resourceManager = ResourceManager::Get();
    const std::vector<Mesh> meshes = resourceManager.GetMeshes(meshes_);
    
    std::vector<Shader> shaders = std::vector<Shader>(meshes.size());
    for (size_t i = 0; i < shaders.size(); i++)
    {
        shaders[i] = resourceManager.GetShader(resourceManager.GetMaterial(meshes[i].GetMaterialId()).GetShaderId());
        shaders[i].Bind();
        shaders[i].OnDraw(*this, resourceManager.GetCamera(id));
    }

    for (size_t i = 0; i < meshes.size(); i++)
    {
        shaders[i].Bind();
        meshes[i].Draw();
    }
    Shader::Unbind();
}
const glm::mat4& gl::Model::GetModelMatrix() const
{
    return gl::ResourceManager::Get().GetTransform(transform_).GetModelMatrix();
}
gl::Transform3dId gl::Model::GetTransform() const
{
    return transform_;
}
std::vector<gl::ShaderId> gl::Model::GetShaderIds() const
{
    const gl::ResourceManager& resourceManager = gl::ResourceManager::Get();
    const std::vector<Mesh> meshes = resourceManager.GetMeshes(meshes_);
    auto returnVal = std::vector<ShaderId>(meshes.size());
    for (size_t i = 0; i < meshes.size(); i++)
    {
        returnVal[i] = resourceManager.GetMaterial(meshes[i].GetMaterialId()).GetShaderId();
    }
    return returnVal;
}

std::vector<gl::MeshId> gl::Model::GetMeshIds() const
{
    return meshes_;
}
