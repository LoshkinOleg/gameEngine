#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "vertex_buffer.h"
#include "material.h"
#include "defines.h"

namespace gl
{
    class Mesh
    {
    public:
        void Create(VertexBuffer vb, Material mat);

        void Draw(const std::vector<glm::mat4>& modelMatrices);
        void DrawSingle();
        void DrawUsingShader(Shader& shader, int nrOfInstances = 1);
    private:

        VertexBuffer vb_ = {};
        Material material_ = {};
    };

}//!gl