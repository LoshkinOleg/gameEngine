#pragma once
#include <string_view>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace gl
{
	const float SCREEN_RESOLUTION[2] = { 1024.0f, 720.0f };
	const glm::mat4 PERSPECTIVE = glm::perspective(glm::radians(45.0f), SCREEN_RESOLUTION[0] / SCREEN_RESOLUTION[1], 0.1f, 100.0f);
	const float CAMERA_MOV_SPEED = 0.1f;
	const float CAMERA_MOUSE_SENSITIVITY = 0.001f;
	const glm::mat4 IDENTITY_MAT4 = glm::mat4(1.0f);
	const glm::vec3 ZERO_VEC3 = glm::vec3(0.0f);
	const glm::vec3 ONE_VEC3 = glm::vec3(1.0f);
	const uint32_t HASHING_SEED = 0xFFFF1337;
	const unsigned int DEFAULT_ID = 0xFFFFFFFF;
	const int DEFAULT_TEX_TYPE = 0x0DE1; // Corresponds to GL_TEXTURE_2D

	// Common shader variable names.
	const std::string_view AMBIENT_MAP_SAMPLER_NAME = "material.ambientMap"; // TODO: make sure this is mirrored in shaders!
	const int AMBIENT_SAMPLER_TEXTURE_UNIT = 0;
	const std::string_view DIFFUSE_MAP_SAMPLER_NAME = "material.diffuseMap";
	const int DIFFUSE_SAMPLER_TEXTURE_UNIT = 1;
	const std::string_view SPECULAR_MAP_SAMPLER_NAME = "material.specularMap";
	const int SPECULAR_SAMPLER_TEXTURE_UNIT = 2;
	const std::string_view NORMAL_MAP_SAMPLER_NAME = "material.normalMap";
	const int NORMAL_SAMPLER_TEXTURE_UNIT = 3;
}//!gl