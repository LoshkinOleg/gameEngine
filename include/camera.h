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
        struct State
        {
            glm::vec3 position_ = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 front_ = glm::vec3(0.0f, 0.0f, -1.0f);
            glm::vec3 right_ = glm::vec3(1.0f, 0.0f, 0.0f);
            glm::vec3 up_ = glm::vec3(0.0f, 1.0f, 0.0f);
            // Used for mouse input.
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
        State GetCameraState() const;
        void SetCameraState(const State& state);
        void Translate(glm::vec3 dir);

	private:
        friend class ResourceManager;

        void UpdateCameraVectors_();

        State state_ = {};
	};
}//!gl.
