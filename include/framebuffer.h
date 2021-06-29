#pragma once
#include <array>
#include <vector>
#include <map>

#include "texture.h"
#include "shader.h"
#include "vertex_buffer.h"
#include "defines.h"

namespace gl
{
class Framebuffer
{
public:
    enum class Type
    {
        INVALID = 0,
        RBO_DEPTH24 =   1 << 0, // 0000 0000 0001
        // Note: stencil rbo not implemented
        // RBO_STENCIL8 =  1 << 1, // 0000 0000 0010
        FBO_RGBA0 =     1 << 2, // 0000 0000 0100
        FBO_RGBA1 =     1 << 3, // 0000 0000 1000
         
        FBO_RGBA2 =     1 << 4, // 0000 0001 0000
        FBO_RGBA3 =     1 << 5, // 0000 0010 0000
        FBO_RGBA4 =     1 << 6, // 0000 0100 0000
        FBO_RGBA5 =     1 << 7, // 0000 1000 0000

        HDR =           1 << 8, // 0001 0000 0000
        FBO_DEPTH0 =    1 << 9, // 0010 0000 0000
        NO_DRAW =       1 << 10,// 0100 0000 0000
        DEFAULT = FBO_RGBA0 | RBO_DEPTH24 /*| RBO_STENCIL8*/ | HDR // 0001 0000 0111
    };
    struct Definition
    {
        Type type = Type::DEFAULT;
        std::array<std::string_view, 2> shaderPaths =
        {
            "../data/shaders/fb.vert",
            "../data/shaders/fb_hdr_reinhard.frag"
        };
        std::array<size_t, 2> resolution = { (size_t)SCREEN_RESOLUTION[0], (size_t)SCREEN_RESOLUTION[1] };

        std::map<std::string_view, int> staticInts =
        {
            {FRAMEBUFFER_SAMPLER0_NAME, FRAMEBUFFER_TEXTURE0_UNIT},
            {FRAMEBUFFER_SAMPLER1_NAME, FRAMEBUFFER_TEXTURE1_UNIT},
            {FRAMEBUFFER_SAMPLER2_NAME, FRAMEBUFFER_TEXTURE2_UNIT},
            {FRAMEBUFFER_SAMPLER3_NAME, FRAMEBUFFER_TEXTURE3_UNIT}
        };
        std::map<std::string_view, glm::vec3> staticVec3s = {};
        std::map<std::string_view, glm::mat4> staticMat4s = {};
        std::map<std::string_view, float> staticFloats = {};

        // NOTE: const* since those value are read only.
        std::map<std::string_view, const int*> dynamicInts = {};
        std::map<std::string_view, const glm::vec3*> dynamicVec3s = {};
        std::map<std::string_view, const glm::mat4*> dynamicMat4s = {};
        std::map<std::string_view, const float*> dynamicFloats = {};
    };

    void Create(Definition def);
    void Resize(std::array<size_t, 2> newResolution);

    void Bind() const;
    void BindTextures() const;
    void UnbindTextures() const;
    void Unbind(const std::array<size_t, 2> screenResolution = { (size_t)SCREEN_RESOLUTION[0], (size_t)SCREEN_RESOLUTION[1] }) const;
    void Draw();
private:

    unsigned int FBO_ = 0, RBO_ = 0;
    Shader shader_ = {};
    VertexBuffer vb_ = {};
    std::vector<Texture> textures_ = {};
    Definition defCopy_ = {}; // To be able to easilly recreate the a resized framebuffer.
};

}//!gl
