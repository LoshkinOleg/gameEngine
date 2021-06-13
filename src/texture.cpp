#include "texture.h"

#include<glad/glad.h>

#include "utility.h"

void gl::Texture::Bind() const
{
    glActiveTexture(GL_TEXTURE0 + samplerTextureUnitPair_.second);
    glBindTexture(textureType_, TEX_);
    CheckGlError(__FILE__, __LINE__);
}
void gl::Texture::Unbind()
{
    glActiveTexture(GL_TEXTURE0 + samplerTextureUnitPair_.second);
    glBindTexture(textureType_, 0);
    CheckGlError(__FILE__, __LINE__);
}
const std::pair<std::string, int>& gl::Texture::GetSamplerTextureUnitPair() const
{
    return samplerTextureUnitPair_;
}
void gl::Texture::Destroy() const
{
    glDeleteTextures(1, &TEX_);
}