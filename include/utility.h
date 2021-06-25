#pragma once

namespace gl
{
    void CheckGlError(const char* file, int line);
    void Error(const char* file, int line, const char* msg);
    void Warning(const char* file, int line, const char* msg);
    void CheckFramebufferStatus(const char* file, int line);
}//!gl