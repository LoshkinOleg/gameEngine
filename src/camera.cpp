#include "camera.h"

#include "defines.h"

void gl::Camera::ProcessKeyboard(const glm::vec3 direction)
{
    const glm::vec3 nDirection = glm::normalize(direction);
    state_.position += state_.right * nDirection.x * CAMERA_MOV_SPEED;
    state_.position += state_.up * nDirection.y * CAMERA_MOV_SPEED;
    state_.position += state_.front * nDirection.z * CAMERA_MOV_SPEED;
    UpdateViewModel();
}
void gl::Camera::ProcessMouseMovement(int x, int y)
{
    state_.yaw += (float)x * CAMERA_MOUSE_SENSITIVITY;
    state_.pitch -= (float)y * CAMERA_MOUSE_SENSITIVITY;
    if (state_.pitch > glm::radians(89.0f)) state_.pitch = glm::radians(89.0f);
    if (state_.pitch < glm::radians(-89.0f)) state_.pitch = glm::radians(-89.0f);
    UpdateCameraVectors();
    UpdateViewModel();
}
const glm::vec3& gl::Camera::GetPosition() const
{
    return state_.position;
}
const glm::mat4* gl::Camera::GetViewMatrixPtr()
{
    UpdateViewModel();
    return &view_;
}
const glm::vec3* gl::Camera::GetPositionPtr() const
{
    return &state_.position;
}
const glm::vec3& gl::Camera::GetFront() const
{
    return state_.front;
}
const glm::vec3& gl::Camera::GetRight() const
{
    return state_.right;
}
const glm::vec3& gl::Camera::GetUp() const
{
    return state_.up;
}
const glm::mat4& gl::Camera::GetViewMatrix()
{
    UpdateViewModel();
    return view_;
}
gl::Camera::State gl::Camera::GetCameraState() const
{
    return state_;
}
void gl::Camera::SetCameraState(const State& state)
{
    state_ = state;
}
void gl::Camera::Translate(glm::vec3 dir)
{
    state_.position += dir;
}
void gl::Camera::SetPosition(glm::vec3 v)
{
    state_.position = v;
}

void gl::Camera::LookAt(const glm::vec3 pos, const glm::vec3 up)
{
    // Find the new front, right and up.
    const glm::vec3 front = glm::normalize(pos - state_.position);
    glm::vec3 nUp = glm::normalize(up);
    nUp = glm::normalize(nUp - glm::dot(nUp, front) * front); // Make nUp orthogonal to nFront.
    const glm::vec3 right = glm::cross(front, nUp);

    state_.front = front;
    state_.right = right;
    state_.up = nUp;

    // Compute the new yaw and pitch. If we don't the next mouse movement will override our direction vectors.
    const float pitch = glm::asin(front.y);
    const float yaw = glm::acos(front.x / glm::cos(pitch));
    state_.pitch = pitch;
    state_.yaw = yaw;
    if (state_.pitch > glm::radians(89.0f)) state_.pitch = glm::radians(89.0f);
    if (state_.pitch < glm::radians(-89.0f)) state_.pitch = glm::radians(-89.0f);
}

void gl::Camera::UpdateCameraVectors()
{
    // calculate the new Front vector in relation to world axis using yaw and pitch.
    glm::vec3 front;
    front.x = cos(state_.yaw) * cos(state_.pitch);
    front.y = sin(state_.pitch);
    front.z = sin(state_.yaw) * cos(state_.pitch);
    state_.front = glm::normalize(front);
    // Update right and up.
    state_.right = glm::normalize(glm::cross(state_.front, glm::vec3(0.0f, 1.0f, 0.0f))); // 0.0,1.0,0.0 is world UP.
    state_.up = glm::normalize(glm::cross(state_.right, state_.front));
}

void gl::Camera::UpdateViewModel()
{
    view_ = glm::lookAt(state_.position, state_.position + state_.front, state_.up); // pos + front as 2nd arg to have camera always face something right in front of it.
}
