#include "ShaderCompiler.hpp"

#include <iterator>

bool ShaderCompiler::compileShader(GLuint& shader, const char* filepath, const GLenum shaderType, std::string& errorLog)
{
    shader = glCreateShader(shaderType);
    if(shader == 0)
    {
        errorLog = "glCreateShader failed\n";
        return false;
    }

    std::string src;

    if(!_loadFile(filepath, src))
    {
        std::stringstream sstr;
        sstr << "Failed to open shader file: " << std::string(filepath) << std::endl;
        errorLog = sstr.str();
        return false;
    }

    const char* cstr = src.c_str();
    glShaderSource(shader, 1, &cstr, NULL);
    glCompileShader(shader);

    //Compilation log
    GLint result = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if(result==GL_FALSE)
    {
        int loglen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &loglen);

        if(loglen>0)
        {
            char* log = new char[loglen];

            glGetShaderInfoLog(shader, loglen, nullptr, log);
            errorLog.assign(log);

            delete[] log;
        }

        return false;
    }

    return true;
}

bool ShaderCompiler::linkProgram(GLuint& program, std::string& errorLog, size_t count, ...)
{
    program = glCreateProgram();

    if(program == 0)
    {
        errorLog = "glCreateProgram failed!\n";
        return false;
    }

    va_list args;
    va_start(args, count);
    for(size_t i = 0; i < count; ++i)
    {
        glAttachShader(program, va_arg(args, GLuint));
    }
    va_end(args);

    glLinkProgram(program);

    //Check link
    int status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    if(status==GL_FALSE)
    {
        int loglen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &loglen);

        if(loglen>0)
        {
            char* log = new char[loglen];

            glGetProgramInfoLog(program, loglen, nullptr, log);
            errorLog.assign(log);

            delete[] log;
        }

        return false;
    }

    return true;
}

bool ShaderCompiler::_loadFile(const char* file, std::string& content)
{
    std::ifstream stream(file);

    if(stream.fail())
        return false;

    content =  std::string(std::istream_iterator<char>(stream >> std::noskipws), std::istream_iterator<char>());
    return true;
}