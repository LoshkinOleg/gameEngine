#pragma once

#include <utility>
#include <string_view>

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
            std::vector<std::pair<std::string_view, Texture::Type>> texturePathsAndTypes = {};
            bool flipImages = DEFAULT_FLIP_IMAGES;
            bool correctGamma = DEFAULT_CORRECT_GAMMA;
        };

        void Create(Definition def);

        void Bind() const;
        void Unbind() const;
    private:

        Shader shader_ = {};
        std::vector<Texture> textures_ = {};
    };
}//!gl