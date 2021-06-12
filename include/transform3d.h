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
    private:
        friend class ResourceManager;

        Transform3dId id_ = DEFAULT_ID;
        glm::mat4 model_ = glm::mat4(1.0f);
        glm::vec3 position_ = glm::vec3(0.0f);
        glm::vec3 cardinalsRotation_ = glm::vec3(0.0f);
        glm::vec3 scale_ = glm::vec3(1.0f);
        glm::quat quaternion_ = glm::quat();
    };
}//!gl