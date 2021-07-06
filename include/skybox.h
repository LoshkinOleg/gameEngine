#pragma once

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
        std::string_view path = "../data/textures/skybox/skybox.ktx";
        Shader::Definition shader = {};
    };

    void Create(Definition def);

    void Draw();
private:

    VertexBuffer vb_ = {};
    Shader shader_ = {};
    Texture cubemap_ = {};
};
}//!gl