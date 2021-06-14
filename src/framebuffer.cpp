#include "framebuffer.h"

#include <glad/glad.h>

#include "vertex_buffer.h"
#include "texture.h"
#include "shader.h"
#include "resource_manager.h"

void gl::Framebuffer::Draw() const
{
    ResourceManager resourceManager = ResourceManager::Get();
    const VertexBuffer& vertexBuffer = resourceManager.GetVertexBuffer(vertexBuffer_);
    const Texture& texture = (texture_ == DEFAULT_ID) ? Texture() : resourceManager.GetTexture(texture_); // If there's no color attachment, no reason to have a texture.
    Shader& shader = resourceManager.GetShader(shader_);

    glDisable(GL_DEPTH_TEST);
    shader.Bind();
    if (texture_ != DEFAULT_ID) texture.Bind();
    vertexBuffer.Draw();
    if (texture_ != DEFAULT_ID) texture.Unbind();
    shader.Unbind();
    glEnable(GL_DEPTH_TEST);
}
void gl::Framebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_);
    glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);

    int clearMask = 0;
    for (const auto& attachment : attachments_)
    {
        if (attachment == Attachments::COLOR)
        {
            clearMask |= GL_COLOR_BUFFER_BIT;
        }
        else if (attachment == Attachments::DEPTH24_STENCIL8)
        {
            clearMask |= GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
        }
    }
    glClear(clearMask);
}
void gl::Framebuffer::UnBind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}