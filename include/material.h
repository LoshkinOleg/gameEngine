#pragma once
#include <array>

#include <glm/glm.hpp>

#include "defines.h"

// TODO: there's a problem of creation orders between Shader and Material making the use of the code awkward. Look into it.

namespace gl
{
    using TextureId = unsigned int;
    using MaterialId = unsigned int;
    using ShaderId = unsigned int;

    class Material
    {
    public:
        void Bind() const;
        static void Unbind();
        std::array<TextureId, 4> GetTextureIds() const;
        const std::array<glm::vec3, 3> GetColors() const;
        float GetShininess() const;
        ShaderId GetShaderId() const;
    private:
        friend class ResourceManager;

        MaterialId id_ = DEFAULT_ID;
        TextureId ambientMap_ = DEFAULT_ID;
        TextureId diffuseMap_ = DEFAULT_ID;
        TextureId specularMap_ = DEFAULT_ID;
        TextureId normalMap_ = DEFAULT_ID;
        ShaderId shader_ = DEFAULT_ID;
        glm::vec3 ambientColor_ = ONE_VEC3;
        glm::vec3 diffuseColor_ = ONE_VEC3;
        glm::vec3 specularColor_ = ONE_VEC3;
        float shininess_ = 1.0f;
    };
}//!gl