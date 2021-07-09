#include "model.h"

#include <glad/glad.h>
#include "tiny_obj_loader.h"
#include "glm/gtc/quaternion.hpp"
#ifdef TRACY_ENABLE
#include <Tracy.hpp>
#include <TracyOpenGL.hpp>
#endif//!TRACY_ENABLE

#include "resource_manager.h"

void gl::Model::Create(std::vector<VertexBuffer::Definition> vb, std::vector<Material::Definition> mat, std::vector<glm::mat4> modelMatrices)
{
    modelMatrices_ = modelMatrices;

    assert(vb.size() == mat.size());

    glGenBuffers(1, &modelMatricesVBO_);
    ResourceManager::Get().AppendNewVBO(modelMatricesVBO_);
    glBindBuffer(GL_ARRAY_BUFFER, modelMatricesVBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * modelMatrices_.size(), &modelMatrices_[0][0], GL_DYNAMIC_DRAW);

    for (size_t i = 0; i < vb.size(); i++)
    {
        meshes_.push_back(Mesh());
        meshes_.back().Create(vb[i], mat[i]);
        CheckGlError();
    }
}

void gl::Model::Draw(bool bypassFrustumCulling, const size_t transformModelOffset)
{
#ifdef TRACY_ENABLE
    ZoneNamedN(modelDraw, "Model::Draw()", true);
    TracyGpuNamedZone(gpumodelDraw, "Model::Draw()", true);
#endif
    if (!bypassFrustumCulling)
    {
        std::vector<glm::mat4> modelMatricesToDraw;

        {
#ifdef TRACY_ENABLE
        ZoneNamedN(modelDrawFrustumCulling, "Model::Draw(): FrustumCulling", true);
#endif
            // Cull off screen meshes.
            const float near = PROJECTION_NEAR;
            const float far = PROJECTION_FAR;
            const float aspect = SCREEN_RESOLUTION[0] / SCREEN_RESOLUTION[1];
            const float fovY = PROJECTION_FOV * 0.5f;
            const float fovX = std::atan(std::tan(PROJECTION_FOV * 0.5f) * aspect); // TODO: look more into this
            const glm::vec3 cameraPos = ResourceManager::Get().GetCamera().GetPosition();
            const glm::vec3 right = ResourceManager::Get().GetCamera().GetRight();
            const glm::vec3 up = ResourceManager::Get().GetCamera().GetUp();
            const glm::vec3 front = ResourceManager::Get().GetCamera().GetFront();

            for (size_t mesh = 0; mesh < meshes_.size(); mesh++)
            {
                for (size_t i = 0; i < modelMatrices_.size(); i++)
                {
                    const glm::vec3 column0 = modelMatrices_[i][0];
                    const glm::vec3 column1 = modelMatrices_[i][1];
                    const glm::vec3 column2 = modelMatrices_[i][2];
                    const glm::vec3 column3 = modelMatrices_[i][3];

                    const glm::vec3 relativeMeshPosition = column3 - cameraPos;
                    const glm::vec3 scale = glm::vec3(glm::length(column0), glm::length(column1), glm::length(column2)); // This only works for scale values > 0.
                    const float biggestScale = std::max(std::max(scale.x, scale.y),scale.z);
                    const float boundingSphereRadius = meshes_[mesh].GetBoundingSphereRadius();

                    // TODO: suspicious culling behaviour when scaling is taken into account, culls the object when it's well off the screen. Look into it.
                    { // Front and back.
                        const float projection = glm::dot(front, relativeMeshPosition);
                        if (projection < (near - boundingSphereRadius * biggestScale) || projection > (far + boundingSphereRadius * biggestScale))
                        {
                            continue;
                        }
                    }
                    { // Left.
                        // Q: @Elias: can you explain angleAxis function in detail?
                        const glm::vec3 normal = glm::angleAxis(fovX, up) * -right; // Normal to the left side of the frustum.
                        const float projection = glm::dot(normal, relativeMeshPosition);
                        if (projection > boundingSphereRadius * biggestScale) // projection is positive, meaning the position is outside the frustrum on the left.
                        {
                            continue;
                        }
                    }
                    { // Right.
                        const glm::vec3 normal = glm::angleAxis(-fovX, up) * right;
                        const float projection = glm::dot(normal, relativeMeshPosition);
                        if (projection > boundingSphereRadius * biggestScale)
                        {
                            continue;
                        }
                    }
                    { // Bottom.
                        const glm::vec3 normal = glm::angleAxis(fovY, -right) * -up;
                        const float projection = glm::dot(normal, relativeMeshPosition);
                        if (projection > boundingSphereRadius * biggestScale)
                        {
                            continue;
                        }
                    }
                    { // Top.
                        const glm::vec3 normal = glm::angleAxis(-fovY, -right) * up;
                        const float projection = glm::dot(normal, relativeMeshPosition);
                        if (projection > boundingSphereRadius * biggestScale)
                        {
                            continue;
                        }
                    }
                    modelMatricesToDraw.push_back(modelMatrices_[i]);
                }
            }
        }

        if (modelMatricesToDraw.size() > 0)
        {
            {
#ifdef TRACY_ENABLE
            ZoneNamedN(modelDrawTransferringModelsCulled, "Model::Draw(): TransferringModelsCulled", true);
            TracyGpuNamedZone(gpumodelDrawTransferringModelsCulled, "Model::Draw(): TransferringModelsCulled", true);
#endif
                glBindBuffer(GL_ARRAY_BUFFER, modelMatricesVBO_);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * modelMatricesToDraw.size(), (void*)&modelMatricesToDraw[0][0]);

            }
                for (size_t i = 0; i < meshes_.size(); i++)
                {
                    meshes_[i].Draw(modelMatricesToDraw, transformModelOffset);
                }
        }
    }
    else // Draw all models. Used for things like direct shadow rendering passes.
    {
        {
#ifdef TRACY_ENABLE
        ZoneNamedN(modelDrawTransferringModelsUnculled, "Model::Draw(): TransferringModelsUnculled", true);
        TracyGpuNamedZone(gpumodelDrawTransferringModelsUnculled, "Model::Draw(): TransferringModelsUnculled", true);
#endif
            glBindBuffer(GL_ARRAY_BUFFER, modelMatricesVBO_);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * modelMatrices_.size(), (void*)&modelMatrices_[0][0]);
        }
            for (size_t i = 0; i < meshes_.size(); i++)
            {
                meshes_[i].Draw(modelMatrices_, transformModelOffset);
            }
    }
}

void gl::Model::DrawUsingShader(Shader& shader)
{
    glBindBuffer(GL_ARRAY_BUFFER, modelMatricesVBO_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * modelMatrices_.size(), (void*)&modelMatrices_[0][0]);
    for (size_t i = 0; i < meshes_.size(); i++)
    {
        meshes_[i].DrawUsingShader(modelMatrices_, shader);
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

std::vector<glm::mat4>& gl::Model::GetModelMatrices()
{
    return modelMatrices_;
}
