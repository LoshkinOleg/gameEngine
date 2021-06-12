#pragma once

#include "defines.h"

namespace gl
{
    using VertexBufferId = unsigned int;

    class VertexBuffer
    {
    public:
        void Draw() const;
        void Destroy() const;
    private:
        friend class ResourceManager;

        VertexBufferId id_ = DEFAULT_ID;
        unsigned int VAO_ = 0, VBO_ = 0;
        size_t verticesCount_ = 0;
    };
}//!gl