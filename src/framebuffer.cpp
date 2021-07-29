#include "framebuffer.h"

#include <glad/glad.h>

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

    if (def.type & Type::FBO_DEPTH_NO_DRAW)
    {
        assert(
            !(def.type & Type::FBO_RGBA0) && // Don't want to have both a depth attachment AND color attachments because of the glDrawBuffers and glReadBuffer conflicting.
            !(def.type & Type::FBO_RGBA1) &&
            !(def.type & Type::FBO_RGBA2) &&
            !(def.type & Type::FBO_RGBA3) &&
            !(def.type & Type::FBO_RGBA4)
        );

        CheckGlError();
        // TEXs_.push_back({ 0, FRAMEBUFFER_SHADOWMAP_UNIT - FRAMEBUFFER_TEXTURE0_UNIT }); // Shadowmap's texture unit is the last out the ones attributed to framebuffers (15 in this case).
        glGenTextures(1, &TEXs_.back().first);
        assert(TEXs_.back().first != 0);
        glBindTexture(GL_TEXTURE_2D, TEXs_.back().first);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, (GLsizei)def.resolution[0], (GLsizei)def.resolution[1], 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
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
    else
    {
        std::vector<unsigned int> attachments;
        for (size_t colorAttachment = 0; colorAttachment < 5; colorAttachment++) // Max 5 color attachments.
        {
            if (def.type & (Type::FBO_RGBA0 << colorAttachment))
            {
                CheckGlError();
                attachments.push_back(GL_COLOR_ATTACHMENT0 + (unsigned int)colorAttachment);
                TEXs_.push_back({ 0, (unsigned int)colorAttachment });
                glGenTextures(1, &TEXs_.back().first);
                assert(TEXs_.back().first != 0);
                glBindTexture(GL_TEXTURE_2D, TEXs_.back().first);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, (int)def.resolution[0], (int)def.resolution[1], 0, GL_RGBA, GL_FLOAT, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // We want GL_LINEAR here to be able to blur textures as they get smaller.
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Can't use mipmaps for magnification duh
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glGenerateMipmap(GL_TEXTURE_2D);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (unsigned int)colorAttachment, GL_TEXTURE_2D, TEXs_.back().first, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                CheckGlError();
            }
        }
        assert(attachments.size() < 6); // 5 color attachments max.
        glDrawBuffers((int)attachments.size(), attachments.data());
        CheckGlError();
    }

    if (def.type & Type::RBO)
    {
        CheckGlError();
        glGenRenderbuffers(1, &RBO_);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (int)def.resolution[0], (int)def.resolution[1]);
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
    CheckGlError();
    glViewport(0, 0, (int)defCopy_.resolution[0], (int)defCopy_.resolution[1]);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear color needs to be all 0 for the bloom effect.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    CheckGlError();
}
void gl::Framebuffer::BindGBuffer(bool generateMipmaps) const
{
    CheckGlError();
    for (const auto& tex : TEXs_)
    {
        // glActiveTexture(GL_TEXTURE0 + FRAMEBUFFER_TEXTURE0_UNIT + tex.second);
        glBindTexture(GL_TEXTURE_2D, tex.first);
        if (generateMipmaps)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        CheckGlError();
    }
}
void gl::Framebuffer::UnbindGBuffer() const
{
    for (const auto& tex : TEXs_)
    {
        // glActiveTexture(GL_TEXTURE0 + FRAMEBUFFER_TEXTURE0_UNIT + tex.second);
        glBindTexture(GL_TEXTURE_2D, 0);
        CheckGlError();
    }
}

void gl::Framebuffer::Unbind(const std::array<size_t, 2> screenResolution) const
{
    CheckGlError();
    glViewport(0, 0, (int)screenResolution[0], (int)screenResolution[1]);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    CheckGlError();
}