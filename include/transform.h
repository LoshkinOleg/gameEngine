#pragma once
#include <glm/glm.hpp>

#include "defines.h"

namespace gl
{
    using TransformId = unsigned int;

    class Transform
    {
    public:
        struct Definition
        {
            glm::vec3 position = ZERO_VEC3;
            glm::vec3 cardinalsRotation = ZERO_VEC3;
            glm::vec3 scale = ONE_VEC3;
        };

        const glm::mat4 GetModelMatrix() const;

        void Translate(glm::vec3 v);
        void Rotate(glm::vec3 cardinalRotation);
        void Scale(glm::vec3 v);
        void SetPosition(glm::vec3 v);
        void SetRotation(glm::vec3 cardinalRotation);
        void SetScale(glm::vec3 v);
    private:
        friend class ResourceManager;

        void UpdateModel();

        glm::mat4 model_ = IDENTITY_MAT4;
        glm::vec3 position_ = ZERO_VEC3;
        glm::vec3 cardinalsRotation_ = ZERO_VEC3;
        glm::vec3 scale_ = ONE_VEC3;
    };
}//!gl