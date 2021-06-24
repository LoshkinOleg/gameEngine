#pragma once
#include <vector>
#include <string_view>

#include "defines.h"

namespace gl
{
    using TextureId = unsigned int;

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
            CUBEMAP = CUBEMAP_TEXTURE_UNIT // 10
        };

        struct Definition
        {
            Type textureType = Type::AMBIENT;
            std::vector<std::string_view> paths = // If there's more than one path here, it's a cubemap.
            {
                "../data/textures/blank2x2.png"
            };
            bool flipImage = false;
            bool correctGamma = false;
            bool useHdr = false;
            std::array<size_t, 2> resolution = { (size_t)SCREEN_RESOLUTION[0], (size_t)SCREEN_RESOLUTION[1] };
        };

        void Bind() const;
        void Unbind() const;
    private:
        friend class ResourceManager;

        unsigned int TEX_ = 0;
        Type textureType_ = Type::AMBIENT;
    };

}//!gl