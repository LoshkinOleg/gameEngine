#pragma once
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "utility.h"

namespace gl
{
	// Window parameters.
	const float SCREEN_RESOLUTION[2] = { 1024.0f, 768.0f }; // 4:3 aspect
	const float PROJECTION_NEAR = 0.1f;
	const float PROJECTION_FAR = 100.0f;
	const float PROJECTION_FOV = glm::radians(45.0f);
	const float ORTHO_HALF_HEIGHT = 10.0f;
	const float ORTHO_HALF_WIDTH = 10.0f;
	const glm::mat4 PERSPECTIVE = glm::perspective(PROJECTION_FOV, SCREEN_RESOLUTION[0] / SCREEN_RESOLUTION[1], PROJECTION_NEAR, PROJECTION_FAR);
	const glm::mat4 ORTHO = glm::ortho(-ORTHO_HALF_WIDTH, ORTHO_HALF_WIDTH, -ORTHO_HALF_HEIGHT, ORTHO_HALF_HEIGHT, PROJECTION_NEAR, PROJECTION_FAR);

	// Audio parameters.
	const size_t BYTES_PER_SAMPLE = 2; // 16-bit sounds.
	const size_t BUFFER_SIZE_IN_BYTES = 10485760; // 10 MB
	const size_t NR_OF_SAMPLES = 4096; // samples value when using WAV files.
	const int AUDIO_FORMAT = 0x8010; // SDL's AUDIO_S16
	const size_t DSP_FREQUENCY = 44100; // Standard CD quality audio's frequency.
	const size_t NR_OF_CHANNELS = 1; // We're using mono.

	// Error and warning aliases.
#ifndef glErrorGuard
#define EngineError(msg) gl::Error(__FILE__, __LINE__, msg)
#endif //!glErrorGuard
#ifndef glDisplayWarningGuard
#define EngineWarning(msg) gl::Warning(__FILE__, __LINE__, msg)
#endif //!glDisplayWarningGuard
#ifndef glDisplayMessageGuard
#define EngineMessage(msg) gl::Message(__FILE__, __LINE__, msg)
#endif //!glDisplayMessageGuard
#ifndef glCheckGlErrorGuard
#define CheckGlError() gl::CheckGlError(__FILE__, __LINE__)
// #define CheckGlError() [](){}
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
	const float PI = 3.14f;

	// Common geometry.
#ifndef QUAD_POSITIONS
#define QUAD_POSITIONS {-1.0f, -1.0f, 1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f,  1.0f};
#endif // !QUAD_POSITIONS

	// Hashing parameters.
	const uint32_t HASHING_SEED = 0xFFFF1337;

	// GL parameters.
	const float CLEAR_SCREEN_COLOR[4] = { 0.3f, 0.0f, 0.3f, 1.0f };

	// Common shader variables.
	const std::string ALPHA_SAMPLER_NAME = "material.alphaMap";
	const int ALPHA_TEXTURE_UNIT = 0;
	const std::string NORMALMAP_SAMPLER_NAME = "material.normalMap";
	const int NORMALMAP_TEXTURE_UNIT = 1;
	const std::string CUBEMAP_SAMPLER_NAME = "cubemap";
	const int CUBEMAP_TEXTURE_UNIT = 4;
	const int RESERVED_TEXTURE_UNIT5 = 5;
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
	const std::string FRAMEBUFFER_SAMPLER4_NAME = "fbTexture4";
	const int FRAMEBUFFER_TEXTURE4_UNIT = 14;
	const std::string FRAMEBUFFER_SHADOWMAP_NAME = "shadowmap";
	const int FRAMEBUFFER_SHADOWMAP_UNIT = 15;
	const std::string VIEW_POSITION_NAME = "viewPos";
	const std::string CAMERA_MARIX_NAME = "cameraMatrix";
	const std::string PROJECTION_MARIX_NAME = "projectionMatrix";
	const std::string VIEW_MARIX_NAME = "viewMatrix";
	const std::string LIGHT_MATRIX_NAME = "lightMatrix";
	const std::string HDR_EXPOSURE_NAME = "exposure";
	// Blinn-Phong.
	const std::string DIFFUSE_SAMPLER_NAME = "material.diffuseMap";
	const int DIFFUSE_TEXTURE_UNIT = 2;
	const std::string SPECULAR_SAMPLER_NAME = "material.specularMap";
	const int SPECULAR_TEXTURE_UNIT = 3;
	const std::string SHININESS_NAME = "material.shininess";

	const int POSITION_LOCATION = 0;
	const int TEXCOORD_LOCATION = 1;
	const int NORMAL_LOCATION = 2;
	const int TANGENT_LOCATION = 3;
	const int MODEL_MATRIX_LOCATION = 4;

}//!gl