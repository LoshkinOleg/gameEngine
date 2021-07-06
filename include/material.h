#pragma once

#include <string>
#include <vector>
#include <utility>

#include "texture.h"
#include "shader.h"

namespace gl
{
    class Material
    {
    public:
        struct Definition
        {
            Shader::Definition shader = {};
            std::vector<std::pair<std::string, Texture::Type>> texturePathsAndTypes = {};
        };

        void Create(Definition def);

        void Bind();
        void Unbind();
    private:

        Shader shader_ = {};
        std::vector<Texture> textures_ = {};
    };
}//!gl