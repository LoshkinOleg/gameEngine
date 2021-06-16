#pragma once
#include <vector>

#include "defines.h"

// TODO: make a "resize" function for framebuffer that recreates new framebuffer with the right dimensions

namespace gl
{
    using VertexBufferId = unsigned int;
    using TextureId = unsigned int;
    using ShaderId = unsigned int;
    using CameraId = unsigned int;

class Framebuffer
{
public:
    enum class AttachmentType
    {
        COLOR,
        DEPTH24_STENCIL8
    };

    void Draw(CameraId id = 0) const;
    void Bind() const;
    static void UnBind();
private:
    friend class ResourceManager;

    unsigned int FBO_ = 0, RBO_ = 0;
    VertexBufferId vertexBuffer_ = DEFAULT_ID;
    TextureId texture_ = DEFAULT_ID;
    ShaderId shader_ = DEFAULT_ID;
    std::vector<AttachmentType> attachments_ = { AttachmentType::COLOR, AttachmentType::DEPTH24_STENCIL8 };
};

}//!gl
