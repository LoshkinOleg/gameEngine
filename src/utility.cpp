#include "utility.h"

#include <iostream>
#include <glad/glad.h>
#ifdef TRACY_ENABLE
#include <Tracy.hpp>
#include <TracyOpenGL.hpp>
#endif//!TRACY_ENABLE

void gl::CheckGlError(const char* file, int line)
{
#ifdef TRACY_ENABLE
    ZoneNamedN(utilityCheckGlError, "CheckGlError()", true);
    TracyGpuNamedZone(gpuutilityCheckGlError, "CheckGlError()", true);
#endif
    auto error_code = glGetError();
    if (error_code != GL_NO_ERROR)
    {
        std::cerr
            << error_code
            << " in file: " << file
            << " at line: " << line
            << "\n";
        abort();
    }
}

void gl::Error(const char* file, int line, const char* msg)
{
    std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": " << msg << std::endl;
    abort();
}

void gl::Warning(const char* file, int line, const char* msg)
{
    std::cout << "WARNING at file: " << __FILE__ << ", line: " << __LINE__ << ": " << msg << std::endl;
}

void gl::Message(const char* file, int line, const char* msg, bool writeTriggerPoint)
{
    if (!writeTriggerPoint)
    {
        std::cout << msg << std::endl;
    }
    else
    {
        std::cout << "MESSAGE at file: " << __FILE__ << ", line: " << __LINE__ << ": " << msg << std::endl;
    }
}

void gl::CheckFramebufferStatus(const char* file, int line)
{
    const auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::string log;
        switch (status)
        {
            case GL_FRAMEBUFFER_UNDEFINED:
                log = "Framebuffer is undefined!";
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                log = "Framebuffer is unsupported!";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                log = "Framebuffer has incomplete attachment!";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                log = "Framebuffer has incomplete missing attachment!";
                break;
            default:
                return;
        }
        std::cerr << "ERROR at file: " << file << ", line: " << line << ": " << log << std::endl;
        abort();
    }
}

float gl::RemapToRange(const float inputRangeLower, const float inputRangeUpper, const float outputRangeLower, const float outputRangeUpper, const float value)
{
    assert(value >= inputRangeLower && value <= inputRangeUpper);
    return outputRangeLower + (value - inputRangeLower) * (outputRangeUpper - outputRangeLower) / (inputRangeUpper - inputRangeLower);
}
