#include "framebuffer.h"

#include <glad/glad.h>
#ifdef TRACY_ENABLE
#include <Tracy.hpp>
#include <TracyOpenGL.hpp>
#endif//!TRACY_ENABLE

#include "defines.h"
#include "resource_manager.h"

void gl::Framebuffer::Create(Definition def)
{
    if (FBO_ != 0)
    {
        EngineWarning("Recreating the Framebuffer!");
    }

    defCopy_ = def;

    glGenFramebuffers(1, &FBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_);
    CheckGlError();

    // I think FBO_DEPTH and RGBA0 uses the same attachment slot?

    if (def.type & Type::FBO_DEPTH0_NO_DRAW)
    {
        assert(
            !(def.type & Type::FBO_RGBA0) && // Don't want to draw anything if we're using a shadowmap.
            !(def.type & Type::FBO_RGBA1) &&
            !(def.type & Type::FBO_RGBA2) &&
            !(def.type & Type::FBO_RGBA3) &&
            !(def.type & Type::FBO_RGBA4) &&
            !(def.type & Type::FBO_RGBA5)
        );

        CheckGlError();
        TEXs_.push_back({ 0, 0 });
        glGenTextures(1, &TEXs_.back().first);
        assert(TEXs_.back().first != 0);
        glBindTexture(GL_TEXTURE_2D, TEXs_.back().first);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, def.resolution[0], def.resolution[1], 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, TEXs_.back().first, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        CheckGlError();
        GLenum drawBuffers = GL_NONE;
        glDrawBuffers(1, &drawBuffers);
        glReadBuffer(GL_NONE);
        CheckGlError();
    }

    std::vector<unsigned int> attachments;
    for (size_t colorAttachment = 0; colorAttachment < 6; colorAttachment++) // Max 6 color attachments.
    {
        if (def.type & (Type::FBO_RGBA0 << colorAttachment))
        {
            CheckGlError();
            attachments.push_back(GL_COLOR_ATTACHMENT0 + colorAttachment);
            TEXs_.push_back({0, (unsigned int)colorAttachment});
            glGenTextures(1, &TEXs_.back().first);
            assert(TEXs_.back().first != 0);
            glBindTexture(GL_TEXTURE_2D, TEXs_.back().first);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, def.resolution[0], def.resolution[1], 0, GL_RGBA, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // We want GL_LINEAR here to be able to blur textures as they get smaller.
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glGenerateMipmap(GL_TEXTURE_2D);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachment, GL_TEXTURE_2D, TEXs_.back().first, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            CheckGlError();
        }
    }
    assert(attachments.size() < 7); // 6 color attachments max.
    glDrawBuffers(attachments.size(), attachments.data());
    // glReadBuffer(GL_NONE);
    CheckGlError();

    if (def.type & Type::RBO)
    {
        CheckGlError();
        glGenRenderbuffers(1, &RBO_);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, def.resolution[0], def.resolution[1]);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO_);
        CheckGlError();
    }

    CheckGlError();
    CheckFramebufferStatus();
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    CheckGlError();

    for (const auto& tex : TEXs_)
    {
        ResourceManager::Get().AppendNewTEX(tex.first);
    }
}
void gl::Framebuffer::Resize(std::array<size_t, 2> newResolution)
{
    defCopy_.resolution = newResolution;
    for (const auto& tex : TEXs_)
    {
        ResourceManager::Get().DeleteTEX(tex.first);
    }
    TEXs_.clear();
    glDeleteRenderbuffers(1, &RBO_);
    RBO_ = 0;
    glDeleteFramebuffers(1, &FBO_);
    FBO_ = 0;
    Create(defCopy_);
}

void gl::Framebuffer::Bind() const
{
#ifdef TRACY_ENABLE
    ZoneNamedN(framebufferBind, "Framebuffer::Bind()", true);
    TracyGpuNamedZone(gpuframebufferBind, "Framebuffer::Bind()", true);
#endif
    CheckGlError();
    glViewport(0, 0, defCopy_.resolution[0], defCopy_.resolution[1]);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // We want all framebuffer values set to 0,0,0,0 set by default. Especially the last one since that's used to determine whether the quad should be drawn or the background.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    CheckGlError();
}
void gl::Framebuffer::BindGBuffer(bool generateMipmaps) const
{
#ifdef TRACY_ENABLE
    ZoneNamedN(framebufferBindGBuffer, "Framebuffer::BindGBuffer()", true);
    TracyGpuNamedZone(gpuframebufferBindGBuffer, "Framebuffer::BindGBuffer()", true);
#endif
    CheckGlError();
    for (const auto& tex : TEXs_)
    {
        glActiveTexture(GL_TEXTURE0 + FRAMEBUFFER_TEXTURE0_UNIT + tex.second);
        glBindTexture(GL_TEXTURE_2D, tex.first);
        if (generateMipmaps)
        {
            // TODO: this generates mipmaps for ALL framebuffer textures, which isn't necessary. We could specify for which index of TEXs_ we want to generate a mipmap...
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        CheckGlError();
    }
}
void gl::Framebuffer::UnbindGBuffer() const
{
#ifdef TRACY_ENABLE
    ZoneNamedN(framebufferUnbindGBuffer, "Framebuffer::UnbindGBuffer()", true);
    TracyGpuNamedZone(gpuframebufferUnbindGBuffer, "Framebuffer::UnbindGBuffer()", true);
#endif
    for (const auto& tex : TEXs_)
    {
        glActiveTexture(GL_TEXTURE0 + FRAMEBUFFER_TEXTURE0_UNIT + tex.second);
        glBindTexture(GL_TEXTURE_2D, 0);
        CheckGlError();
    }
}

void gl::Framebuffer::Unbind(const std::array<size_t, 2> screenResolution) const
{
#ifdef TRACY_ENABLE
    ZoneNamedN(framebufferUnbind, "Framebuffer::Unbind()", true);
    TracyGpuNamedZone(gpuframebufferUnbind, "Framebuffer::Unbind()", true);
#endif
    CheckGlError();
    glViewport(0, 0, screenResolution[0], screenResolution[1]);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    CheckGlError();
}