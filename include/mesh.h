#pragma once

#include "defines.h"

namespace gl
{
    using MeshId = unsigned int;
    using VertexBufferId = unsigned int;
    using MaterialId = unsigned int;

    class Mesh
    {
    public:
        struct Definition
        {
            gl::VertexBufferId vertexBuffer = DEFAULT_ID;
            gl::MaterialId material = DEFAULT_ID;
        };

        void Draw() const;
    private:
        friend class ResourceManager;

        VertexBufferId vertexBuffer_ = DEFAULT_ID;
        MaterialId material_ = DEFAULT_ID;
    };

}//!gl