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

    // Note: we want to be able to create identical framebuffers, so no hashing.
    defCopy_ = def;

    glGenFramebuffers(1, &FBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_);
    CheckGlError();

    if ((int)def.type & (int)Type::FBO_DEPTH0)
    {
        assert((int)def.type & (int)Type::NO_DRAW);
        Texture tex;
        tex.Create(def.resolution, (Texture::FramebufferAttachment)Type::FBO_DEPTH0); // TODO: this is ugly as sheit, fix it.
        textures_.push_back(tex);
        tex.Bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex.GetTEX(), 0);
        tex.Unbind();
        CheckGlError();
    }
    if ((int)def.type & (int)Type::FBO_RGBA0)
    {
        Texture tex;
        tex.Create(def.resolution, (Texture::FramebufferAttachment)Type::FBO_RGBA0);
        CheckGlError();
        textures_.push_back(tex);
        tex.Bind();
        CheckGlError();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex.GetTEX(), 0);
        CheckGlError();
        tex.Unbind();
        CheckGlError();
    }
    if ((int)def.type & (int)Type::FBO_RGBA1)
    {
        Texture tex;
        tex.Create(def.resolution, (Texture::FramebufferAttachment)Type::FBO_RGBA1);
        textures_.push_back(tex);
        tex.Bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tex.GetTEX(), 0);
        tex.Unbind();
        CheckGlError();
    }
    if ((int)def.type & (int)Type::FBO_RGBA2)
    {
        Texture tex;
        tex.Create(def.resolution, (Texture::FramebufferAttachment)Type::FBO_RGBA2);
        textures_.push_back(tex);
        tex.Bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, tex.GetTEX(), 0);
        tex.Unbind();
        CheckGlError();
    }
    if ((int)def.type & (int)Type::FBO_RGBA3)
    {
        Texture tex;
        tex.Create(def.resolution, (Texture::FramebufferAttachment)Type::FBO_RGBA3);
        textures_.push_back(tex);
        tex.Bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, tex.GetTEX(), 0);
        tex.Unbind();
        CheckGlError();
    }
    if ((int)def.type & (int)Type::FBO_RGBA4)
    {
        Texture tex;
        tex.Create(def.resolution, (Texture::FramebufferAttachment)Type::FBO_RGBA4);
        textures_.push_back(tex);
        tex.Bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, tex.GetTEX(), 0);
        tex.Unbind();
        CheckGlError();
    }
    if ((int)def.type & (int)Type::FBO_RGBA5)
    {
        Texture tex;
        tex.Create(def.resolution, (Texture::FramebufferAttachment)Type::FBO_RGBA5);
        textures_.push_back(tex);
        tex.Bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, tex.GetTEX(), 0);
        tex.Unbind();
        CheckGlError();
    }
    if ((int)def.type & (int)Type::NO_DRAW)
    {
        GLenum drawBuffers = GL_NONE;
        glDrawBuffers(1, &drawBuffers);
        glReadBuffer(GL_NONE);
        CheckGlError();
    }
    if ((int)def.type & (int)Type::RBO_DEPTH24)
    {
        glGenRenderbuffers(1, &RBO_);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, def.resolution[0], def.resolution[1]);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO_);
        CheckGlError();
    }
    CheckFramebufferStatus();
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    CheckGlError();
}
void gl::Framebuffer::Resize(std::array<size_t, 2> newResolution)
{
    defCopy_.resolution = newResolution;
    for (auto& tex : textures_)
    {
        auto TEX = tex.GetTEX();
        glDeleteTextures(1, &TEX);
    }
    textures_.clear();
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
    glCullFace(GL_FRONT);
    glViewport(0, 0, defCopy_.resolution[0], defCopy_.resolution[1]);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_);
    glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    CheckGlError();
}
void gl::Framebuffer::BindGBuffer() const
{
    for (const auto& tex : textures_)
    {
        tex.Bind();
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
    glCullFace(GL_BACK);
    glViewport(0, 0, screenResolution[0], screenResolution[1]);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    CheckGlError();
}