#pragma once
#include <string>

#include "defines.h"

namespace gl {

    using TextureId = unsigned int;

    class Texture
    {
    public:
        void Bind() const;
        void Unbind() const;
        const std::pair<std::string, int>& GetSamplerTextureUnitPair() const;
    private:
        friend class ResourceManager;

        TextureId id_ = DEFAULT_ID;
        unsigned int TEX_ = 0;
        unsigned int textureType_ = DEFAULT_TEX_TYPE;
        std::pair<std::string, int> samplerTextureUnitPair_ = {}; // Can use the same texture for multiple samplers. Ex: ambient and diffuse
    };
//
//class Cubemap
//{
//public:
//	Cubemap() = default;
//	Cubemap(std::array<const char*, 6> textures_faces)
//	{
//		glGenTextures(1, &TEX_);
//		IsError(__FILE__, __LINE__);
//		glBindTexture(GL_TEXTURE_CUBE_MAP, TEX_);
//		IsError(__FILE__, __LINE__);
//
//		int width, height, nrChannels;
//		unsigned char* data;
//		for (unsigned int i = 0; i < 6; i++)
//		{
//			data = stbi_load(textures_faces[i], &width, &height, &nrChannels, 0);
//			assert(data);
//			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//			stbi_image_free(data);
//			assert(nrChannels == 3);
//		}
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
//	}
//	void Bind(unsigned int i = 0)
//	{
//		glActiveTexture(GL_TEXTURE0 + i);
//		IsError(__FILE__, __LINE__);
//		glBindTexture(GL_TEXTURE_CUBE_MAP, TEX_);
//		IsError(__FILE__, __LINE__);
//	}
//	static void UnBind()
//	{
//		glBindTexture(GL_TEXTURE_2D, 0);
//		glActiveTexture(GL_TEXTURE0);
//	}
//	void Destroy()
//	{
//		glDeleteTextures(1, &TEX_);
//	}
//private:
//	void IsError(const char* file, int line)
//	{
//		auto error_code = glGetError();
//		if (error_code != GL_NO_ERROR)
//		{
//			throw std::runtime_error(
//				std::to_string(error_code) +
//				" in file: " + file +
//				" at line: " + std::to_string(line));
//		}
//	}
//
//	unsigned int TEX_ = 0;
//};
//	class Texture {
//	public:
//		unsigned int id = 0;
//		Texture() {};
//		Texture(const std::string& file_name)
//		{
//			int width, height, nrChannels;
//			// stbi_set_flip_vertically_on_load(true);
//			unsigned char* dataDiffuse = stbi_load(
//				file_name.c_str(),
//				&width,
//				&height,
//				&nrChannels,
//				0);
//			assert(dataDiffuse);
//
//			glGenTextures(1, &id);
//			IsError(__FILE__, __LINE__);
//			glBindTexture(GL_TEXTURE_2D, id);
//			IsError(__FILE__, __LINE__);
//			if (nrChannels == 3)
//			{
//				glTexImage2D(
//					GL_TEXTURE_2D,
//					0,
//					GL_RGB,
//					width,
//					height,
//					0,
//					GL_RGB,
//					GL_UNSIGNED_BYTE,
//					dataDiffuse);
//				IsError(__FILE__, __LINE__);
//			}
//			if (nrChannels == 4)
//			{
//				glTexImage2D(
//					GL_TEXTURE_2D,
//					0,
//					GL_RGBA,
//					width,
//					height,
//					0,
//					GL_RGBA,
//					GL_UNSIGNED_BYTE,
//					dataDiffuse);
//				IsError(__FILE__, __LINE__);
//			}
//			glTexParameteri(
//				GL_TEXTURE_2D,
//				GL_TEXTURE_WRAP_S,
//				GL_MIRRORED_REPEAT);
//			IsError(__FILE__, __LINE__);
//			glTexParameteri(
//				GL_TEXTURE_2D,
//				GL_TEXTURE_WRAP_T,
//				GL_MIRRORED_REPEAT);
//			IsError(__FILE__, __LINE__);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//			IsError(__FILE__, __LINE__);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//			IsError(__FILE__, __LINE__);
//			glGenerateMipmap(GL_TEXTURE_2D);
//			IsError(__FILE__, __LINE__);
//			glBindTexture(GL_TEXTURE_2D, 0);
//			IsError(__FILE__, __LINE__);
//			stbi_image_free(dataDiffuse);
//		}
//		void Bind(unsigned int i = 0)
//		{
//			glActiveTexture(GL_TEXTURE0 + i);
//			IsError(__FILE__, __LINE__);
//			glBindTexture(GL_TEXTURE_2D, id);
//			IsError(__FILE__, __LINE__);
//		}
//		static void UnBind()
//		{
//			glBindTexture(GL_TEXTURE_2D, 0);
//			glActiveTexture(GL_TEXTURE0);
//		}
//		void Destroy()
//		{
//			glDeleteTextures(1, &id);
//		}
//	protected:
//		void IsError(const char* file, int line) {
//			auto error_code = glGetError();
//			if (error_code != GL_NO_ERROR)
//			{
//				throw std::runtime_error(
//					std::to_string(error_code) + 
//					" in file: " + file + 
//					" at line: " + std::to_string(line));
//			}
//		}
//	};

} // End namespace gl.
