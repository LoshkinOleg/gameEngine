#pragma once
#include <array>
#include <string_view>

#include "defines.h"

namespace gl
{
    class Texture
    {
    public:
        enum Type
        {
            // ALPHA = ALPHA_TEXTURE_UNIT, // 0
            // NORMALMAP = NORMALMAP_TEXTURE_UNIT, // 1
            // DIFFUSE = DIFFUSE_TEXTURE_UNIT, // 2
            // SPECULAR = SPECULAR_TEXTURE_UNIT, // 3
            // CUBEMAP = CUBEMAP_TEXTURE_UNIT, // 4
            // INVALID = CUBEMAP_TEXTURE_UNIT + 1
        };

        // Those are hashed.
        void Create(Type textureType, std::string_view path);

        unsigned int GetTEX() const;

        void Bind() const;
        void Unbind() const;
    private:

        unsigned int TEX_ = 0;
        // Texture::Type type_ = Type::INVALID;
    };

}//!gl