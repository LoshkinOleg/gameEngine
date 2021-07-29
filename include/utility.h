#pragma once

#include <string>

#include <glm/glm.hpp>

namespace gl
{
    /*
    @brief: If there was any glError, throws an exception and prints out the gl error code and the file and line that triggered it.
    */
    void CheckGlError(const char* file, int line);
    /*
    @brief: Raises an exception and prints out a message passed in arguments as well as the file and line that triggered it.
    */
    void Error(const char* file, int line, const char* msg);
    /*
    @brief: Prints out a message passed in arguments as well as the file and line that triggered it. Useful for pointing out unusual behaviour.
    */
    void Warning(const char* file, int line, const char* msg);
    /*
    @brief: Ensures the currently bound framebuffer is complete, else throws an error with a brief reason for framebuffer incompleteness.
    */
    void CheckFramebufferStatus(const char* file, int line);
    void Message(const char* file, int line, const std::string& msg, bool writeTriggerPoint = false);

    float RemapToRange(const float inputRangeLower, const float inputRangeUpper, const float outputRangeLower, const float outputRangeUpper, const float value);
    int RemapToRange(const int inputRangeLower, const int inputRangeUpper, const int outputRangeLower, const int outputRangeUpper, const int value);

    std::string ToStr(const glm::vec3 vec);
    std::string ToStr(const glm::vec2 vec);
    glm::vec2 ToVec2(const glm::vec3 vec);
    glm::vec2 ToVec2(const glm::ivec2 vec);
    glm::vec3 ToVec3(const glm::vec2 vec);
    glm::vec2 NdcToWorld(const glm::vec2 pos, const glm::mat4& projection, const glm::mat4& view);
    glm::vec2 ScreenToNdc(const glm::vec2 pos, const float SCREEN_RESOLUTION[2]);
    float ManhattanDist(const glm::vec2 v);
    bool ManhattanCompare(const glm::vec2 v, const glm::vec2 u);
    bool EuclideanCompare(const glm::vec2 v, const glm::vec2 u);
}//!gl