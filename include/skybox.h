#pragma once

#include "defines.h"

namespace gl
{
    using VertexBufferId = unsigned int;
    using TextureId = unsigned int;
    using ShaderId = unsigned int;
    using CameraId = unsigned int;

class Skybox
{
public:

    struct Paths
    {
        std::string_view right = "../data/textures/skybox/right.jpg";
        std::string_view left = "../data/textures/skybox/left.jpg";
        std::string_view top = "../data/textures/skybox/top.jpg";
        std::string_view bottom = "../data/textures/skybox/bottom.jpg";
        std::string_view front = "../data/textures/skybox/front.jpg";
        std::string_view back = "../data/textures/skybox/back.jpg";
    };

    void Draw(CameraId id = 0) const;
private:
    friend class ResourceManager;

    VertexBufferId vertexBuffer_ = DEFAULT_ID;
    TextureId texture_ = DEFAULT_ID;
    ShaderId shader_ = DEFAULT_ID;
};
}//!gl