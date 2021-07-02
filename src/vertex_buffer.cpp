#include "vertex_buffer.h"

#include <string>
#include <numeric>

#include <glad/glad.h>
#ifndef XXH_INLINE_ALL
#define XXH_INLINE_ALL
#endif // !XXH_INLINE_ALL
#include "xxhash.h"

#include "resource_manager.h"
#include "defines.h"

void gl::VertexBuffer::Create(Definition def)
{
    if (VAO_ != 0)
    {
        EngineError("Calling Create() a second time...");
    }

    assert(def.data.size() > 0 && def.dataLayout.size() > 0);

    // Hash the data of the buffer and check if it's not loaded already.
    std::string accumulatedData = std::to_string(XXH32(def.data.data(), sizeof(float) * def.data.size(), HASHING_SEED));
    for (const auto& layout : def.dataLayout)
    {
        accumulatedData += std::to_string(layout); // The same data interpreted differently is still different data.
    }
    const XXH32_hash_t hash = XXH32(accumulatedData.c_str(), sizeof(char) * accumulatedData.size(), HASHING_SEED);

    VBO_ = ResourceManager::Get().RequestVBO(hash);
    VAO_ = ResourceManager::Get().RequestVAO(hash);
    const size_t stride = std::accumulate(def.dataLayout.begin(), def.dataLayout.end(), 0);
    verticesCount_ = def.data.size() / stride;
    if (VBO_ != 0)
    {
        return;
    }

    CheckGlError();
    glGenVertexArrays(1, &VAO_);
    glBindVertexArray(VAO_);
    CheckGlError();
    glGenBuffers(1, &VBO_);
    CheckGlError();
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    CheckGlError();
    glBufferData(GL_ARRAY_BUFFER, def.data.size() * sizeof(float), def.data.data(), GL_STATIC_DRAW);
    CheckGlError();

    // Enable the vertex attribute pointers.
    size_t accumulatedOffset = 0;
    for (size_t i = 0; i < def.dataLayout.size(); i++)
    {
        glEnableVertexAttribArray((unsigned int)i);
        glVertexAttribPointer((unsigned int)i, def.dataLayout[i], GL_FLOAT, GL_FALSE, (size_t)(stride * sizeof(float)), (void*)accumulatedOffset);
        accumulatedOffset += def.dataLayout[i] * sizeof(float);
        CheckGlError();
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    CheckGlError();

    ResourceManager::Get().AppendNewVAO(VAO_, hash);
    ResourceManager::Get().AppendNewVBO(VBO_, hash);
}

std::array<unsigned int, 2> gl::VertexBuffer::GetVAOandVBO() const
{
    return std::array<unsigned int, 2>{VAO_, VBO_};
}

void gl::VertexBuffer::Bind() const
{
    glBindVertexArray(VAO_);
    CheckGlError();
}

void gl::VertexBuffer::Unbind()
{
    // glBindVertexArray(0);
    CheckGlError();
}

void gl::VertexBuffer::Draw(int nrOfInstances) const
{
    assert(VAO_ != 0 && VBO_ != 0);

    Bind();
    glDrawArraysInstanced(GL_TRIANGLES, 0, verticesCount_, nrOfInstances);
    CheckGlError();
    Unbind();
}

void gl::VertexBuffer::DrawSingle() const
{
    assert(VAO_ != 0 && VBO_ != 0);

    Bind();
    glDrawArrays(GL_TRIANGLES, 0, verticesCount_);
    CheckGlError();
    Unbind();
}
