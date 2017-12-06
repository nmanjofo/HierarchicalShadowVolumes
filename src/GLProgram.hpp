#pragma once

#include <GL/glew.h>
#include <map>
#include <glm/glm.hpp>
#include <vector>
#include <string>

class GLProgram
{
public:
	GLProgram();
	~GLProgram();

	void bind();

	void unbind();

	void free();

	//Takes number of shaders as first argument
	//the rest are pairs of shader type () and filepath (const char*)
	bool makeProgram(size_t count, ...);

	void updateUniform(const char* name, float value);
	void updateUniform(const char* name, const glm::mat4& value);
	void updateUniform(const char* name, const glm::vec3& value);

	GLint getUniformLocation(const char* name) const;

private:
	GLuint _program;

	std::map < std::string, std::pair<GLuint, GLenum> > _uniformMap;
	
	void _getAllUniforms();
		
	GLuint _createShader(const char* filepath, const GLenum shaderType, std::string& errorLog) const;
		bool _readWholeFile(const char* file, std::string& content) const;
		void _checkShaderCompileErrors(GLuint shader, std::string& errorLog) const;

	bool _linkProgram(const std::vector<GLuint>& shaders);
		void _checkProgramLinkErrors(std::string& errorLog) const;
};
