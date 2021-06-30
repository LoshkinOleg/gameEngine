#pragma once
#include <array>
#include <vector>
#include <map>

#include "texture.h"
#include "shader.h"
#include "vertex_buffer.h"
#include "defines.h"

namespace gl
{
class Framebuffer
{
public:
    enum class Type
    {
        INVALID = 0,
        RBO_DEPTH24 =   1 << 0, // 0000 0000 0001
        // Note: stencil rbo not implemented
        // RBO_STENCIL8 =  1 << 1, // 0000 0000 0010
        FBO_RGBA0 =     1 << 2, // 0000 0000 0100
        FBO_RGBA1 =     1 << 3, // 0000 0000 1000
         
        FBO_RGBA2 =     1 << 4, // 0000 0001 0000
        FBO_RGBA3 =     1 << 5, // 0000 0010 0000
        FBO_RGBA4 =     1 << 6, // 0000 0100 0000
        FBO_RGBA5 =     1 << 7, // 0000 1000 0000

        HDR =           1 << 8, // 0001 0000 0000
        FBO_DEPTH0 =    1 << 9, // 0010 0000 0000
        NO_DRAW =       1 << 10,// 0100 0000 0000
        DEFAULT = FBO_RGBA0 | RBO_DEPTH24 /*| RBO_STENCIL8*/ | HDR // 0001 0000 0111
    };
    struct Definition
    {
        Type type = Type::DEFAULT;
        std::array<size_t, 2> resolution = { (size_t)SCREEN_RESOLUTION[0], (size_t)SCREEN_RESOLUTION[1] };
    };

    void Create(Definition def);
    void Resize(std::array<size_t, 2> newResolution);

    void Bind() const;
    void BindGBuffer() const;
    void UnbindGBuffer() const;
    void Unbind(const std::array<size_t, 2> screenResolution = { (size_t)SCREEN_RESOLUTION[0], (size_t)SCREEN_RESOLUTION[1] }) const;
private:

    unsigned int FBO_ = 0, RBO_ = 0;
    std::vector<Texture> textures_ = {};
    Definition defCopy_ = {}; // To be able to easilly recreate the a resized framebuffer.
};

}//!gl
