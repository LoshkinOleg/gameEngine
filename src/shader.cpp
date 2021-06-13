#include "shader.h"

#include <iostream>

#include <glad/glad.h>

#include "utility.h"
#include "resource_manager.h"

void gl::Shader::Bind() const
{
	glUseProgram(PROGRAM_);
	CheckGlError(__FILE__, __LINE__);
}
void gl::Shader::Unbind()
{
	glUseProgram(0);
	CheckGlError(__FILE__, __LINE__);
}
void gl::Shader::SetMaterial(MaterialId materialId)
{
	const Material& material = ResourceManager::Get().GetMaterial(materialId);

	const std::array<TextureId, 4> textureIds = material.GetTextures();
	for (const TextureId id : textureIds)
	{
		if (id != UINT_MAX) // Don't try setting textures that aren't used.
		{
			SetTexture(id);
		}
	}

	const auto& colors = material.GetColors();
	SetVec3("material.ambientColor", colors[0]); // TODO: make defines for default shader names and use them here
	SetVec3("material.diffuseColor", colors[1]);
	SetVec3("material.specularColor", colors[2]);

	const float shininess = material.GetShininess();
	SetFloat("material.shininess", shininess);
}
void gl::Shader::SetFloat(const std::string& name, const float value)
{
	const auto match = uniformNames_.find(name);
	if (match != uniformNames_.end()) // Name of uniform already known, use it.
	{
		glUniform1f(match->second, value);
	}
	else
	{
		const int uniformIntName = glGetUniformLocation(PROGRAM_, name.c_str());
		uniformNames_.insert({ name.c_str(), uniformIntName }); // Add the new entry.
		glUniform1f(uniformIntName, value);
	}
	CheckGlError(__FILE__, __LINE__);
}
void gl::Shader::SetVec3(const std::string& name, const glm::vec3 value)
{
	const auto match = uniformNames_.find(name);
	if (match != uniformNames_.end()) // Name of uniform already known, use it.
	{
		glUniform3fv(match->second, 1, &value[0]);
	}
	else
	{
		const int uniformIntName = glGetUniformLocation(PROGRAM_, name.c_str());
		uniformNames_.insert({ name.c_str(), uniformIntName }); // Add the new entry.
		glUniform3fv(uniformIntName, 1, &value[0]);
	}
	CheckGlError(__FILE__, __LINE__);
}
void gl::Shader::SetInt(const std::string& name, const int value)
{
	const auto match = uniformNames_.find(name);
	if (match != uniformNames_.end()) // Name of uniform already known, use it.
	{
		glUniform1i(match->second, value);
	}
	else
	{
		const int uniformIntName = glGetUniformLocation(PROGRAM_, name.c_str());
		uniformNames_.insert({ name.c_str(), uniformIntName }); // Add the new entry.
		glUniform1i(uniformIntName, value);
	}
	CheckGlError(__FILE__, __LINE__);
}
void gl::Shader::SetTexture(TextureId textureId)
{
	const Texture& tex = ResourceManager::Get().GetTexture(textureId);
	const auto& samplerTextureUnitPairs = tex.GetSamplerTextureUnitPairs();

	for (const auto& pair : samplerTextureUnitPairs)
	{
		SetInt(pair.first, pair.second);
	}
}
void gl::Shader::SetProjectionMatrix(const glm::mat4& mat)
{
	SetMat4("projection", mat);
}
void gl::Shader::SetViewMatrix(const glm::mat4& mat)
{
	SetMat4("view", mat);
}
void gl::Shader::SetModelMatrix(const glm::mat4& mat)
{
	SetMat4("model", mat);
}
void gl::Shader::SetMat4(const std::string& name, const glm::mat4& mat)
{
	const auto match = uniformNames_.find(name);
	if (match != uniformNames_.end()) // Name of the uniform already retireved, use it, don't bother the gpu.
	{
		glUniformMatrix4fv(
			match->second,
			1,
			GL_FALSE,
			&mat[0][0]);
	}
	else
	{
		const int uniformIntName = glGetUniformLocation(PROGRAM_, name.c_str());
		uniformNames_.insert({ name, uniformIntName });

		glUniformMatrix4fv(
			uniformIntName,
			1,
			GL_FALSE,
			&mat[0][0]);
	}
	CheckGlError(__FILE__, __LINE__);
}
void gl::Shader::Destroy() const
{
	glDeleteShader(PROGRAM_);
}

void gl::Shader::OnInit(const Model& model)
{
	try
	{
		onInit_(*this, model);
	}
	catch (const std::exception& e)
	{
		std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": invoking onInit_ has thrown an exception: " << e.what() << std::endl;
	}
}

void gl::Shader::OnDraw(const Model& model, const Camera& camera)
{
	try
	{
		onDraw_(*this, model, camera);
	}
	catch (const std::exception& e)
	{
		std::cerr << "ERROR at file: " << __FILE__ << ", line: " << __LINE__ << ": invoking onDraw_ has thrown an exception: " << e.what() << std::endl;
	}
}