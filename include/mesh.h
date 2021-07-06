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
        void Create(const VertexBuffer::Definition vbdef, const Material::Definition matdef, const float uniformScale = 1.0f);

        void Draw(const std::vector<glm::mat4>& modelMatrices);
        void DrawUsingShader(const std::vector<glm::mat4>& modelMatrices, Shader& shader);

        float GetBoundingSphereRadius() const;
        float GetUniformScale() const;
    private:

        VertexBuffer vb_ = {};
        Material material_ = {};
        float boundingSphereRadius_ = 0.0f;
        float uniformScale_ = 1.0f;
    };

}//!gl