#include "framebuffer.h"

#include <glad/glad.h>

#include "defines.h"
#include "resource_manager.h"

void gl::Framebuffer::Draw() const
{
    ResourceManager& rm = ResourceManager::Get();
    const Mesh& mesh = rm.GetMesh(mesh_);

    glDisable(GL_DEPTH_TEST);
    mesh.Draw();
    glEnable(GL_DEPTH_TEST);
    CheckGlError();
}
void gl::Framebuffer::Bind() const
{
    // TODO: adjust viewport size to resolution of framebuffer, then reset it

    glBindFramebuffer(GL_FRAMEBUFFER, FBO_);
    glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);

    int clearMask = GL_COLOR_BUFFER_BIT;
    if ((int)attachments_ & (int)AttachmentMask::DEPTH24)
    {
        clearMask |= GL_DEPTH_BUFFER_BIT;
    }
    if ((int)attachments_ & (int)AttachmentMask::STENCIL8)
    {
        clearMask |= GL_STENCIL_BUFFER_BIT;
    }
    glClear(clearMask);
    CheckGlError();
}
void gl::Framebuffer::UnBind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    CheckGlError();
}