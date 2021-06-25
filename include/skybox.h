#pragma once

#include <array>

#include "defines.h"

namespace gl
{
    using SkyboxId = unsigned int;

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
        std::array<std::string_view, 2> shader =
        {
            "../data/shaders/skybox.vert",
            "../data/shaders/skybox.frag"
        };
        bool flipImages = DEFAULT_FLIP_IMAGES;
        bool correctGamma = DEFAULT_CORRECT_GAMMA;
    };

    void Create(Definition def);

    void Draw() const;
private:
    friend class ResourceManager;

    unsigned int PROGRAM_ = 0, VAO_ = 0, VBO_ = 0, TEX_ = 0;
};
}//!gl