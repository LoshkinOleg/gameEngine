#include "mesh.h"

#include <numeric>
#include <algorithm>

#include <glad/glad.h>

void gl::Mesh::Create(const VertexBuffer::Definition vbdef, const Material::Definition matdef)
{
    if (vb_.GetVAOandVBO()[0] != 0)
    {
        EngineError("Calling Create() a second time...");
    }

    // Compute spherical bounds by using the distance to the furthest vertex of the mesh as a radius.
    float furthestDistanceToVertex = 0.0f;
    const size_t stride = (size_t)std::accumulate(vbdef.dataLayout.begin(), vbdef.dataLayout.end(), 0u);
    // Assuming the very first element in vbdef.data is a position.
    for (size_t vertex = 0; vertex < vbdef.data.size(); vertex += stride)
    {
        const glm::vec3 currentVertexPos = { vbdef.data[vertex], vbdef.data[vertex + 1], vbdef.data[vertex + 2] };
        furthestDistanceToVertex = std::max(furthestDistanceToVertex, glm::length(currentVertexPos));
    }
    // Make sure we've actually computed something.
    assert(furthestDistanceToVertex > 0.0f);
    boundingSphereRadius_ = furthestDistanceToVertex;

    vb_.Create(vbdef);
    CheckGlError();
    material_.Create(matdef);
    CheckGlError();
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

void gl::Mesh::DrawUsingShader(const std::vector<glm::mat4>& modelMatrices, Shader& shader)
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
    shader.Bind();
    vb_.Draw(modelMatrices.size());
    shader.Unbind();
    material_.Unbind();
}

float gl::Mesh::GetBoundingSphereRadius() const
{
    return boundingSphereRadius_;
}
