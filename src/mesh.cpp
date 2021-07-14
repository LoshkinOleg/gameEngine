#include "mesh.h"

#include <numeric>
#include <algorithm>

#include <glad/glad.h>
#ifdef TRACY_ENABLE
#include <Tracy.hpp>
#include <TracyOpenGL.hpp>
#endif//!TRACY_ENABLE

void gl::Mesh::Create(const VertexBuffer::Definition vbdef, const Material::Definition matdef)
{
    if (vb_.GetVAOandVBO()[0] != 0)
    {
        EngineError("Calling Create() a second time...");
    }

    if (vbdef.generateBoundingSphereRadius)
    {
        // Compute spherical bounds by using the distance to the furthest vertex of the mesh as a radius.
        float furthestDistanceToVertex = 0.0f;
        const size_t stride = (size_t)std::accumulate(vbdef.dataLayout.begin(), vbdef.dataLayout.end(), 0u);
        assert(stride > 2); // For 2D objects, no sense in having a bounding sphere. 2D objects are always in the camera's frustum.
        // Assuming the very first element in vbdef.data is a position.
        for (size_t vertex = 0; vertex < vbdef.data.size(); vertex += stride)
        {
            const glm::vec3 currentVertexPos = { vbdef.data[vertex], vbdef.data[vertex + 1], vbdef.data[vertex + 2] };
            furthestDistanceToVertex = std::max(furthestDistanceToVertex, glm::length(currentVertexPos));
        }
        boundingSphereRadius_ = furthestDistanceToVertex;
    }
    else
    {
        boundingSphereRadius_ = 0.0f;
    }

    vb_.Create(vbdef);
    CheckGlError();
    material_.Create(matdef);
    CheckGlError();
}

void gl::Mesh::Draw(const std::vector<glm::mat4>& modelMatrices, Shader& shader, bool updateModels, size_t transformModelOffset)
{
    if (updateModels)
    {
        const auto& vaoAndVbo = vb_.GetVAOandVBO();
        glBindVertexArray(vaoAndVbo[0]);

        // Update pointers here in case multiple models use the same VAO/VBO.
        glEnableVertexAttribArray((unsigned int)transformModelOffset);
        glVertexAttribPointer((unsigned int)transformModelOffset, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray((unsigned int)transformModelOffset + 1);
        glVertexAttribPointer((unsigned int)transformModelOffset + 1, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(4 * sizeof(float)));
        glEnableVertexAttribArray((unsigned int)transformModelOffset + 2);
        glVertexAttribPointer((unsigned int)transformModelOffset + 2, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(2 * 4 * sizeof(float)));
        glEnableVertexAttribArray((unsigned int)transformModelOffset + 3);
        glVertexAttribPointer((unsigned int)transformModelOffset + 3, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(3 * 4 * sizeof(float)));
        glVertexAttribDivisor((unsigned int)transformModelOffset, 1);
        glVertexAttribDivisor((unsigned int)transformModelOffset + 1, 1);
        glVertexAttribDivisor((unsigned int)transformModelOffset + 2, 1);
        glVertexAttribDivisor((unsigned int)transformModelOffset + 3, 1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        CheckGlError();
    }
    
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
