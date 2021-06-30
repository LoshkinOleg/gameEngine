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
        enum class Type
        {
            ALPHA = ALPHA_TEXTURE_UNIT, // 0
            NORMALMAP = NORMALMAP_TEXTURE_UNIT, // 1
            AMBIENT = AMBIENT_TEXTURE_UNIT, // 2
            ALBEDO = ALBEDO_TEXTURE_UNIT, // 2
            DIFFUSE = DIFFUSE_TEXTURE_UNIT, // 3
            ROUGHNESS = ROUGHNESS_TEXTURE_UNIT, // 3
            SPECULAR = SPECULAR_TEXTURE_UNIT, // 4
            METALLIC = METALLIC_TEXTURE_UNIT, // 4
            CUBEMAP = CUBEMAP_TEXTURE_UNIT, // 9
            FRAMEBUFFER0 = FRAMEBUFFER_TEXTURE0_UNIT, // 10
            FRAMEBUFFER1 = FRAMEBUFFER_TEXTURE0_UNIT, // 11
            FRAMEBUFFER2 = FRAMEBUFFER_TEXTURE0_UNIT, // 12
            FRAMEBUFFER3 = FRAMEBUFFER_TEXTURE0_UNIT, // 13
            INVALID = FRAMEBUFFER_TEXTURE3_UNIT + 1
        };
        enum class FramebufferAttachment // TODO: get rid of this sheit, it's an ugly hack. Find a way around the cyclical dependancy of Texture and Framebuffer
        {
            FBO_RGBA0 = 1 << 2, // 0000 0000 0100
            FBO_RGBA1 = 1 << 3, // 0000 0000 1000

            FBO_RGBA2 = 1 << 4, // 0000 0001 0000
            FBO_RGBA3 = 1 << 5, // 0000 0010 0000
            FBO_RGBA4 = 1 << 6, // 0000 0100 0000
            FBO_RGBA5 = 1 << 7, // 0000 1000 0000

            HDR = 1 << 8, // 0001 0000 0000
            FBO_DEPTH0 = 1 << 9, // 0010 0000 0000
        };

        // Those are hashed.
        void Create(Type textureType, std::string_view path, bool flipImage = DEFAULT_FLIP_IMAGES, bool correctGamma = DEFAULT_CORRECT_GAMMA, bool generateMipMaps = DEFAULT_GENERATE_MIPMAPS);
        void Create(std::array<std::string_view, 6> paths, bool flipImages = DEFAULT_FLIP_IMAGES, bool correctGamma = DEFAULT_CORRECT_GAMMA);
        // THIS ONE ISN'T! Always call DeleteTEX() if you need to drop the reference to it, else it'll result in a memory leak on the gpu!
        void Create(std::array<size_t, 2> resolution, FramebufferAttachment type);

        GLuint GetTEX() const;

        void Bind() const;
        void Unbind() const;
    private:

        unsigned int TEX_ = 0;
        Texture::Type type_ = Type::INVALID;
    };

}//!gl