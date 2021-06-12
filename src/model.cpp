#include "model.h"

#include "resource_manager.h"

void gl::Model::Draw() const
{
    ResourceManager& resourceManager = ResourceManager::Get();
    Shader& shader = resourceManager.GetShader(shader_);
    const std::vector<Mesh> meshes = resourceManager.GetMeshes(meshes_);

    shader.Bind();
    shader.OnDraw(); // TODO: Fix this shit, make on draw of shader happen!
    for (const auto& mesh : meshes)
    {
        mesh.Draw();
    }
    shader.Unbind();
}

const glm::mat4& gl::Model::GetModelMatrix() const
{
    return gl::ResourceManager::Get().GetTransform(transform_).GetModelMatrix();
}