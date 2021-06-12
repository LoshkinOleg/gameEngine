#pragma once

#include <glm/glm.hpp>

namespace gl {

	class Camera
	{
	public:
		enum class Camera_Movement
		{
			FORWARD,
			BACKWARD,
			LEFT,
			RIGHT,
			UP,
			DOWN
		};
        struct CameraState
        {
            glm::vec3 position_ = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 front_ = glm::vec3(0.0f, 0.0f, -1.0f);
            glm::vec3 right_ = glm::vec3(1.0f, 0.0f, 0.0f);
            glm::vec3 up_ = glm::vec3(0.0f, 1.0f, 0.0f);
            float pitch_ = 0.0f;
            float yaw_ = glm::radians(-90.0f);
        };

        void ProcessKeyboard(Camera_Movement direction);
        void ProcessMouseMovement(int x, int y);
        glm::vec3 GetPosition() const;
        glm::vec3 GetFront() const;
        glm::vec3 GetRight() const;
        glm::vec3 GetUp() const;
        glm::mat4 GetViewMatrix() const;
        CameraState GetCameraState() const;
        void SetCameraState(const CameraState& state);
        void Translate(glm::vec3 dir);

	private:
        void UpdateCameraVectors_();

        CameraState state_ = {};
	};
}//!gl.
