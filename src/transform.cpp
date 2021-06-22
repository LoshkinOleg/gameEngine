#include "transform.h"

const glm::mat4 gl::Transform::GetModelMatrix() const
{
    return model_;
}
void gl::Transform::Translate(glm::vec3 v)
{
    position_ += v;
    UpdateModel();
}
void gl::Transform::Rotate(glm::vec3 cardinalRotation)
{
    cardinalsRotation_ += cardinalRotation;
    UpdateModel();
}
void gl::Transform::Scale(glm::vec3 v)
{
    scale_ += v;
    UpdateModel();
}
void gl::Transform::SetPosition(glm::vec3 v)
{
    position_ = v;
    UpdateModel();
}
void gl::Transform::SetRotation(glm::vec3 v)
{
    cardinalsRotation_ = v;
    UpdateModel();
}
void gl::Transform::SetScale(glm::vec3 v)
{
    scale_ = v;
    UpdateModel();
}
void gl::Transform::UpdateModel()
{
    glm::mat4 m = glm::translate(IDENTITY_MAT4, position_);
    m = glm::rotate(m, cardinalsRotation_.x, glm::vec3(1.0f, 0.0f, 0.0f));
    m = glm::rotate(m, cardinalsRotation_.y, glm::vec3(0.0f, 1.0f, 0.0f));
    m = glm::rotate(m, cardinalsRotation_.z, glm::vec3(0.0f, 0.0f, 1.0f));
    m = glm::scale(m, scale_);
    model_ = m;
}