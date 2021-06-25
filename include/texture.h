#pragma once
#include <array>
#include <string_view>

#include "framebuffer.h"
#include "defines.h"

namespace gl
{
    class Texture
    {
    public:
        enum class Type
        {
            AMBIENT = AMBIENT_TEXTURE_UNIT, // 0
            ALPHA = ALPHA_TEXTURE_UNIT, // 1
            DIFFUSE = DIFFUSE_TEXTURE_UNIT, // 2
            SPECULAR = SPECULAR_TEXTURE_UNIT, // 3
            NORMALMAP = NORMALMAP_TEXTURE_UNIT, // 4
            ROUGHNESS = ROUGHNESS_TEXTURE_UNIT, // 5
            METALLIC = METALLIC_TEXTURE_UNIT, // 6
            SHEEN = SHEEN_TEXTURE_UNIT, // 7
            EMISSIVE = EMISSIVE_TEXTURE_UNIT, // 8
            FRAMEBUFFER = FRAMEBUFFER_TEXTURE_UNIT, // 9
            CUBEMAP = CUBEMAP_TEXTURE_UNIT, // 10
            INVALID = CUBEMAP_TEXTURE_UNIT + 1
        };

        void Create(Type textureType, std::string_view path, bool flipImage = DEFAULT_FLIP_IMAGES, bool correctGamma = DEFAULT_CORRECT_GAMMA, bool generateMipMaps = DEFAULT_GENERATE_MIPMAPS);
        void Create(std::array<std::string_view, 6> paths, bool flipImages = DEFAULT_FLIP_IMAGES, bool correctGamma = DEFAULT_CORRECT_GAMMA);
        void Create(std::array<size_t, 2> resolution, Framebuffer::Type type);

        void Bind() const;
        void Unbind() const;
    private:

        unsigned int TEX_ = 0;
        Texture::Type type_ = Type::INVALID;
    };

}//!gl