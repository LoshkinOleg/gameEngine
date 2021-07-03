#pragma once
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "utility.h"

namespace gl
{
	// Window parameters.
	const float SCREEN_RESOLUTION[2] = { 1024.0f, 720.0f };
	const glm::mat4 PERSPECTIVE = glm::perspective(glm::radians(45.0f), SCREEN_RESOLUTION[0] / SCREEN_RESOLUTION[1], 0.1f, 100.0f);
	const glm::mat4 ORTHO = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f);

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
#ifndef glCheckFramebufferStatusGuard
#define CheckFramebufferStatus() gl::CheckFramebufferStatus(__FILE__, __LINE__)
#endif //!glCheckFramebufferStatusGuard

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

	// Common geometry.
#ifndef QUAD_POSITIONS
#define QUAD_POSITIONS {-1.0f, -1.0f, 1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f,  1.0f};
#endif // !QUAD_POSITIONS

	// Hashing parameters.
	const uint32_t HASHING_SEED = 0xFFFF1337;

	// GL parameters.
	const float CLEAR_SCREEN_COLOR[4] = { 0.3f, 0.0f, 0.3f, 1.0f };
	const bool DEFAULT_FLIP_IMAGES = true;
	const bool DEFAULT_CORRECT_GAMMA = false;
	const bool DEFAULT_GENERATE_MIPMAPS = true;

	// Common shader variables.
	const std::string ALPHA_SAMPLER_NAME = "material.alphaMap";
	const int ALPHA_TEXTURE_UNIT = 0;
	const std::string NORMALMAP_SAMPLER_NAME = "material.normalMap";
	const int NORMALMAP_TEXTURE_UNIT = 1;
	const std::string CUBEMAP_SAMPLER_NAME = "cubemap";
	const int CUBEMAP_TEXTURE_UNIT = 5;
	const int RESERVED_TEXTURE_UNIT6 = 6; // Reserved for consistent use: 0-9 for material textures, 10+ for framebuffer texture units.
	const int RESERVED_TEXTURE_UNIT7 = 7;
	const int RESERVED_TEXTURE_UNIT8 = 8;
	const int RESERVED_TEXTURE_UNIT9 = 9;
	const std::string FRAMEBUFFER_SAMPLER0_NAME = "fbTexture0";
	const int FRAMEBUFFER_TEXTURE0_UNIT = 10;
	const std::string FRAMEBUFFER_SAMPLER1_NAME = "fbTexture1";
	const int FRAMEBUFFER_TEXTURE1_UNIT = 11;
	const std::string FRAMEBUFFER_SAMPLER2_NAME = "fbTexture2";
	const int FRAMEBUFFER_TEXTURE2_UNIT = 12;
	const std::string FRAMEBUFFER_SAMPLER3_NAME = "fbTexture3";
	const int FRAMEBUFFER_TEXTURE3_UNIT = 13;
	const std::string VIEW_POSITION_NAME = "viewPos";
	const std::string CAMERA_MARIX_NAME = "cameraMatrix";
	const std::string PROJECTION_MARIX_NAME = "projectionMatrix";
	const std::string VIEW_MARIX_NAME = "viewMatrix";
	const std::string LIGHT_MATRIX_NAME = "lightMatrix";
	const std::string HDR_EXPOSURE_NAME = "exposure";
	// Blinn-Phong.
	const std::string AMBIENT_SAMPLER_NAME = "material.ambientMap";
	const int AMBIENT_TEXTURE_UNIT = 2;
	const std::string DIFFUSE_SAMPLER_NAME = "material.diffuseMap";
	const int DIFFUSE_TEXTURE_UNIT = 3;
	const std::string SPECULAR_SAMPLER_NAME = "material.specularMap";
	const int SPECULAR_TEXTURE_UNIT = 4;
	const std::string SHININESS_NAME = "material.shininess";
	// PBR.
	const std::string ALBEDO_SAMPLER_NAME = "material.albedoMap";
	const int ALBEDO_TEXTURE_UNIT = 2;
	const std::string ROUGHNESS_SAMPLER_NAME = "material.roughnessMap";
	const int ROUGHNESS_TEXTURE_UNIT = 3;
	const std::string METALLIC_SAMPLER_NAME = "material.metallicMap";
	const int METALLIC_TEXTURE_UNIT = 4;
	const std::string IOR_NAME = "material.ior";

	const int POSITION_LOCATION = 0;
	const int TEXCOORD_LOCATION = 1;
	const int NORMAL_LOCATION = 2;
	const int TANGENT_LOCATION = 3;
	const int MODEL_MATRIX_LOCATION = 4;

	// Default shaders.
	const std::string ILLUM2_SHADER[2] = {"../data/shaders/illum2.vert", "../data/shaders/illum2.frag" };
	const std::string FRAMEBUFFER_RGB_SHADER[2] = {"../data/shaders/fb.vert", "../data/shaders/fb_rgb.frag" };
	const std::string FRAMEBUFFER_HDR_REINHARD_SHADER[2] = {"../data/shaders/fb.vert", "../data/shaders/fb_hdr_reinhard.frag" };
	const std::string SKYBOX_SHADER[2] = {"../data/shaders/skybox.vert", "../data/shaders/skybox.frag" };

}//!gl