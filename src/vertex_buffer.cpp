#include "vertex_buffer.h"

#include <glad/glad.h>

#include "defines.h"

void gl::VertexBuffer::Bind() const
{
    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    CheckGlError();
}

void gl::VertexBuffer::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    CheckGlError();
}

void gl::VertexBuffer::Draw() const
{
    if (VAO_ == 0 || VBO_ == 0)
    {
        EngineError("Trying to draw an uninitialized VertexBuffer!");
    }

    Bind();
    glDrawArrays(GL_TRIANGLES, 0, verticesCount_);
    CheckGlError();
    Unbind();
}