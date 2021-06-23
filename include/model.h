#pragma once
#include <vector>

#include <glm/glm.hpp>

namespace gl
{
    using ModelId = unsigned int;
    using MeshId = unsigned int;

    class Model
    {
    public:
        struct Definition
        {
            std::vector<MeshId> meshes = {};
            std::vector<glm::mat4> modelMatrices = {};
        };

        void Draw() const;

        void Translate(glm::vec3 v, size_t modelMatrixIndex = 0);
        void Rotate(glm::vec3 cardinalRotation, size_t modelMatrixIndex = 0);
        void Scale(glm::vec3 v, size_t modelMatrixIndex = 0);

        const std::vector<MeshId> GetMesheIds() const;

    private:
        friend class ResourceManager;

        std::vector<MeshId> meshes_ = {};
        std::vector<glm::mat4> modelMatrices_ = {};
        unsigned int modelMatricesVBO_ = 0;
    };
}//!gl