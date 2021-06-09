#pragma once

#include <string>

#include <glad/glad.h>

#include "shader.h"

namespace gl
{

class Framebuffer
{
public:
    Framebuffer() = default;
    void Init(const std::string& shader, int width, int height, bool useHdr = false)
    {
        shader_ = Shader("../data/shaders/" + shader + ".vert", "../data/shaders/" + shader + ".frag");
        shader_.Bind();
        shader_.SetInt("screenTex", 0);
        shader_.UnBind();

        // Quad's data.
        float quad[30] =
        {
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,

            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f
        };
        glGenVertexArrays(1, &VAO_);
        glBindVertexArray(VAO_);
        glGenBuffers(1, &VBO_);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Framebuffer.
        glGenFramebuffers(1, &FBO_);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO_);

        glGenTextures(1, &TEX_);
        glBindTexture(GL_TEXTURE_2D, TEX_);
        // Note that we're using the texture the size of our screen, although it is not mandatory.
        if (useHdr)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TEX_, 0);

        glGenRenderbuffers(1, &RBO_);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO_); // Only viable target is GL_RENDERBUFFER, making this type of argument completely redundant... thanks gl.
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO_);

        assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void Draw()
    {
        shader_.Bind();
        glDisable(GL_DEPTH_TEST);
        glBindVertexArray(VAO_);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        glBindTexture(GL_TEXTURE_2D, TEX_);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glEnable(GL_DEPTH_TEST);
        shader_.UnBind();
    }
    void Destroy()
    {
        shader_.Destroy();
        glDeleteTextures(1, &TEX_);
        glDeleteRenderbuffers(1, &RBO_);
        glDeleteFramebuffers(1, &FBO_);
        glDeleteBuffers(1, &VBO_);
        glDeleteVertexArrays(1, &VAO_);
    }
    void Bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO_);
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    void UnBind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
private:
    unsigned int VAO_ = 0, VBO_ = 0, FBO_ = 0, RBO_ = 0, TEX_ = 0;
    Shader shader_ = {};
};

}//!gl
