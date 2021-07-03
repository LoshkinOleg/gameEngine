#include "texture.h"

#include <string>

#include<glad/glad.h>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif // !STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#ifndef XXH_INLINE_ALL
#define XXH_INLINE_ALL
#endif // !XXH_INLINE_ALL
#include "xxhash.h"

#include "resource_manager.h"

void gl::Texture::CreateTexture2d(Type textureType, std::string_view path, bool flipImage, bool correctGamma, bool generateMipMaps)
{
    if (TEX_ != 0)
    {
        EngineError("Calling Create() a second time...");
    }

    assert((int)textureType < (int)Type::CUBEMAP && (int)textureType > -1 && !path.empty());

    stbi_set_flip_vertically_on_load(flipImage);
    int width, height, nrChannels;
    unsigned char* data = nullptr;
    data = stbi_load(path.data(), &width, &height, &nrChannels, 0);
    assert(data && data[0] != '\0');

    const XXH32_hash_t dataHash = XXH32(data, sizeof(unsigned char) * width * height * nrChannels, HASHING_SEED);

    // Accumulate all the relevant data in a single string for hashing.
    std::string accumulatedData = std::to_string(dataHash);
    accumulatedData += std::to_string((int)textureType); // Possible values: 0 through 8

    const XXH32_hash_t hash = XXH32(accumulatedData.c_str(), sizeof(char) * accumulatedData.size(), HASHING_SEED);

    TEX_ = ResourceManager::Get().RequestTEX(hash);
    type_ = textureType;
    if (TEX_ != 0) // This means the data has been already loaded. Just use the returned gpu name.
    {
        stbi_image_free(data);
        return;
    }

    glGenTextures(1, &TEX_);
    glBindTexture(GL_TEXTURE_2D, TEX_);
    // assert(nrChannels == 3);
    if (nrChannels == 3)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, correctGamma ? GL_SRGB8 : GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    else if (nrChannels == 1)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
    }
    stbi_image_free(data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (generateMipMaps)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    CheckGlError();

    glBindTexture(GL_TEXTURE_2D, 0);

    ResourceManager::Get().AppendNewTEX(TEX_, hash);
}

void gl::Texture::CreateCubemap(std::array<std::string_view, 6> paths, bool flipImages, bool correctGamma)
{
    if (TEX_ != 0)
    {
        EngineError("Calling Create() a second time...");
    }

    stbi_set_flip_vertically_on_load(flipImages);
    int width[6], height[6], nrChannels[6];
    unsigned char* data[6];
    std::string accumulatedData = "";
    for (size_t i = 0; i < 6; i++)
    {
        assert(!paths[i].empty());
        data[i] = stbi_load(paths[i].data(), &width[i], &height[i], &nrChannels[i], 0);
        assert(data[i] && data[i][0] != '\0');
        assert(nrChannels[i] == 3 && width[i] > 0 && height[i] > 0);
        accumulatedData += std::to_string(XXH32(data[i], sizeof(unsigned char) * width[i] * height[i] * nrChannels[i], HASHING_SEED));
    }
    accumulatedData += std::to_string(correctGamma); // A corrected and non corrected texture have different data even if it's not yet visible from the raw image.
    const XXH32_hash_t hash = XXH32(accumulatedData.c_str(), sizeof(char) * accumulatedData.size(), HASHING_SEED);

    TEX_ = ResourceManager::Get().RequestTEX(hash);
    type_ = Type::CUBEMAP;
    if (TEX_ != 0) // This means the data has been already loaded. Just use the returned gpu name.
    {
        for (size_t i = 0; i < 6; i++)
        {
            stbi_image_free(data[i]);
        }
        return;
    }

    glGenTextures(1, &TEX_);
    CheckGlError();
    glBindTexture(GL_TEXTURE_CUBE_MAP, TEX_);
    CheckGlError();
    for (unsigned int i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, correctGamma ? GL_SRGB : GL_RGB, width[i], height[i], 0, GL_RGB, GL_UNSIGNED_BYTE, data[i]);
        CheckGlError();
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    CheckGlError();

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    ResourceManager::Get().AppendNewTEX(TEX_, hash);

    for (size_t i = 0; i < 6; i++)
    {
        stbi_image_free(data[i]);
    }
}

GLuint gl::Texture::GetTEX() const
{
    return TEX_;
}

void gl::Texture::Bind() const
{
    CheckGlError();
    assert((int)type_ > -1 && (int)type_ < (int)Type::INVALID);
    glActiveTexture(GL_TEXTURE0 + (int)type_);
    glBindTexture((int)type_ == (int)Type::CUBEMAP ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, TEX_);
    CheckGlError();
}

void gl::Texture::Unbind() const
{
    CheckGlError();
    assert((int)type_ > -1 && (int)type_ < (int)Type::INVALID);
    glActiveTexture(GL_TEXTURE0 + (int)type_);
    glBindTexture((int)type_ == (int)Type::CUBEMAP ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, 0);
    CheckGlError();
}
