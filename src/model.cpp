#include "model.h"

#include "resource_manager.h"

void gl::Model::Draw() const
{
    ResourceManager& resourceManager = ResourceManager::Get();
    const std::vector<Mesh> meshes = resourceManager.GetMeshes(meshes_);

    for (size_t i = 0; i < meshes.size(); i++)
    {
        meshes[i].Draw();
    }
}

void gl::Model::Translate(glm::vec3 v, size_t modelMatrixIndex)
{
    modelMatrices_[modelMatrixIndex] = glm::translate(modelMatrices_[modelMatrixIndex], v);
}

void gl::Model::Rotate(glm::vec3 cardinalRotation, size_t modelMatrixIndex)
{
    modelMatrices_[modelMatrixIndex] = glm::rotate(modelMatrices_[modelMatrixIndex], cardinalRotation.x, RIGHT_VEC3);
    modelMatrices_[modelMatrixIndex] = glm::rotate(modelMatrices_[modelMatrixIndex], cardinalRotation.y, UP_VEC3);
    modelMatrices_[modelMatrixIndex] = glm::rotate(modelMatrices_[modelMatrixIndex], cardinalRotation.z, FRONT_VEC3);
}

void gl::Model::Scale(glm::vec3 v, size_t modelMatrixIndex)
{
    modelMatrices_[modelMatrixIndex] = glm::scale(modelMatrices_[modelMatrixIndex], v);
}

const std::vector<gl::MeshId> gl::Model::GetMesheIds() const
{
    return meshes_;
}
