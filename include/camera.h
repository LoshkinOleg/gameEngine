#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

namespace gl {

	class Camera
	{
	public:
		enum class Camera_Movement
		{
			FORWARD,
			BACKWARDS,
			LEFT,
			RIGHT,
			UP,
			DOWN
		};

		glm::mat4 GetViewMatrix() const;
		float GetZoom() const;
		glm::vec3 GetPos() const;
		glm::vec3 GetFront() const;
		void ProcessKeyboard(Camera_Movement direction);
		void ProcessMouseMovement(Sint32 x, Sint32 y);
		void ProcessMouseWheel(Sint32 y);

	private:
		void UpdateCameraVectors();

		glm::vec3 position_ = glm::vec3(0.0f, 0.0f, 10.0f);
		glm::vec3 front_ = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 up_ = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 right_ = glm::vec3(1.0f, 0.0f, 0.0f);
		float pitch_ = 0.0f;
		float yaw_ = glm::radians(-90.0f);
		float zoom_ = glm::radians(85.0f);

		const float MOV_SPEED_ = 0.1f;
		const float MOUSE_SENSITIVITY_ = 0.01f;
		const float ZOOM_SPEED_ = 0.1f;
	};


    glm::mat4 gl::Camera::GetViewMatrix() const
    {
        return glm::lookAt(position_, position_ + front_, up_); // pos + front as 2nd arg to have camera always face something right in front of it.
    }

    float gl::Camera::GetZoom() const
    {
        return zoom_;
    }

    void gl::Camera::ProcessKeyboard(Camera::Camera_Movement direction)
    {
        switch (direction)
        {
        case Camera_Movement::FORWARD:
            position_ += front_ * MOV_SPEED_;
            break;
        case Camera_Movement::BACKWARDS:
            position_ -= front_ * MOV_SPEED_;
            break;
        case Camera_Movement::LEFT:
            position_ -= right_ * MOV_SPEED_;
            break;
        case Camera_Movement::RIGHT:
            position_ += right_ * MOV_SPEED_;
            break;
        case Camera_Movement::UP:
            position_ += up_ * MOV_SPEED_;
            break;
        case Camera_Movement::DOWN:
            position_ -= up_ * MOV_SPEED_;
            break;
        default:
            break;
        }
    }

    void gl::Camera::ProcessMouseMovement(Sint32 x, Sint32 y)
    {
        yaw_ += (float)x * MOUSE_SENSITIVITY_;
        pitch_ -= (float)y * MOUSE_SENSITIVITY_;
        if (pitch_ > glm::radians(89.0f)) pitch_ = glm::radians(89.0f);
        if (pitch_ < glm::radians(-89.0f)) pitch_ = glm::radians(-89.0f);
        UpdateCameraVectors();
    }

    void gl::Camera::ProcessMouseWheel(Sint32 y)
    {
        zoom_ -= (float)y * ZOOM_SPEED_;
        if (zoom_ < glm::radians(85.0f)) zoom_ = glm::radians(85.0f);
        if (zoom_ > glm::radians(120.0f)) zoom_ = glm::radians(120.0f);
    }

    void gl::Camera::UpdateCameraVectors()
    {
        // calculate the new Front vector in relation to world axis using yaw and pitch.
        glm::vec3 front;
        front.x = cos(yaw_) * cos(pitch_);
        front.y = sin(pitch_);
        front.z = sin(yaw_) * cos(pitch_);
        front_ = glm::normalize(front);
        // Update right and up.
        right_ = glm::normalize(glm::cross(front_, glm::vec3(0.0f, 1.0f, 0.0f))); // 0.0,1.0,0.0 is world UP.
        up_ = glm::normalize(glm::cross(right_, front_));
    }

    glm::vec3 gl::Camera::GetPos() const
    {
        return position_;
    }

    glm::vec3 gl::Camera::GetFront() const
    {
        return front_;
    }
} // End namespace gl.
