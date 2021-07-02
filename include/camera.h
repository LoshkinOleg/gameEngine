#pragma once

#include <glm/glm.hpp>

#include "defines.h"

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
            bool usePerspective = true;
        };

        void Create(Definition def);

        void LookAt(const glm::vec3 pos, const glm::vec3 up = UP_VEC3);

        void ProcessKeyboard(const glm::vec3 direction);
        void ProcessMouseMovement(int x, int y);

        // These get functions return read-only references, allowing the Material class to update dynamic(ie: the ones that change on every frame) shader uniforms.
        const glm::mat4* GetCameraMatrixPtr();
        const glm::mat4* GetViewMatrixPtr(); // NOTE: this is needed for skybox since it needs to multiply projection * mat3(view)...
        const glm::vec3* GetPositionPtr() const;

	private:

        void UpdateCameraVectors();
        void UpdateCameraMatrix();

        State state_ = {};
        glm::mat4 cameraMatrix_ = IDENTITY_MAT4;
        glm::mat4 viewMatrix_ = IDENTITY_MAT4; // Annoyingly this is still needed for the skybox shader.
        bool usePerspective_ = true;

        const float CAMERA_MOV_SPEED_ = 0.1f;
        const float CAMERA_MOUSE_SENSITIVITY_ = 0.001f;
        const glm::mat4 PERSPECTIVE_ = glm::perspective(glm::radians(45.0f), SCREEN_RESOLUTION[0] / SCREEN_RESOLUTION[1], 0.1f, 100.0f);
        const glm::mat4 ORTHO_ = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f);
	};
}//!gl.
