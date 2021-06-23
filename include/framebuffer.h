#pragma once
#include <array>
#include <vector>

#include "defines.h"

// TODO: add shadow buffer support, add an interpret framebuffer error func

namespace gl
{
    using FramebufferId = unsigned int;
    using MeshId = unsigned int;

class Framebuffer
{
public:
    enum class AttachmentMask
    {
        DEPTH24 =   1 << 0, // 0000 0001
        STENCIL8 =  1 << 1, // 0000 0010
        COLOR0 =    1 << 2, // 0000 0100
        COLOR1 =    1 << 3, // 0000 1000

        COLOR2 =    1 << 4, // 0001 0000
        COLOR3 =    1 << 5, // 0010 0000
        COLOR4 =    1 << 6, // 0100 0000
        COLOR5 =    1 << 7, // 1000 0000
    };
    struct Definition
    {
        AttachmentMask attachments = AttachmentMask::COLOR0;
        std::array<std::string_view, 2> shaderPaths =
        {
            "../data/shaders/fb.vert",
            "../data/shaders/fb_hdr_reinhard.frag"
        };
        // TODO: add resolution var
    };

    void Bind() const;
    static void UnBind();
    void Draw() const;
private:
    friend class ResourceManager;

    unsigned int FBO_ = 0, RBO_ = 0;
    MeshId mesh_ = DEFAULT_ID;
    AttachmentMask attachments_ = AttachmentMask::COLOR0;
};

}//!gl
