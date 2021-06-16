#include "framebuffer.h"

#include <glad/glad.h>

#include "vertex_buffer.h"
#include "texture.h"
#include "shader.h"
#include "resource_manager.h"

void gl::Framebuffer::Draw(CameraId id) const
{
    ResourceManager& resourceManager = ResourceManager::Get();
    const Texture& texture = (texture_ == DEFAULT_ID) ? Texture() : resourceManager.GetTexture(texture_);
    const VertexBuffer& vertexBuffer = resourceManager.GetVertexBuffer(vertexBuffer_);
    Shader& shader = resourceManager.GetShader(shader_);

    glDisable(GL_DEPTH_TEST);
    shader.Bind();
    // TODO: make framebuffer linked to a model so that it may be passed to Shader for any custom shaders' use.
    shader.OnDraw(Model{}, resourceManager.GetCamera(id)); // Framebuffer isn't linked to any Model.
    texture.Bind();
    vertexBuffer.Draw();
    texture.Unbind();
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
        if (attachment == AttachmentType::COLOR)
        {
            clearMask |= GL_COLOR_BUFFER_BIT;
        }
        else if (attachment == AttachmentType::DEPTH24_STENCIL8)
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