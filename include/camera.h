#pragma once

#include <glm/glm.hpp>

#include "defines.h"

// TODO: simplify camera to view and perspective matrices

namespace gl {

    using CameraId = unsigned int;

	class Camera
	{
	public:
        struct State
        {
            glm::vec3 position = FRONT_VEC3 * 5.0f; // (0;0;5)
            glm::vec3 front = BACK_VEC3; // Want the camera to face origin by default.
            glm::vec3 right = RIGHT_VEC3;
            glm::vec3 up = UP_VEC3;
            // Used for mouse input.
            float pitch = 0.0f;
            float yaw = glm::radians(-90.0f);
        };
        struct Definition
        {
            glm::vec3 position = FRONT_VEC3 * 5.0f;
            glm::vec3 front = BACK_VEC3;
            glm::vec3 up = UP_VEC3;
            float pitch = 0.0f;
        };

        void Create(Definition def);

        void Translate(glm::vec3 dir);
        void SetPosition(glm::vec3 v);
        void LookAt(const glm::vec3 pos, const glm::vec3 up = UP_VEC3);
        void SetCameraState(const State& state);

        void ProcessKeyboard(const glm::vec3 direction);
        void ProcessMouseMovement(int x, int y);

        State GetCameraState() const;
        // These get functions return read-only references, allowing the Material class to update dynamic(ie: the ones that change on every frame) shader uniforms.
        const glm::mat4& GetViewMatrix();
        const glm::vec3& GetPosition() const;
        const glm::mat4* GetViewMatrixPtr();
        const glm::vec3* GetPositionPtr() const;
        const glm::vec3& GetFront() const;
        const glm::vec3& GetRight() const;
        const glm::vec3& GetUp() const;

	private:

        void UpdateCameraVectors();
        void UpdateViewModel();

        State state_ = {};
        glm::mat4 view_ = IDENTITY_MAT4;
	};
}//!gl.
