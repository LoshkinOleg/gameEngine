#pragma once
#include <string_view>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "utility.h"

namespace gl
{
	// Window parameters.
	const float SCREEN_RESOLUTION[2] = { 1024.0f, 720.0f };
	const glm::mat4 PERSPECTIVE = glm::perspective(glm::radians(45.0f), SCREEN_RESOLUTION[0] / SCREEN_RESOLUTION[1], 0.1f, 100.0f);

	// Error and warning aliases.
#ifndef glErrorGuard
#define EngineError(msg) gl::Error(__FILE__, __LINE__, msg)
#endif //!glErrorGuard
#ifndef glDisplayWarningGuard
#define EngineWarning(msg) gl::Warning(__FILE__, __LINE__, msg)
#endif //!glDisplayWarningGuard
#ifndef glCheckGlErrorGuard
#define CheckGlError() gl::CheckGlError(__FILE__, __LINE__)
#endif //!glCheckGlErrorGuard

	// Camera parameters.
	const float CAMERA_MOV_SPEED = 0.1f;
	const float CAMERA_MOUSE_SENSITIVITY = 0.001f;

	// GLM default values.
	const glm::mat4 IDENTITY_MAT4 = glm::mat4(1.0f);
	const glm::vec3 ZERO_VEC3 = glm::vec3(0.0f);
	const glm::vec3 ONE_VEC3 = glm::vec3(1.0f);
	const glm::vec3 RIGHT_VEC3 = glm::vec3(1.0f, 0.0f, 0.0f);
	const glm::vec3 LEFT_VEC3 = glm::vec3(-1.0f, 0.0f, 0.0f);
	const glm::vec3 UP_VEC3 = glm::vec3(0.0f, 1.0f, 0.0f);
	const glm::vec3 DOWN_VEC3 = glm::vec3(0.0f, -1.0f, 0.0f);
	const glm::vec3 FRONT_VEC3 = glm::vec3(0.0f, 0.0f, 1.0f);
	const glm::vec3 BACK_VEC3 = glm::vec3(0.0f, 0.0f, -1.0f);

	// Hashing parameters.
	const uint32_t HASHING_SEED = 0xFFFF1337;
	const unsigned int DEFAULT_ID = 0xFFFFFFFF;

	// GL parameters.
	const int DEFAULT_TEX_TYPE = 0x0DE1; // Corresponds to GL_TEXTURE_2D
	const float CLEAR_SCREEN_COLOR[4] = { 0.0f, 0.0f, 0.3f, 1.0f };

	// Common shader variable names.
	const std::string_view AMBIENT_SAMPLER_NAME = "material.ambientMap"; // TODO: make sure this is mirrored in shaders!
	const int AMBIENT_TEXTURE_UNIT = 0;
	const std::string_view ALPHA_SAMPLER_NAME = "material.alphaMap";
	const int ALPHA_TEXTURE_UNIT = 1;
	const std::string_view DIFFUSE_SAMPLER_NAME = "material.diffuseMap";
	const int DIFFUSE_TEXTURE_UNIT = 2;
	const std::string_view SPECULAR_SAMPLER_NAME = "material.specularMap";
	const int SPECULAR_TEXTURE_UNIT = 3;
	const std::string_view NORMALMAP_SAMPLER_NAME = "material.normalMap";
	const int NORMALMAP_TEXTURE_UNIT = 4;
	const std::string_view ROUGHNESS_SAMPLER_NAME = "material.roughnessMap";
	const int ROUGHNESS_TEXTURE_UNIT = 5;
	const std::string_view METALLIC_SAMPLER_NAME = "material.metallicMap";
	const int METALLIC_TEXTURE_UNIT = 6;
	const std::string_view SHEEN_SAMPLER_NAME = "material.sheenMap";
	const int SHEEN_TEXTURE_UNIT = 7;
	const std::string_view EMISSIVE_SAMPLER_NAME = "material.emissiveMap";
	const int EMISSIVE_TEXTURE_UNIT = 8;
	const std::string_view FRAMEBUFFER_SAMPLER_NAME = "fbTexture";
	const int FRAMEBUFFER_TEXTURE_UNIT = 9;
	const std::string_view CUBEMAP_SAMPLER_NAME = "cubemap";
	const int CUBEMAP_TEXTURE_UNIT = 10;
	const std::string_view SHININESS_NAME = "material.shininess";
	const std::string_view IOR_NAME = "material.ior";
	const std::string_view VIEW_POSITION_NAME = "viewPos";
	const std::string_view PROJECTION_MARIX_NAME = "projection";
	const std::string_view VIEW_MARIX_NAME = "view";

	const int POSITION_LOCATION = 0;
	const int TEXCOORD_LOCATION = 1;
	const int NORMAL_LOCATION = 2;
	const int TANGENT_LOCATION = 3;
	const int MODEL_MATRIX_LOCATION = 4;

	// Default obj shaders.
	const std::string_view ILLUM0_SHADER[2] = {"../data/shaders/illum0.vert", "../data/shaders/illum0.frag" };
	const std::string_view ILLUM1_SHADER[2] = {"../data/shaders/illum1.vert", "../data/shaders/illum1.frag" };
	const std::string_view ILLUM2_SHADER[2] = {"../data/shaders/illum2.vert", "../data/shaders/illum2.frag" };
	const std::string_view FRAMEBUFFER_RGB_SHADER[2] = {"../data/shaders/fb.vert", "../data/shaders/fb_rgb.frag" };
	const std::string_view FRAMEBUFFER_HDR_REINHARD_SHADER[2] = {"../data/shaders/fb.vert", "../data/shaders/fb_hdr_reinhard.frag" };
	const std::string_view SKYBOX_SHADER[2] = {"../data/shaders/skybox.vert", "../data/shaders/skybox.frag" };

}//!gl