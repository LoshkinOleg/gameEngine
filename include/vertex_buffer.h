#pragma once

#include <array>
#include <vector>

// TODO: move all of this to Mesh class.

namespace gl
{
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
            bool generateBoundingSphereRadius = true;
        };

        void Create(Definition def);

        std::array<unsigned int, 2> GetVAOandVBO() const; // Used by the Model to bind the VAO before setting up a AttribPointer to the transformModels.

        void Bind() const;
        static void Unbind();
        /*
        @brief: Issues an instanced draw call. Default behaviour.
        */
        void Draw(int nrOfInstances = 1) const;
        /*
        @brief: Issues a single draw call. Requires a shader that defines a model uniform to work.
        */
        void DrawSingle() const;
    private:

        unsigned int VAO_ = 0, VBO_ = 0;
        int verticesCount_ = 0;
    };
}//!gl