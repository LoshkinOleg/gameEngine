#include "transform.h"

void gl::Transform::Create(Definition def)
{
    // NOTE: we want transforms to be able to have identical data, so no hashing.
    gl::Transform transform;
    transform.position_ = def.position;
    transform.cardinalsRotation_ = def.cardinalsRotation;
    transform.scale_ = def.scale;
    transform.model_ = glm::translate(IDENTITY_MAT4, def.position);
    transform.model_ = glm::rotate(transform.model_, transform.cardinalsRotation_.x, RIGHT_VEC3);
    transform.model_ = glm::rotate(transform.model_, transform.cardinalsRotation_.y, UP_VEC3);
    transform.model_ = glm::rotate(transform.model_, transform.cardinalsRotation_.z, FRONT_VEC3);
    transform.model_ = glm::scale(transform.model_, transform.scale_);

    unsigned int id = (unsigned int)transforms_.size();
    transforms_.insert({ id, transform });

    return id;
}

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