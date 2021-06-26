#include "framebuffer.h"

#include <glad/glad.h>

#include "defines.h"
#include "resource_manager.h"

void gl::Framebuffer::Draw()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    shader_.Bind();
    for (const auto& tex : textures_)
    {
        tex.Bind();
    }
    vb_.Draw();
    for (const auto& tex : textures_)
    {
        tex.Unbind();
    }
    shader_.Unbind();
    glEnable(GL_DEPTH_TEST);
    CheckGlError();
}
void gl::Framebuffer::Create(Definition def)
{
    if (FBO_ != 0)
    {
        EngineWarning("Recreating the Framebuffer!");
    }

    // Note: we want to be able to create identical framebuffers, so no hashing.
    defCopy_ = def;

    // Fill out the VAO and VBO.
    VertexBuffer::Definition vbdef;
    vbdef.data =
    {   // Positions   // TexCoords
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f,

        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f,
        -1.0f,  1.0f,   0.0f, 1.0f
    };
    vbdef.dataLayout =
    {
        2, // Pos
        2 // Uvs
    };
    vb_.Create(vbdef);

    Shader::Definition sdef;
    sdef.vertexPath = def.shaderPaths[0];
    sdef.fragmentPath = def.shaderPaths[1];
    sdef.staticFloats = def.staticFloats;
    sdef.staticInts = def.staticInts;
    sdef.staticMat4s = def.staticMat4s;
    sdef.staticVec3s = def.staticVec3s;
    sdef.dynamicFloats = def.dynamicFloats;
    sdef.dynamicInts = def.dynamicInts;
    sdef.dynamicMat4s = def.dynamicMat4s;
    sdef.dynamicVec3s = def.dynamicVec3s;
    shader_.Create(sdef);

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
    auto& rm = ResourceManager::Get();
    const auto vaoAndVbo = vb_.GetVAOandVBO();
    rm.DeleteVAO(vaoAndVbo[0]); // Just to be sure. Technically the ResourceManager should return the existing gpuNames but let's be paranoid.
    rm.DeleteVBO(vaoAndVbo[1]);
    rm.DeletePROGRAM(shader_.GetPROGRAM());
    for (const auto& tex : textures_) // Annoying but necessary as framebuffer textures aren't hashed.
    {
        rm.DeleteTEX(tex.GetTEX());
    }
    Create(defCopy_);
}

void gl::Framebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_);
    glViewport(0, 0, defCopy_.resolution[0], defCopy_.resolution[1]);
    glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    CheckGlError();
}
void gl::Framebuffer::Unbind(const std::array<size_t, 2> screenResolution) const
{
    glViewport(0, 0, screenResolution[0], screenResolution[1]);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    CheckGlError();
}