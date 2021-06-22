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