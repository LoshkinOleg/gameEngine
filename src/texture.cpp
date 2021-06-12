#include "texture.h"

#include<glad/glad.h>

#include "utility.h"

void gl::Texture::Bind() const
{
    glBindTexture(textureType_, TEX_);
    for (const auto& pair : samplerTextureUnitPairs_)
    {
        glActiveTexture(GL_TEXTURE0 + pair.second);
    }
    CheckGlError(__FILE__, __LINE__);
}
void gl::Texture::Unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    CheckGlError(__FILE__, __LINE__);
}
const std::vector<std::pair<std::string, int>>& gl::Texture::GetSamplerTextureUnitPairs() const
{
    return samplerTextureUnitPairs_;
}
void gl::Texture::Destroy() const
{
    glDeleteTextures(1, &TEX_);
}