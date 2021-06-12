#include "transform3d.h"

const glm::mat4& gl::Transform3d::GetModelMatrix() const
{
    return model_;
}