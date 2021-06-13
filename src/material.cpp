#include "material.h"

#include <glad/glad.h>

#include "resource_manager.h"

void gl::Material::Bind() const
{
    ResourceManager resourceManager = ResourceManager::Get();

    if (ambientMap_ != DEFAULT_ID) // Don't process maps that aren't used.
    {
        const gl::Texture& ambientTexture = resourceManager.GetTexture(ambientMap_);
        ambientTexture.Bind();
    }
    if (diffuseMap_ != DEFAULT_ID)
    {
        const gl::Texture& diffuseTexture = resourceManager.GetTexture(diffuseMap_);
        diffuseTexture.Bind();
    }
    if (specularMap_ != DEFAULT_ID)
    {
        const gl::Texture& specularTexture = resourceManager.GetTexture(specularMap_);
        specularTexture.Bind();
    }
    if (normalMap_ != DEFAULT_ID)
    {
        const gl::Texture& normalTexture = resourceManager.GetTexture(normalMap_);
        normalTexture.Bind();
    }

    // TODO: handle cubemaps
}
void gl::Material::Unbind()
{
    for (int i = 0; i < 4; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
std::array<gl::TextureId, 4> gl::Material::GetTextures() const
{
    return std::array<gl::TextureId, 4>{ambientMap_, diffuseMap_, specularMap_, normalMap_};
}
const std::array<glm::vec3, 3> gl::Material::GetColors() const
{
    return std::array<glm::vec3, 3>{ambientColor_, diffuseColor_, specularColor_};
}
float gl::Material::GetShininess() const
{
    return shininess_;
}