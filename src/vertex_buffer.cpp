#include "vertex_buffer.h"

#include <glad/glad.h>

void gl::VertexBuffer::Draw() const
{
    // TODO: check buffer is valid
    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glDrawArrays(GL_TRIANGLES, 0, verticesCount_);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}