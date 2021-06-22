#pragma once

#include <array>

#include "defines.h"

namespace gl
{
    using SkyboxId = unsigned int;
    using MeshId = unsigned int;

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
        bool flipImages = true;
        bool correctGamma = true;
    };

    void Draw() const;
private:
    friend class ResourceManager;

    MeshId mesh_ = DEFAULT_ID;
};
}//!gl