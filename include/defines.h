#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace gl
{
	const float SCREEN_RESOLUTION[2] = { 1024.0f, 720.0f };
	const glm::mat4 PERSPECTIVE = glm::perspective(glm::radians(45.0f), SCREEN_RESOLUTION[0] / SCREEN_RESOLUTION[1], 0.1f, 100.0f);
	const float CAMERA_MOV_SPEED = 0.1f;
	const float CAMERA_MOUSE_SENSITIVITY = 0.001f;
}//!gl