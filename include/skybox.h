#pragma once

#include <array>

#include "vertex_buffer.h"
#include "texture.h"
#include "shader.h"
#include "defines.h"

namespace gl
{
class Skybox
{
public:
    struct Definition
    {
        std::array<std::string_view, 6> paths =
        {
            "../data/textures/skybox/right.jpg",
            "../data/textures/skybox/left.jpg",
            "../data/textures/skybox/top.jpg",
            "../data/textures/skybox/bottom.jpg",
            "../data/textures/skybox/front.jpg",
            "../data/textures/skybox/back.jpg"
        };
        Shader::Definition shader = {};
        bool flipImages = false;
        bool correctGamma = DEFAULT_CORRECT_GAMMA;
    };

    void Create(Definition def);

    void Draw();
private:

    VertexBuffer vb_ = {};
    Shader shader_ = {};
    Texture cubemap_ = {};
};
}//!gl