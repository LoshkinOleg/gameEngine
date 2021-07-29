#pragma once
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "utility.h"

namespace gl
{
	// Window parameters.
	constexpr const float SCREEN_RESOLUTION[2] = { 1024.0f, 768.0f }; // 4:3 aspect
	constexpr const float PROJECTION_NEAR = 0.0f;
	constexpr const float PROJECTION_FAR = 1.0f;
	constexpr const float PROJECTION_FOV = glm::radians(45.0f);
	constexpr const float ORTHO_ZOOM = 50.0f;
	constexpr const float ORTHO_HALF_HEIGHT = 0.375f * ORTHO_ZOOM;
	constexpr const float ORTHO_HALF_WIDTH = 0.5f * ORTHO_ZOOM;
	const glm::mat4 PERSPECTIVE = glm::perspective(PROJECTION_FOV, SCREEN_RESOLUTION[0] / SCREEN_RESOLUTION[1], PROJECTION_NEAR, PROJECTION_FAR);
	const glm::mat4 ORTHO = glm::ortho(-ORTHO_HALF_WIDTH, ORTHO_HALF_WIDTH, -ORTHO_HALF_HEIGHT, ORTHO_HALF_HEIGHT, PROJECTION_NEAR, PROJECTION_FAR);

	// Audio parameters.
	constexpr const size_t BYTES_PER_SAMPLE = 2; // 16-bit sounds.
	constexpr const size_t BUFFER_SIZE_IN_BYTES = 10485760; // 10 MB
	constexpr const size_t NR_OF_SAMPLES = 4096; // samples value when using WAV files.
	constexpr const int AUDIO_FORMAT = 0x8010; // SDL's AUDIO_S16
	constexpr const size_t DSP_FREQUENCY = 44100; // Standard CD quality audio's frequency.
	constexpr const size_t NR_OF_CHANNELS = 1; // We're using mono.

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
	constexpr const glm::mat4 IDENTITY_MAT4 = glm::mat4(1.0f);
	constexpr const glm::vec3 ZERO_VEC3 = glm::vec3(0.0f);
	constexpr const glm::vec2 ZERO_VEC2 = glm::vec2(0.0f);
	constexpr const glm::vec3 ONE_VEC3 = glm::vec3(1.0f);
	constexpr const glm::vec2 ONE_VEC2 = glm::vec2(1.0f);
	constexpr const glm::vec3 RIGHT_VEC3 = glm::vec3(1.0f, 0.0f, 0.0f);
	constexpr const glm::vec2 RIGHT_VEC2 = glm::vec2(1.0f, 0.0f);
	constexpr const glm::vec3 LEFT_VEC3 = glm::vec3(-1.0f, 0.0f, 0.0f);
	constexpr const glm::vec2 LEFT_VEC2 = glm::vec2(-1.0f, 0.0f);
	constexpr const glm::vec3 UP_VEC3 = glm::vec3(0.0f, 1.0f, 0.0f);
	constexpr const glm::vec2 UP_VEC2 = glm::vec2(0.0f, 1.0f);
	constexpr const glm::vec3 DOWN_VEC3 = glm::vec3(0.0f, -1.0f, 0.0f);
	constexpr const glm::vec2 DOWN_VEC2 = glm::vec2(0.0f, -1.0f);
	constexpr const glm::vec3 FRONT_VEC3 = glm::vec3(0.0f, 0.0f, 1.0f);
	constexpr const glm::vec3 BACK_VEC3 = glm::vec3(0.0f, 0.0f, -1.0f);
	constexpr const glm::vec3 WHITE = glm::vec3(1.0f, 1.0f, 1.0f);
	constexpr const glm::vec3 BLACK = glm::vec3(0.0f, 0.0f, 0.0f);
	constexpr const glm::vec3 RED = glm::vec3(1.0f, 0.0f, 0.0f);
	constexpr const glm::vec3 GREEN = glm::vec3(0.0f, 1.0f, 0.0f);
	constexpr const glm::vec3 BLUE = glm::vec3(0.0f, 0.0f, 1.0f);
	constexpr const float PI = 3.14f;
	constexpr const float SQRT_OF_TWO = 1.41f;

	// Common geometry.
#ifndef QUAD_POSITIONS
#define QUAD_POSITIONS {-1.0f, -1.0f, 1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f,  1.0f};
#endif // !QUAD_POSITIONS

	// Hashing parameters.
	constexpr const uint32_t HASHING_SEED = 0xFFFF1337;

	// GL parameters.
	constexpr const float CLEAR_SCREEN_COLOR[4] = { 0.3f, 0.0f, 0.3f, 1.0f };

}//!gl