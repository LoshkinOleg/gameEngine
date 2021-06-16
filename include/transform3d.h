#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "defines.h"

namespace gl
{
    using Transform3dId = unsigned int;

    class Transform3d
    {
    public:
        const glm::mat4& GetModelMatrix() const;
        void Translate(glm::vec3 v);
        void Rotate(glm::vec3 cardinalRotation);
        void Scale(glm::vec3 v);
        void SetPosition(glm::vec3 v);
        void SetRotation(glm::vec3 v);
        void SetScale(glm::vec3 v);
    private:
        friend class ResourceManager;

        void UpdateModel();

        Transform3dId id_ = DEFAULT_ID;
        glm::mat4 model_ = glm::mat4(1.0f);
        glm::vec3 position_ = glm::vec3(0.0f);
        glm::vec3 cardinalsRotation_ = glm::vec3(0.0f);
        glm::vec3 scale_ = glm::vec3(1.0f);
        glm::quat quaternion_ = glm::quat();
    };
}//!gl