#pragma once
#include <array>
#include <vector>

#include "defines.h"

namespace gl
{
class Framebuffer
{
public:
    // NOTE: annoyingly, you can't define bitwise operators for nested enums... int casting it is whenever you want to use it then...
    enum Type
    {
        INVALID =       0,

        RBO =           1 << 0, // 0000 0001 : GL_DEPTH24_STENCIL8 in RBO.
        NO_DRAW =       1 << 1, // 0000 0010
        FBO_RGBA0 =     1 << 2, // 0000 0100
        FBO_RGBA1 =     1 << 3, // 0000 1000
                                   
        FBO_RGBA2 =     1 << 4, // 0001 0000
        FBO_RGBA3 =     1 << 5, // 0010 0000     
        FBO_DEPTH0 =    1 << 6  // 0100 0000
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
    std::vector<unsigned int> TEXs_ = {};
    Definition defCopy_ = {}; // To be able to easilly recreate a resized framebuffer.
};

}//!gl
