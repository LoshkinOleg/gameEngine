#include "texture.h"

#include <string>

#include<glad/glad.h>
#include <gli/gli.hpp>
#ifndef XXH_INLINE_ALL
#define XXH_INLINE_ALL
#endif // !XXH_INLINE_ALL
#include "xxhash.h"

#include "resource_manager.h"

void gl::Texture::Create(Type textureType, std::string_view path)
{
    if (TEX_ != 0)
    {
        EngineError("Calling Create() a second time...");
    }

    assert((int)textureType < (int)Type::CUBEMAP + 1 && (int)textureType > -1 && !path.empty());

    // Accumulate all the relevant data in a single string for hashing.
    std::string accumulatedData = path.data();
    accumulatedData += std::to_string((int)textureType);

    const XXH32_hash_t hash = XXH32(accumulatedData.c_str(), sizeof(char) * accumulatedData.size(), HASHING_SEED);

    TEX_ = ResourceManager::Get().RequestTEX(hash);
    type_ = textureType;
    if (TEX_ != 0) // This means the data has been already loaded. Just use the returned gpu name.
    {
        return;
    }

    // Gli black magic fuckery.
    gli::texture Texture = gli::load(path.data());
    if (Texture.empty()) EngineError("Could not open image file!");

    gli::gl GL(gli::gl::PROFILE_GL33);
    gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());
    GLenum Target = GL.translate(Texture.target());

    glGenTextures(1, &TEX_);
    glBindTexture(Target, TEX_);
    CheckGlError();
    glTexParameteri(Target, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
    CheckGlError();
    glTexParameteri(Target, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
    glTexParameteri(Target, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
    glTexParameteri(Target, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
    glTexParameteri(Target, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);
    CheckGlError();

    glm::tvec3<GLsizei> const Extent(Texture.extent());
    GLsizei const FaceTotal = static_cast<GLsizei>(Texture.layers() * Texture.faces());

    switch (Texture.target())
    {
        case gli::TARGET_1D_ARRAY:
        case gli::TARGET_2D:
        case gli::TARGET_CUBE:
            glTexStorage2D(
                Target, static_cast<GLint>(Texture.levels()), Format.Internal,
                Extent.x, Extent.y);
            CheckGlError();
            break;
        case gli::TARGET_2D_ARRAY:
        case gli::TARGET_3D:
        case gli::TARGET_CUBE_ARRAY:
            glTexStorage3D(
                Target, static_cast<GLint>(Texture.levels()), Format.Internal,
                Extent.x, Extent.y,
                Texture.target() == gli::TARGET_3D ? Extent.z : FaceTotal);
            CheckGlError();
            break;
        default:
            assert(0);
            break;
    }

    for (std::size_t Layer = 0; Layer < Texture.layers(); ++Layer)
        for (std::size_t Face = 0; Face < Texture.faces(); ++Face)
            for (std::size_t Level = 0; Level < Texture.levels(); ++Level)
            {
                GLsizei const LayerGL = static_cast<GLsizei>(Layer);
                glm::tvec3<GLsizei> Extent(Texture.extent(Level));
                Target = gli::is_target_cube(Texture.target())
                    ? static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face)
                    : Target;

                switch (Texture.target())
                {
                    case gli::TARGET_1D_ARRAY:
                    case gli::TARGET_2D:
                    case gli::TARGET_CUBE:
                        if (gli::is_compressed(Texture.format()))
                        {
                            glCompressedTexSubImage2D(
                                Target, static_cast<GLint>(Level),
                                0, 0,
                                Extent.x,
                                Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y,
                                Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
                                Texture.data(Layer, Face, Level));
                            CheckGlError();
                        }
                        else
                        {
                            glTexSubImage2D(
                                Target, static_cast<GLint>(Level),
                                0, 0,
                                Extent.x,
                                Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y,
                                Format.External, Format.Type,
                                Texture.data(Layer, Face, Level));
                            CheckGlError();
                        }
                        break;
                    case gli::TARGET_2D_ARRAY:
                    case gli::TARGET_3D:
                    case gli::TARGET_CUBE_ARRAY:
                        if (gli::is_compressed(Texture.format()))
                        {
                            glCompressedTexSubImage3D(
                                Target, static_cast<GLint>(Level),
                                0, 0, 0,
                                Extent.x, Extent.y,
                                Texture.target() == gli::TARGET_3D ? Extent.z : LayerGL,
                                Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
                                Texture.data(Layer, Face, Level));
                            CheckGlError();
                        }
                        else
                        {
                            glTexSubImage3D(
                                Target, static_cast<GLint>(Level),
                                0, 0, 0,
                                Extent.x, Extent.y,
                                Texture.target() == gli::TARGET_3D ? Extent.z : LayerGL,
                                Format.External, Format.Type,
                                Texture.data(Layer, Face, Level));
                            CheckGlError();
                        }
                        break;
                    default: assert(0); break;
                }
            }

    CheckGlError();

    glBindTexture(GL_TEXTURE_2D, 0);

    ResourceManager::Get().AppendNewTEX(TEX_, hash);
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
