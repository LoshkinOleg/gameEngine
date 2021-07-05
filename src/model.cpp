#include "model.h"

#include <glad/glad.h>
#include "tiny_obj_loader.h"
#include "glm/gtc/quaternion.hpp"

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

void gl::Model::Draw(bool drawAll)
{
    if (!drawAll)
    {
        // Cull off screen meshes.
        const float near = PROJECTION_NEAR;
        const float far = PROJECTION_FAR;
        const float aspect = SCREEN_RESOLUTION[0] / SCREEN_RESOLUTION[1];
        const float fovY = PROJECTION_FOV * 0.5f;
        // Q: @Elias: could you explain this fuckery?
        const float fovX = std::atan(std::tan(PROJECTION_FOV * 0.5f) * aspect); // Black magic afoot.
        const glm::vec3 cameraPos = ResourceManager::Get().GetCamera().GetPosition();
        const glm::vec3 right = ResourceManager::Get().GetCamera().GetRight();
        const glm::vec3 up = ResourceManager::Get().GetCamera().GetUp();
        const glm::vec3 front = ResourceManager::Get().GetCamera().GetFront();

        std::vector<glm::mat4> modelMatricesToDraw;

        // TODO: account for scaling.
        for (size_t mesh = 0; mesh < meshes_.size(); mesh++)
        {
            for (size_t i = 0; i < modelMatrices_.size(); i++)
            {
                glm::vec3 relativeMeshPosition = glm::vec3(modelMatrices_[i][3].x, modelMatrices_[i][3].y, modelMatrices_[i][3].z) - cameraPos;
                const float boundingSphereRadius = meshes_[mesh].GetBoundingSphereRadius();

                { // Front and back.
                    const float projection = glm::dot(front, relativeMeshPosition);
                    if (projection < (near - boundingSphereRadius) || projection >(far + boundingSphereRadius))
                    {
                        continue;
                    }
                }
                { // Left.
                    // Q: @Elias: can you explain angleAxis function in detail?
                    const glm::vec3 normal = glm::angleAxis(fovX, up) * -right; // Normal to the left side of the frustum.
                    const float projection = glm::dot(normal, relativeMeshPosition);
                    if (projection > boundingSphereRadius) // projection is positive, meaning the position is outside the frustrum on the left.
                    {
                        continue;
                    }
                }
                { // Right.
                    const glm::vec3 normal = glm::angleAxis(-fovX, up) * right;
                    const float projection = glm::dot(normal, relativeMeshPosition);
                    if (projection > boundingSphereRadius)
                    {
                        continue;
                    }
                }
                { // Bottom.
                    const glm::vec3 normal = glm::angleAxis(fovY, -right) * -up;
                    const float projection = glm::dot(normal, relativeMeshPosition);
                    if (projection > boundingSphereRadius)
                    {
                        continue;
                    }
                }
                { // Top.
                    const glm::vec3 normal = glm::angleAxis(-fovY, -right) * up;
                    const float projection = glm::dot(normal, relativeMeshPosition);
                    if (projection > boundingSphereRadius)
                    {
                        continue;
                    }
                }
                modelMatricesToDraw.push_back(modelMatrices_[i]);
            }
        }

        if (modelMatricesToDraw.size() > 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, modelMatricesVBO_);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * modelMatricesToDraw.size(), (void*)&modelMatricesToDraw[0][0]);
            for (size_t i = 0; i < meshes_.size(); i++)
            {
                meshes_[i].Draw(modelMatricesToDraw);
            }
        }
    }
    else // Draw all models. Used for things like direct shadow rendering passes.
    {
        glBindBuffer(GL_ARRAY_BUFFER, modelMatricesVBO_);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * modelMatrices_.size(), (void*)&modelMatrices_[0][0]);
        for (size_t i = 0; i < meshes_.size(); i++)
        {
            meshes_[i].Draw(modelMatrices_);
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
