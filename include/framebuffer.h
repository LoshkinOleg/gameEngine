#pragma once
#include <array>
#include <vector>

#include "defines.h"

namespace gl
{
    using TextureUnitOffset = unsigned int;

class Framebuffer
{
public:
    // NOTE: annoyingly, you can't define bitwise operators for nested enums... int casting it is whenever you want to use it then...
    enum Type
    {
        INVALID =               0,

        RBO =                   1 << 0, // GL_DEPTH24_STENCIL8 in RBO.
        FBO_RGBA0 =             1 << 1,
        FBO_RGBA1 =             1 << 2,             
        FBO_RGBA2 =             1 << 3,

        FBO_RGBA3 =             1 << 4,  
        FBO_RGBA4 =             1 << 5,
        FBO_DEPTH_NO_DRAW =    1 << 6
    };

    struct Definition
    {
        Type type = Type::INVALID;
        std::array<size_t, 2> resolution = { (size_t)SCREEN_RESOLUTION[0], (size_t)SCREEN_RESOLUTION[1] };
    };

    void Create(Definition def);
    void Resize(std::array<size_t, 2> newResolution);

    void Bind() const;
    void BindGBuffer(bool generateMipmaps = false) const;
    void UnbindGBuffer() const;
    void Unbind(const std::array<size_t, 2> screenResolution = { (size_t)SCREEN_RESOLUTION[0], (size_t)SCREEN_RESOLUTION[1] }) const;
private:

    unsigned int FBO_ = 0, RBO_ = 0;
    std::vector<std::pair<unsigned int, TextureUnitOffset>> TEXs_ = {};
    Definition defCopy_ = {}; // To be able to easilly recreate a resized framebuffer.
};

}//!gl
