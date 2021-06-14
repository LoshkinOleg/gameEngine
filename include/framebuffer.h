#pragma once
#include <vector>

#include "defines.h"

namespace gl
{
    using VertexBufferId = unsigned int;
    using TextureId = unsigned int;
    using ShaderId = unsigned int;

class Framebuffer
{
public:
    enum class Attachments
    {
        COLOR,
        DEPTH24_STENCIL8
    };

    void Draw() const;
    void Bind() const;
    static void UnBind();
private:
    friend class ResourceManager;

    unsigned int FBO_ = 0, RBO_ = 0;
    VertexBufferId vertexBuffer_ = DEFAULT_ID;
    TextureId texture_ = DEFAULT_ID;
    ShaderId shader_ = DEFAULT_ID;
    std::vector<Attachments> attachments_ = { Attachments::COLOR, Attachments::DEPTH24_STENCIL8 };
};

}//!gl
