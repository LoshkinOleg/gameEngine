#include "camera.h"
#include "defines.h"

void gl::Camera::ProcessKeyboard(Camera_Movement direction)
{
    switch (direction)
    {
        case Camera_Movement::FORWARD:
            state_.position_ += state_.front_ * CAMERA_MOV_SPEED;
            break;
        case Camera_Movement::BACKWARD:
            state_.position_ -= state_.front_ * CAMERA_MOV_SPEED;
            break;
        case Camera_Movement::LEFT:
            state_.position_ -= state_.right_ * CAMERA_MOV_SPEED;
            break;
        case Camera_Movement::RIGHT:
            state_.position_ += state_.right_ * CAMERA_MOV_SPEED;
            break;
        case Camera_Movement::UP:
            state_.position_ += state_.up_ * CAMERA_MOV_SPEED;
            break;
        case Camera_Movement::DOWN:
            state_.position_ -= state_.up_ * CAMERA_MOV_SPEED;
            break;
        default:
            break;
    }
}
void gl::Camera::ProcessMouseMovement(int x, int y)
{
    state_.yaw_ += (float)x * CAMERA_MOUSE_SENSITIVITY;
    state_.pitch_ -= (float)y * CAMERA_MOUSE_SENSITIVITY;
    if (state_.pitch_ > glm::radians(89.0f)) state_.pitch_ = glm::radians(89.0f);
    if (state_.pitch_ < glm::radians(-89.0f)) state_.pitch_ = glm::radians(-89.0f);
    UpdateCameraVectors_();
}
glm::vec3 gl::Camera::GetPosition() const
{
    return state_.position_;
}
glm::vec3 gl::Camera::GetFront() const
{
    return state_.front_;
}
glm::vec3 gl::Camera::GetRight() const
{
    return state_.right_;
}
glm::vec3 gl::Camera::GetUp() const
{
    return state_.up_;
}
glm::mat4 gl::Camera::GetViewMatrix() const
{
    return glm::lookAt(state_.position_, state_.position_ + state_.front_, state_.up_); // pos + front as 2nd arg to have camera always face something right in front of it.
}
gl::Camera::CameraState gl::Camera::GetCameraState() const
{
    return state_;
}
void gl::Camera::SetCameraState(const CameraState& state)
{
    state_ = state;
}
void gl::Camera::Translate(glm::vec3 dir)
{
    state_.position_.x = dir.x;
    state_.position_.y = dir.y;
    state_.position_.z = -dir.z; // Z coordinate is reversed for the camera since it's facing -Z by default.
}

void gl::Camera::UpdateCameraVectors_()
{
    // calculate the new Front vector in relation to world axis using yaw and pitch.
    glm::vec3 front;
    front.x = cos(state_.yaw_) * cos(state_.pitch_);
    front.y = sin(state_.pitch_);
    front.z = sin(state_.yaw_) * cos(state_.pitch_);
    state_.front_ = glm::normalize(front);
    // Update right and up.
    state_.right_ = glm::normalize(glm::cross(state_.front_, glm::vec3(0.0f, 1.0f, 0.0f))); // 0.0,1.0,0.0 is world UP.
    state_.up_ = glm::normalize(glm::cross(state_.right_, state_.front_));
}