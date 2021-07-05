#include "mesh.h"

#include <glad/glad.h>

void gl::Mesh::Create(VertexBuffer vb, Material mat)
{
    if (vb_.GetVAOandVBO()[0] != 0)
    {
        EngineError("Calling Create() a second time...");
    }

    vb_ = vb;
    material_ = mat;
}

void gl::Mesh::Draw(const std::vector<glm::mat4>& modelMatrices)
{
    const auto& vaoAndVbo = vb_.GetVAOandVBO();
    glBindVertexArray(vaoAndVbo[0]);

    // Update pointers here in case multiple models use the same VAO/VBO.
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

    material_.Bind();
    vb_.Draw(modelMatrices.size());
    material_.Unbind();
}

void gl::Mesh::DrawSingle()
{
    material_.Bind();
    vb_.DrawSingle();
    material_.Unbind();
}

void gl::Mesh::DrawUsingShader(Shader& shader, int nrOfInstances)
{
    material_.Bind();
    shader.Bind();
    vb_.Draw(nrOfInstances);
    shader.Unbind();
    material_.Unbind();
}
