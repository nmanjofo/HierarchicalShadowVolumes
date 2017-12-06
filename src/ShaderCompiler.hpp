#pragma once

#include <fstream>
#include <string>
#include <cstdarg>
#include <sstream>

#include <GL/glew.h>

class ShaderCompiler
{
public:
	//Compiles a single shader from a file
    //errorLog - reference to a string, where error message will be stored, in case the compilation fails
    bool compileShader(GLuint& shader, const char* filepath, const GLenum shaderType, std::string& errorLog);

    //Links shaders to a program
    //program - reference to a program, into which the shaders will be linked
    //errorLog - error log, if return value is false, contains log data
    //count - number of shaders to link
    //the rest are the shaders, the function has variable amount of arguments
    bool linkProgram(GLuint& program, std::string& errorLog, size_t count, ...);

private:
    bool _loadFile(const char* file, std::string& content);
};