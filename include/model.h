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
        // TODO: the passing of the uniform scale is a hack. You'd get wierd frustum culling behaviour if you forget to pass it or if different instances have different scales.
        void Create(std::vector<VertexBuffer::Definition> vb, std::vector<Material::Definition> mat, std::vector<glm::mat4> modelMatrices = { IDENTITY_MAT4 }, const size_t modelMatrixOffset = MODEL_MATRIX_LOCATION);

        void Draw(bool bypassFrustumCulling = false); // TODO: move offset to Model's data?
        void DrawUsingShader(Shader& shader);

        void Translate(glm::vec3 v, size_t modelMatrixIndex = 0);
        void Rotate(glm::vec3 cardinalRotation, size_t modelMatrixIndex = 0);
        void Scale(glm::vec3 v, size_t modelMatrixIndex = 0);

        std::vector<glm::mat4>& GetModelMatrices();

    private:
        size_t modelMatrixOffset_ = MODEL_MATRIX_LOCATION;
        std::vector<Mesh> meshes_ = {};
        std::vector<glm::mat4> modelMatrices_ = {};
        unsigned int modelMatricesVBO_ = 0;
    };
}//!gl