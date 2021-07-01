#pragma once
#include <array>
#include <string_view>

#include "defines.h"

namespace gl
{
    using GLuint = unsigned int;

    class Texture
    {
    public:
        // NOTE: some texture types share the same texture unit since they're never going to be used simultaneously like AMBIENT and ALBEDO.
        enum Type
        {
            ALPHA = ALPHA_TEXTURE_UNIT, // 0
            NORMALMAP = NORMALMAP_TEXTURE_UNIT, // 1
            AMBIENT_OR_ALBEDO = AMBIENT_TEXTURE_UNIT, // 2
            DIFFUSE_OR_ROUGHNESS = DIFFUSE_TEXTURE_UNIT, // 3
            SPECULAR_OR_METALLIC = SPECULAR_TEXTURE_UNIT, // 4
            CUBEMAP = CUBEMAP_TEXTURE_UNIT, // 5
            INVALID = CUBEMAP_TEXTURE_UNIT + 1
        };

        // Those are hashed.
        void Create(Type textureType, std::string_view path, bool flipImage = DEFAULT_FLIP_IMAGES, bool correctGamma = DEFAULT_CORRECT_GAMMA, bool generateMipMaps = DEFAULT_GENERATE_MIPMAPS);
        void Create(std::array<std::string_view, 6> paths, bool flipImages = DEFAULT_FLIP_IMAGES, bool correctGamma = DEFAULT_CORRECT_GAMMA);

        GLuint GetTEX() const;

        void Bind() const;
        void Unbind() const;
    private:

        unsigned int TEX_ = 0;
        Texture::Type type_ = Type::INVALID;
    };

}//!gl