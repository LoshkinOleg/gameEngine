#pragma once

#include <vector>
#include <utility>
#include <string>

#include "texture.h"
#include "shader.h"

#include <glm/glm.hpp>

namespace gl
{
    class Material
    {
    public:
        struct Definition
        {
            Shader::Definition shader = {};
            std::vector<std::pair<std::string, Texture::Type>> texturePathsAndTypes = {};
            bool flipImages = DEFAULT_FLIP_IMAGES;
            bool correctGamma = DEFAULT_CORRECT_GAMMA;
        };

        void Create(Definition def);

        void Bind();
        void Unbind();
    private:

        Shader shader_ = {};
        std::vector<Texture> textures_ = {};
    };
}//!gl