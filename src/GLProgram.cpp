#include "GLProgram.hpp"

#include <fstream>
#include <string>
#include <cstdarg>
#include <sstream>
#include <iterator>
#include <vector>
#include <iostream>
#include <glm/gtc/type_ptr.inl>

GLProgram::GLProgram()
{
	_program = 0;
}

GLProgram::~GLProgram()
{
	free();
}

void GLProgram::free()
{
	if (_program)
	{
		glDeleteProgram(_program);
		_program = 0;
		_uniformMap.clear();
	}
}

void GLProgram::bind()
{
	assert(_program != 0);

	glUseProgram(_program);
}

void GLProgram::unbind()
{
	glUseProgram(0);
}


bool GLProgram::makeProgram(size_t count, ...)
{
	va_list args;
	va_start(args, 2*count);

	bool isOK = true;

	std::vector<GLuint> shaders;

	for (size_t i = 0; i < count; ++i)
	{
		const GLenum shaderType = va_arg(args, GLenum);
		const char* path = va_arg(args, const char*);

		std::string errorLog;
		GLuint shader = _createShader(path, shaderType, errorLog);

		if (!shader)
		{
			std::cerr << "Failed to compile shader " << std::string(path) << std::endl << errorLog << std::endl << std::endl;
			isOK = false;
		}
		else
			shaders.push_back(shader);
	}
	va_end(args);

	if (!isOK)
		return false;

	if (!_linkProgram(shaders))
		return false;

	_getAllUniforms();

	return true;
}

GLuint GLProgram::_createShader(const char* filepath, const GLenum shaderType, std::string& errorLog) const
{
	GLuint shader = glCreateShader(shaderType);
	if (shader == 0)
	{
		errorLog = "glCreateShader failed\n";
		return false;
	}

	std::string src;

	if (!_readWholeFile(filepath, src))
	{
		std::stringstream sstr;
		sstr << "Failed to open shader file: " << std::string(filepath) << std::endl;
		errorLog = sstr.str();
		return 0;
	}

	const char* cstr = src.c_str();
	glShaderSource(shader, 1, &cstr, NULL);
	glCompileShader(shader);

	_checkShaderCompileErrors(shader, errorLog);

	if(errorLog.size())
	{
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

void GLProgram::_checkShaderCompileErrors(GLuint shader, std::string& errorLog) const
{
	GLint result = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE)
	{
		int loglen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &loglen);

		if (loglen>0)
		{
			char* log = new char[loglen];

			glGetShaderInfoLog(shader, loglen, nullptr, log);
			errorLog.assign(log);

			delete[] log;
		}
	}
}

bool GLProgram::_readWholeFile(const char* file, std::string& content) const
{
	std::ifstream stream(file);

	if (stream.fail())
		return false;

	content = std::string(std::istream_iterator<char>(stream >> std::noskipws), std::istream_iterator<char>());

	return true;
}

bool GLProgram::_linkProgram(const std::vector<GLuint>& shaders)
{
	_program = glCreateProgram();
	
	for(const auto shader : shaders)
		glAttachShader(_program, shader);

	glLinkProgram(_program);

	std::string errorLog;
	_checkProgramLinkErrors(errorLog);

	if(errorLog.size()!=0)
	{
		std::cerr << "Failed to link program!\n" << errorLog << std::endl << std::endl;

		return false;
	}

	return true;
}

void GLProgram::_checkProgramLinkErrors(std::string& errorLog) const
{
	int status = 0;
	glGetProgramiv(_program, GL_LINK_STATUS, &status);

	if (status == GL_FALSE)
	{
		int loglen = 0;
		glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &loglen);

		if (loglen>0)
		{
			char* log = new char[loglen];

			glGetProgramInfoLog(_program, loglen, nullptr, log);
			errorLog.assign(log);

			delete[] log;
		}
	}
}

void GLProgram::_getAllUniforms()
{
	assert(_program != 0);
	assert(glGetError() == GL_NO_ERROR);
	GLint uniformCount;
	glGetProgramiv(_program, GL_ACTIVE_UNIFORMS, &uniformCount);

	for (int i = 0; i < uniformCount; i++)
	{
		char uniformName[20];
		int size;
		int length;
		GLenum type;

		glGetActiveUniform(_program, i, 20, &length, &size, &type, uniformName);
		
		_uniformMap[std::string(uniformName)] = std::make_pair(i, type);
	}
	assert(glGetError() == GL_NO_ERROR);
}

void GLProgram::updateUniform(const char* name, float value)
{
	assert(_program != 0);
	assert(glGetError() == GL_NO_ERROR);
	auto findIter = _uniformMap.find(std::string(name));

	if(findIter!=_uniformMap.end() && findIter->second.second == GL_FLOAT)
	{
		glUniform1f(findIter->second.first, value);
	}
	assert(glGetError() == GL_NO_ERROR);
}

void GLProgram::updateUniform(const char* name, const glm::mat4& value)
{
	assert(_program != 0);
	assert(glGetError() == GL_NO_ERROR);
	auto findIter = _uniformMap.find(std::string(name));

	if (findIter != _uniformMap.end() && findIter->second.second == GL_FLOAT_MAT4)
	{
		glUniformMatrix4fv(findIter->second.first, 1, GL_FALSE, glm::value_ptr(value));
	}
	assert(glGetError() == GL_NO_ERROR);
}

void GLProgram::updateUniform(const char* name, const glm::vec3& value)
{
	assert(_program != 0);
	assert(glGetError() == GL_NO_ERROR);
	auto findIter = _uniformMap.find(std::string(name));

	if (findIter != _uniformMap.end() && findIter->second.second == GL_FLOAT_VEC3)
	{
		glUniform3fv(findIter->second.first, 1, glm::value_ptr(value));
	}
	assert(glGetError() == GL_NO_ERROR);
}