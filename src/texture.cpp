#include "texture.h"

#include<glad/glad.h>

#include "defines.h"

void gl::Texture::Bind() const
{
    if ((int)textureType_ < AMBIENT_TEXTURE_UNIT || (int)textureType_ > CUBEMAP_TEXTURE_UNIT)
    {
        EngineError("TextureType of texture being bound is invalid!");
    }

    glActiveTexture(GL_TEXTURE0 + (int)textureType_); // Value of TextureType corresponds to index of texture unit a particular kind of texture is usually found in.
    glBindTexture((textureType_ == Texture::Type::CUBEMAP ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D), TEX_);
    CheckGlError();
}
void gl::Texture::Unbind() const
{
    if ((int)textureType_ < AMBIENT_TEXTURE_UNIT || (int)textureType_ > CUBEMAP_TEXTURE_UNIT)
    {
        EngineError("TextureType of texture being unbound is invalid!");
    }

    glActiveTexture(GL_TEXTURE0 + (int)textureType_); // Value of TextureType corresponds to index of texture unit a particular kind of texture is usually found in.
    glBindTexture((textureType_ == Texture::Type::CUBEMAP ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D), 0);
    CheckGlError();
}