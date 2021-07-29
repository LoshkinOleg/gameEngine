#include "utility.h"

#include <iostream>
#include <glad/glad.h>

void gl::CheckGlError(const char* file, int line)
{
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

void gl::Message(const char* file, int line, const std::string& msg, bool writeTriggerPoint)
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

std::string gl::ToStr(const glm::vec3 vec)
{
    std::string returnVal = "x: ";
    returnVal += std::to_string(vec.x);
    returnVal += ", y: ";
    returnVal += std::to_string(vec.y);
    returnVal += ", z: ";
    returnVal += std::to_string(vec.z);
    return returnVal;
}

std::string gl::ToStr(const glm::vec2 vec)
{
    std::string returnVal = "x: ";
    returnVal += std::to_string(vec.x);
    returnVal += ", y: ";
    returnVal += std::to_string(vec.y);
    return returnVal;
}

glm::vec2 gl::ToVec2(const glm::vec3 vec)
{
    return glm::vec2(vec.x, vec.y);
}

glm::vec2 gl::ToVec2(const glm::ivec2 vec)
{
    return glm::vec2(vec.x, vec.y);
}

glm::vec3 gl::ToVec3(const glm::vec2 vec)
{
    return glm::vec3(vec.x, vec.y, 0.0f);
}

glm::vec2 gl::NdcToWorld(const glm::vec2 pos, const glm::mat4& projection, const glm::mat4& view)
{
    static glm::mat4 cachedInvProjection;
    static glm::mat4 cachedProjection;

    const glm::mat4 invView = glm::inverse(view);

    if (cachedProjection == projection) // Don't bother computing inverse projection again.
    {
        return invView * cachedInvProjection * glm::vec4(pos.x, pos.y, 0.0f, 1.0f);
    }
    else
    {
        cachedProjection = projection;
        cachedInvProjection = glm::inverse(projection);
        return invView * cachedInvProjection * glm::vec4(pos.x, pos.y, 0.0f, 1.0f);
    }
}

glm::vec2 gl::ScreenToNdc(const glm::vec2 pos, const float SCREEN_RESOLUTION[2])
{
    return glm::vec2(RemapToRange(0.0f, SCREEN_RESOLUTION[0], -1.0f, 1.0f, pos.x), RemapToRange(0.0f, SCREEN_RESOLUTION[1], 1.0f, -1.0f, pos.y));
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
    return outputRangeLower + (value - inputRangeLower) * (outputRangeUpper - outputRangeLower) / (inputRangeUpper - inputRangeLower);
}

int gl::RemapToRange(const int inputRangeLower, const int inputRangeUpper, const int outputRangeLower, const int outputRangeUpper, const int value)
{
    return outputRangeLower + (value - inputRangeLower) * (outputRangeUpper - outputRangeLower) / (inputRangeUpper - inputRangeLower);
}

float gl::ManhattanDist(const glm::vec2 v)
{
    return glm::abs(v.x) + glm::abs(v.y);
}

bool gl::ManhattanCompare(const glm::vec2 v, const glm::vec2 u)
{
    return (ManhattanDist(v) * ManhattanDist(v)) < (ManhattanDist(u) * ManhattanDist(u));
}

bool gl::EuclideanCompare(const glm::vec2 v, const glm::vec2 u)
{
    return (glm::length(v)) < (glm::length(u));
}