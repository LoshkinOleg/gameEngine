#pragma once

#include <array>
#include <vector>

namespace gl
{
    using GLuint = unsigned int;

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

        void Create(Definition def);

        std::array<GLuint, 2> GetVAOandVBO() const;

        void Bind() const;
        static void Unbind();
        void Draw(int nrOfInstances = 1) const;
        void DrawSingle() const;
    private:
        friend class Model;

        unsigned int VAO_ = 0, VBO_ = 0;
        int verticesCount_ = 0;
    };
}//!gl