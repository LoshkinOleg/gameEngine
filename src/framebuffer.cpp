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

    // I think FBO_DEPTH and RGBA0 uses the same attachment slot?
    assert(!(def.type & Framebuffer::Type::FBO_DEPTH0 && def.type & Framebuffer::Type::FBO_RGB0));

    if (def.type & Type::FBO_DEPTH0)
    {
        CheckGlError();
        assert((int)def.type & (int)Type::NO_DRAW);
        TEXs_.push_back(0);
        glGenTextures(1, &TEXs_.back());
        glBindTexture(GL_TEXTURE_2D, TEXs_.back());
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, def.resolution[0], def.resolution[1], 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, TEXs_.back(), 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        CheckGlError();
    }

    std::vector<unsigned int> attachments;
    for (size_t colorAttachment = 0; colorAttachment < 4; colorAttachment++) // Max 4 color attachments.
    {
        if (def.type & (Type::FBO_RGB0 << colorAttachment))
        {
            if (colorAttachment > 0)
            {
                // Make sure we're using color attachments in order. We don't want to allow GL_COLOR_ATTACHMENT2 to be used when GL_COLOR_ATTACHMENT1 is not used.
                assert(def.type & (Type::FBO_RGB0 << (colorAttachment - 1)));
            }

            CheckGlError();
            attachments.push_back(GL_COLOR_ATTACHMENT0 + colorAttachment);
            TEXs_.push_back(0);
            glGenTextures(1, &TEXs_.back());
            glBindTexture(GL_TEXTURE_2D, TEXs_.back());
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, def.resolution[0], def.resolution[1], 0, GL_RGB, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // We want GL_LINEAR here to be able to blur textures as they get smaller.
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glGenerateMipmap(GL_TEXTURE_2D);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachment, GL_TEXTURE_2D, TEXs_.back(), 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            CheckGlError();
        }
    }
    glDrawBuffers(attachments.size(), attachments.data());
    CheckGlError();

    if (def.type & Type::NO_DRAW)
    {
        CheckGlError();
        GLenum drawBuffers = GL_NONE;
        glDrawBuffers(1, &drawBuffers);
        glReadBuffer(GL_NONE);
        CheckGlError();
    }

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
        ResourceManager::Get().AppendNewTEX(tex);
    }
}
void gl::Framebuffer::Resize(std::array<size_t, 2> newResolution)
{
    defCopy_.resolution = newResolution;
    glDeleteTextures(TEXs_.size(), TEXs_.data());
    TEXs_.clear();
    glDeleteRenderbuffers(1, &RBO_);
    RBO_ = 0;
    glDeleteFramebuffers(1, &FBO_);
    FBO_ = 0;
    Create(defCopy_);
}

void gl::Framebuffer::Bind() const
{
    // TODO: come up with a better name for the function...
    CheckGlError();
    // glCullFace(GL_FRONT);
    glViewport(0, 0, defCopy_.resolution[0], defCopy_.resolution[1]);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_);
    glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    CheckGlError();
}
void gl::Framebuffer::BindGBuffer(bool generateMipmaps) const
{
    CheckGlError();
    for (size_t i = 0; i < TEXs_.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + FRAMEBUFFER_TEXTURE0_UNIT + i);
        glBindTexture(GL_TEXTURE_2D, TEXs_[i]);
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
    // for (const auto& tex : textures_)
    // {
    //     tex.Unbind();
    // }
}
void gl::Framebuffer::Unbind(const std::array<size_t, 2> screenResolution) const
{
    CheckGlError();
    // glCullFace(GL_BACK);
    glViewport(0, 0, screenResolution[0], screenResolution[1]);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);
    CheckGlError();
}