#pragma once
#include <array>

#include <glm/glm.hpp>

#include "defines.h"

namespace gl
{
    using TextureId = unsigned int;
    using MaterialId = unsigned int;

    class Material
    {
    public:
        void Bind() const;
        static void Unbind();
        std::array<TextureId, 4> GetTextures() const;
        const std::array<glm::vec3, 3>& GetColors() const;
        float GetShininess() const;
    private:
        friend class ResourceManager;

        MaterialId id_ = DEFAULT_ID;
        TextureId ambientMap_ = DEFAULT_ID;
        TextureId diffuseMap_ = DEFAULT_ID;
        TextureId specularMap_ = DEFAULT_ID;
        TextureId normalMap_ = DEFAULT_ID;
        glm::vec3 ambientColor_ = ONE_VEC3;
        glm::vec3 diffuseColor_ = ONE_VEC3;
        glm::vec3 specularColor_ = ONE_VEC3;
        float shininess_ = 1.0f;
    };
}//!gl