#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "mesh.h"
#include "material.h"

namespace gl
{
    class Model
    {
    public:

        void Create(std::vector<VertexBuffer::Definition> vb, std::vector<Material::Definition> mat, std::vector<glm::mat4> modelMatrices = { IDENTITY_MAT4 });

        void Draw(bool drawAll = false);
        void DrawSingle();
        void DrawUsingShader(Shader& shader);

        void Translate(glm::vec3 v, size_t modelMatrixIndex = 0);
        void Rotate(glm::vec3 cardinalRotation, size_t modelMatrixIndex = 0);
        void Scale(glm::vec3 v, size_t modelMatrixIndex = 0);

        std::vector<glm::mat4>& GetModelMatrices();

    private:

        std::vector<Mesh> meshes_ = {};
        std::vector<glm::mat4> modelMatrices_ = {};
        unsigned int modelMatricesVBO_ = 0;
    };
}//!gl