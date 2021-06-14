#include "model.h"

#include "resource_manager.h"

void gl::Model::Draw(const Camera& camera) const
{
    ResourceManager& resourceManager = ResourceManager::Get();
    const std::vector<Mesh> meshes = resourceManager.GetMeshes(meshes_);
    
    // TODO: make sure there's no problems with using copies of Shader instead of ref
    std::vector<Shader> shaders = std::vector<Shader>(meshes.size());
    for (size_t i = 0; i < shaders.size(); i++)
    {
        shaders[i] = resourceManager.GetShader(resourceManager.GetMaterial(meshes[i].GetMaterialId()).GetShaderId());
        shaders[i].Bind();
        shaders[i].OnDraw(*this, camera);
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
std::vector<gl::ShaderId> gl::Model::GetShaderIds() const
{
    const gl::ResourceManager resourceManager = gl::ResourceManager::Get();
    const std::vector<Mesh> meshes = resourceManager.GetMeshes(meshes_);
    auto returnVal = std::vector<ShaderId>(meshes.size());
    for (size_t i = 0; i < meshes.size(); i++)
    {
        returnVal[i] = resourceManager.GetMaterial(meshes[i].GetMaterialId()).GetShaderId();
    }
    return returnVal;
}