#include "material.h"

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "resource_manager.h"

void gl::Material::Bind() const
{
	// Bind textures
	ResourceManager& rm = ResourceManager::Get();
	const auto textures = rm.GetTextures(textures_);
	for (const auto& texture : textures)
	{
		texture.Bind();
	}
	OnDraw();
	glUseProgram(PROGRAM_);
}
void gl::Material::Unbind() const
{
	ResourceManager& rm = ResourceManager::Get();
	const auto textures = rm.GetTextures(textures_);
	for (const auto& texture : textures)
	{
		texture.Unbind();
	}
	glUseProgram(0);
}

void gl::Material::AddDynamicUniformPair(std::pair<std::string_view, const glm::vec3*> pair)
{
	dynamicVec3s_.insert(pair);
}

void gl::Material::SetInt(const std::pair<std::string_view, int> pair) const
{
	glUseProgram(PROGRAM_);
	glUniform1i(ResourceManager::Get().GetUniformName(pair.first, PROGRAM_), pair.second);
	glUseProgram(0);
	CheckGlError();
}

void gl::Material::SetInt(const std::pair<std::string_view, const int*> pair) const
{
	glUseProgram(PROGRAM_);
	glUniform1i(ResourceManager::Get().GetUniformName(pair.first, PROGRAM_), *(pair.second));
	glUseProgram(0);
	CheckGlError();
}

void gl::Material::SetVec3(const std::pair<std::string_view, glm::vec3> pair) const
{
	glUseProgram(PROGRAM_);
	glUniform3fv(ResourceManager::Get().GetUniformName(pair.first, PROGRAM_), 1, &pair.second[0]);
	glUseProgram(0);
	CheckGlError();
}

void gl::Material::SetVec3(const std::pair<std::string_view, const glm::vec3*> pair) const
{
	glUseProgram(PROGRAM_);
	glUniform3fv(ResourceManager::Get().GetUniformName(pair.first, PROGRAM_), 1, &(*pair.second)[0]);
	glUseProgram(0);
	CheckGlError();
}

void gl::Material::SetMat4(const std::pair<std::string_view, glm::mat4> pair) const
{
	glUseProgram(PROGRAM_);
	glUniformMatrix4fv(ResourceManager::Get().GetUniformName(pair.first, PROGRAM_), 1, GL_FALSE, &pair.second[0][0]);
	glUseProgram(0);
	CheckGlError();
}

void gl::Material::SetMat4(const std::pair<std::string_view, const glm::mat4*> pair) const
{
	glUseProgram(PROGRAM_);
	// const glm::vec3 value = *pair.second;
	// glUniformMatrix4fv(ResourceManager::Get().GetUniformName(pair.first, PROGRAM_), 1, GL_FALSE, &value[0][0]);
	glUniformMatrix4fv(ResourceManager::Get().GetUniformName(pair.first, PROGRAM_), 1, GL_FALSE, &(*pair.second)[0][0]);
	glUseProgram(0);
	CheckGlError();
}

void gl::Material::SetFloat(const std::pair<std::string_view, float> pair) const
{
	glUseProgram(PROGRAM_);
	glUniform1f(ResourceManager::Get().GetUniformName(pair.first, PROGRAM_), pair.second);
	glUseProgram(0);
	CheckGlError();
}

void gl::Material::SetFloat(const std::pair<std::string_view, const float*> pair) const
{
	glUseProgram(PROGRAM_);
	glUniform1f(ResourceManager::Get().GetUniformName(pair.first, PROGRAM_), *pair.second);
	glUseProgram(0);
	CheckGlError();
}

void gl::Material::OnInit() const
{
	glUseProgram(PROGRAM_);
	for (const auto& pair : staticFloats_)
	{
		SetFloat(pair);
	}
	for (const auto& pair : staticInts_)
	{
		SetInt(pair);
	}
	for (const auto& pair : staticMat4s_)
	{
		SetMat4(pair);
	}
	for (const auto& pair : staticVec3s_)
	{
		SetVec3(pair);
	}
	glUseProgram(0);
	CheckGlError();
}

void gl::Material::OnDraw() const
{
	glUseProgram(PROGRAM_);
	for (const auto& pair : dynamicFloats_)
	{
		SetFloat(pair);
	}
	for (const auto& pair : dynamicInts_)
	{
		SetInt(pair);
	}
	for (const auto& pair : dynamicMat4s_)
	{
		SetMat4(pair);
	}
	for (const auto& pair : dynamicVec3s_)
	{
		SetVec3(pair);
	}
	glUseProgram(0);
	CheckGlError();
}