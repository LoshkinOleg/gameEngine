#pragma once
#include <iostream>
#include <glad/glad.h>

namespace gl
{
    void CheckGlError(const char* file, int line)
    {
        auto error_code = glGetError();
        if (error_code != GL_NO_ERROR)
        {
            std::cerr
                << error_code
                << " in file: " << file
                << " at line: " << line
                << "\n";
        }
    }
}//!gl