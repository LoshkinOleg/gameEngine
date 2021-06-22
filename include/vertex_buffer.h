#pragma once

#include <vector>

namespace gl
{
    using VertexBufferId = unsigned int;

    class VertexBuffer
    {
    public:
        struct Definition
        {
            std::vector<unsigned int> dataLayout = // How data is laid out in the buffer. The unsigned ints indicate how many floats compose a single attribute.
            {
                3 // Position 3D
            };
            std::vector<float> data = {};
        };

        void Bind() const;
        static void Unbind();
        void Draw() const;
    private:
        friend class ResourceManager;

        unsigned int VAO_ = 0, VBO_ = 0;
        int verticesCount_ = 0;
    };
}//!gl