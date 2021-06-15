#pragma once
#include <string>
#include <functional>
#include <map>

#include <glm/glm.hpp>

#include "defines.h"

namespace gl {

	using MaterialId = unsigned int;
	using TextureId = unsigned int;
	using ShaderId = unsigned int;

	class Model;
	class Camera;

	class Shader
	{
	public:
		void Bind() const;
		static void Unbind();
		void OnInit(const Model& model);
		void OnDraw(const Model& model, const Camera& camera);
		void SetMaterial(MaterialId materialId);
		void SetFloat(const std::string& name, const float value);
		void SetVec3(const std::string& name, const glm::vec3 value);
		void SetInt(const std::string& name, const int value);
		void SetBool(const std::string& name, bool value);
		void SetTexture(TextureId textureId);
		void SetProjectionMatrix(const glm::mat4& mat);
		void SetViewMatrix(const glm::mat4& mat);
		void SetModelMatrix(const glm::mat4& mat);
		void SetMat4(const std::string& name, const glm::mat4& mat);
	private:
		friend class ResourceManager;

		ShaderId id_ = DEFAULT_ID;
		unsigned int PROGRAM_ = 0;
		std::function<void(Shader&, const Model&)> onInit_ = nullptr;
		std::function<void(Shader&, const Model&, const Camera&)> onDraw_ = nullptr;
		// TODO: move this map to resource manager so that the Shader can be passed everywhere as const as well.
		std::map<std::string, int> uniformNames_ = {};
	};

	//class Shader
	//{
	//public:
	//	unsigned int id = 0;
	//	// constructor generates the shader on the fly
	//	Shader() {};
	//	Shader(
	//		const std::string& vertexPath, 
	//		const std::string& fragmentPath)
	//	{
	//		// 1. retrieve the vertex/fragment source code from filePath
	//		std::string vertexCode;
	//		std::string fragmentCode;
	//		std::ifstream vShaderFile;
	//		std::ifstream fShaderFile;
	//		// ensure ifstream objects can throw exceptions:
	//		vShaderFile.exceptions(
	//			std::ifstream::failbit | std::ifstream::badbit);
	//		fShaderFile.exceptions(
	//			std::ifstream::failbit | std::ifstream::badbit);
	//		try
	//		{
	//			// open files
	//			vShaderFile.open(vertexPath);
	//			fShaderFile.open(fragmentPath);
	//			std::stringstream vShaderStream, fShaderStream;
	//			// read file's buffer contents into streams
	//			vShaderStream << vShaderFile.rdbuf();
	//			fShaderStream << fShaderFile.rdbuf();
	//			// close file handlers
	//			vShaderFile.close();
	//			fShaderFile.close();
	//			// convert stream into string
	//			vertexCode = vShaderStream.str();
	//			fragmentCode = fShaderStream.str();
	//		}
	//		catch (std::ifstream::failure& e)
	//		{
	//			std::cerr << "ERROR: Failed to open shader source!" << std::endl;
	//			throw std::runtime_error(e.what());
	//		}
	//		const char* vShaderCode = vertexCode.c_str();
	//		const char* fShaderCode = fragmentCode.c_str();
	//		// 2. compile shaders
	//		unsigned int vertex, fragment;
	//		// vertex shader
	//		vertex = glCreateShader(GL_VERTEX_SHADER);
	//		IsError(__FILE__, __LINE__);
	//		glShaderSource(vertex, 1, &vShaderCode, NULL);
	//		IsError(__FILE__, __LINE__);
	//		glCompileShader(vertex);
	//		IsError(__FILE__, __LINE__);
	//		CheckCompileErrors(vertex, "VERTEX");
	//		// fragment Shader
	//		fragment = glCreateShader(GL_FRAGMENT_SHADER);
	//		IsError(__FILE__, __LINE__);
	//		glShaderSource(fragment, 1, &fShaderCode, NULL);
	//		IsError(__FILE__, __LINE__);
	//		glCompileShader(fragment);
	//		IsError(__FILE__, __LINE__);
	//		CheckCompileErrors(fragment, "FRAGMENT");
	//		// shader Program
	//		id = glCreateProgram();
	//		IsError(__FILE__, __LINE__);
	//		glAttachShader(id, vertex);
	//		IsError(__FILE__, __LINE__);
	//		glAttachShader(id, fragment);
	//		IsError(__FILE__, __LINE__);
	//		glLinkProgram(id);
	//		IsError(__FILE__, __LINE__);
	//		CheckCompileErrors(id, "PROGRAM");
	//		// delete the shaders as they're linked into our program now and no
	//		// longer necessary
	//		glDeleteShader(vertex);
	//		IsError(__FILE__, __LINE__);
	//		glDeleteShader(fragment);
	//		IsError(__FILE__, __LINE__);
	//	}
	//	// activate the shader
	//	void Bind()
	//	{
	//		glUseProgram(id);
	//		IsError(__FILE__, __LINE__);
	//	}
	//	static void UnBind()
	//	{
	//		glUseProgram(0);
	//		IsError(__FILE__, __LINE__);
	//	}
	//	void Destroy()
	//	{
	//		glDeleteShader(id);
	//	}
	//	// utility uniform functions
	//	void SetBool(const std::string& name, bool value) const
	//	{
	//		glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
	//		IsError(__FILE__, __LINE__);
	//	}
	//	void SetInt(const std::string& name, int value) const
	//	{
	//		glUniform1i(glGetUniformLocation(id, name.c_str()), value);
	//		IsError(__FILE__, __LINE__);
	//	}
	//	void SetFloat(const std::string& name, float value) const
	//	{
	//		glUniform1f(glGetUniformLocation(id, name.c_str()), value);
	//		IsError(__FILE__, __LINE__);
	//	}
	//	void SetVec2(const std::string& name, const glm::vec2& value) const
	//	{
	//		glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	//		IsError(__FILE__, __LINE__);
	//	}
	//	void SetVec2(const std::string& name, float x, float y) const
	//	{
	//		glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
	//		IsError(__FILE__, __LINE__);
	//	}
	//	void SetVec3(const std::string& name, const glm::vec3& value) const
	//	{
	//		glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	//		IsError(__FILE__, __LINE__);
	//	}
	//	void SetVec3(const std::string& name, float x, float y, float z) const
	//	{
	//		glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
	//		IsError(__FILE__, __LINE__);
	//	}
	//	void SetVec4(const std::string& name, const glm::vec4& value) const
	//	{
	//		glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	//		IsError(__FILE__, __LINE__);
	//	}
	//	void SetVec4(
	//		const std::string& name, 
	//		float x, float y, float z, float w)
	//	{
	//		glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
	//		IsError(__FILE__, __LINE__);
	//	}
	//	void SetMat2(const std::string& name, const glm::mat2& mat) const
	//	{
	//		glUniformMatrix2fv(
	//			glGetUniformLocation(id, name.c_str()), 
	//			1, 
	//			GL_FALSE, 
	//			&mat[0][0]);
	//		IsError(__FILE__, __LINE__);
	//	}
	//	void SetMat3(const std::string& name, const glm::mat3& mat) const
	//	{
	//		glUniformMatrix3fv(
	//			glGetUniformLocation(id, name.c_str()), 
	//			1, 
	//			GL_FALSE, 
	//			&mat[0][0]);
	//		IsError(__FILE__, __LINE__);
	//	}
	//	void SetMat4(const std::string& name, const glm::mat4& mat) const
	//	{
	//		glUniformMatrix4fv(
	//			glGetUniformLocation(id, name.c_str()), 
	//			1, 
	//			GL_FALSE, 
	//			&mat[0][0]);
	//		IsError(__FILE__, __LINE__);
	//	}

	//private:
	//	// utility function for checking shader compilation/linking errors.
	//	void CheckCompileErrors(GLuint shader, std::string type)
	//	{
	//		GLint success;
	//		GLchar infoLog[1024];
	//		if (type != "PROGRAM")
	//		{
	//			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	//			if (!success)
	//			{
	//				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
	//				throw std::runtime_error(
	//					"Shader compilation error: " + type + " : " + infoLog);
	//			}
	//		}
	//		else
	//		{
	//			glGetProgramiv(shader, GL_LINK_STATUS, &success);
	//			if (!success)
	//			{
	//				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
	//				throw std::runtime_error(
	//					"Shader linking error: " + type + " : " + infoLog);
	//			}
	//		}
	//	}
	//	static void IsError(const char* file, int line) {
	//		auto error_code = glGetError();
	//		if (error_code != GL_NO_ERROR)
	//		{
	//			throw std::runtime_error(
	//				std::to_string(error_code) +
	//				" in file: " + file +
	//				" at line: " + std::to_string(line));
	//		}
	//	}
	//};

} // End namespace gl.
