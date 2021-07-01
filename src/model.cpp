#include "model.h"

#include <glad/glad.h>
#include "tiny_obj_loader.h"

#include "resource_manager.h"

void gl::Model::Create(std::vector<VertexBuffer::Definition> vb, std::vector<Material::Definition> mat, std::vector<glm::mat4> modelMatrices)
{
    modelMatrices_ = modelMatrices;

    assert(vb.size() == mat.size());

    glGenBuffers(1, &modelMatricesVBO_);
    glBindBuffer(GL_ARRAY_BUFFER, modelMatricesVBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * modelMatrices_.size(), &modelMatrices[0][0][0], GL_DYNAMIC_DRAW);

    for (size_t i = 0; i < vb.size(); i++)
    {
        Material material;
        VertexBuffer vertbuff;
        vertbuff.Create(vb[i]);
        CheckGlError();
        material.Create(mat[i]);
        CheckGlError();
        const auto& vaoAndVbo = vertbuff.GetVAOandVBO();
        glBindVertexArray(vaoAndVbo[0]);
        glBindBuffer(GL_ARRAY_BUFFER, modelMatricesVBO_);

        glEnableVertexAttribArray(MODEL_MATRIX_LOCATION);
        glVertexAttribPointer(MODEL_MATRIX_LOCATION, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(MODEL_MATRIX_LOCATION + 1);
        glVertexAttribPointer(MODEL_MATRIX_LOCATION + 1, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(4 * sizeof(float)));
        glEnableVertexAttribArray(MODEL_MATRIX_LOCATION + 2);
        glVertexAttribPointer(MODEL_MATRIX_LOCATION + 2, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(2 * 4 * sizeof(float)));
        glEnableVertexAttribArray(MODEL_MATRIX_LOCATION + 3);
        glVertexAttribPointer(MODEL_MATRIX_LOCATION + 3, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(3 * 4 * sizeof(float)));
        glVertexAttribDivisor(MODEL_MATRIX_LOCATION, 1);
        glVertexAttribDivisor(MODEL_MATRIX_LOCATION + 1, 1);
        glVertexAttribDivisor(MODEL_MATRIX_LOCATION + 2, 1);
        glVertexAttribDivisor(MODEL_MATRIX_LOCATION + 3, 1);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        CheckGlError();

        meshes_.push_back(Mesh());
        meshes_.back().Create(vertbuff, material);
        CheckGlError();
    }
}

void gl::Model::Draw()
{
    // TODO: fix the issue of inappropriate use of aModel in shader when there's more than 1 mesh in a model.
    // TODO: add a way to check if the models need updating, no need to update the transformModels every frame...
    glBindBuffer(GL_ARRAY_BUFFER, modelMatricesVBO_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * modelMatrices_.size(), (void*)&modelMatrices_[0][0]);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    for (size_t i = 0; i < meshes_.size(); i++)
    {
        // glBindBuffer(GL_ARRAY_BUFFER, modelMatricesVBO_);
        meshes_[i].Draw((int)modelMatrices_.size());
    }
}

void gl::Model::DrawSingle()
{
    meshes_[0].DrawSingle();
}

void gl::Model::DrawUsingShader(Shader& shader)
{
    glBindBuffer(GL_ARRAY_BUFFER, modelMatricesVBO_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * modelMatrices_.size(), (void*)&modelMatrices_[0][0]);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    for (size_t i = 0; i < meshes_.size(); i++)
    {
        meshes_[i].DrawUsingShader(shader, (int)modelMatrices_.size());
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
