#pragma once

#include <string>
#include <vector>
#include <utility>

#include "texture.h"

namespace gl
{
    class Material
    {
    public:
        struct Definition
        {
            std::vector<std::pair<std::string, Texture::Type>> texturePathsAndTypes = {};
        };

        void Create(Definition def);

        void Bind();
        void Unbind();
    private:

        std::vector<Texture> textures_ = {};
    };
}//!gl