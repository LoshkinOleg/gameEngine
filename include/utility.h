#pragma once

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
    void Message(const char* file, int line, const char* msg, bool writeTriggerPoint = false);
    float RemapToRange(const float inputRangeLower, const float inputRangeUpper, const float outputRangeLower, const float outputRangeUpper, const float value);
}//!gl