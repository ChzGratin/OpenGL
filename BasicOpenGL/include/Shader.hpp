#ifndef _SHADER_
#define _SHADER_

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

// ==== shader class ====

class Shader
{
    private:
    GLuint m_shaderID;

    public:
    Shader(char* sp, GLenum t);
    ~Shader();

    public:
    GLuint getShaderID() { return m_shaderID; };

    private:
    bool checkCompileError();

    private:
    Shader() {};
    Shader(const Shader& s) {};
    Shader& operator=(const Shader& s) {};
};

Shader::Shader(char* shaderPath, GLenum type)
{
    // local vars
    std::ifstream shaderFile;
    std::stringstream shaderSS;
    std::string shaderCode;

    // init member
    m_shaderID = NULL;

    // check filepath
    if(!shaderPath) { SPDLOG_ERROR("invalid filepath"); return; }

    // open file
    shaderFile.open(shaderPath);
    if(!shaderFile.is_open()) { SPDLOG_ERROR("no such shader file"); return; }

    // fstream -> sstream -> string -> c_str
    shaderSS << shaderFile.rdbuf();
    shaderFile.close();
    shaderCode = shaderSS.str();
    const GLchar *const pShaderCode = shaderCode.c_str();

    // create and compile shader
    m_shaderID = glCreateShader(type);
    if(!m_shaderID) { SPDLOG_ERROR("failed to create shader"); return; }

    glShaderSource(m_shaderID, 1, &pShaderCode, nullptr);
    glCompileShader(m_shaderID);
    if(checkCompileError())
    {
        glDeleteShader(m_shaderID);
        m_shaderID = NULL;
        SPDLOG_ERROR("failed to compile shader");
        return;
    }
}

Shader::~Shader()
{
    if(m_shaderID)
    {
        glDeleteShader(m_shaderID);
        m_shaderID = NULL;
    }
}

// check error from glCompileShader()
// return: true (error occurred), false (no error)
bool Shader::checkCompileError()
{
    GLint success;

    glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &success);
    if(success) { return false; }
    else
    {
        GLchar infoLog[1024];
        glGetShaderInfoLog(m_shaderID, 1024, nullptr, infoLog);
        SPDLOG_ERROR("shader info log:\n{}", infoLog);
        return true;
    }
}

// ==== shader program class ====

class ShaderProgram
{
    private:
    int m_ShaderProgramID;

    public:
    ShaderProgram(char* vsp, char* fsp, char* gsp);
    ~ShaderProgram() {};

    void use();

    private:
    ShaderProgram() {};
    ShaderProgram(const ShaderProgram& sp) {};
    ShaderProgram& operator=(const ShaderProgram& sp) {};
};

ShaderProgram::ShaderProgram(char* vertShaderPath, char* fragShaderPath, char* geoShaderPath = nullptr)
{
    Shader vertShader(vertShaderPath, GL_VERTEX_SHADER), fragShader(fragShaderPath, GL_FRAGMENT_SHADER);
}

void ShaderProgram::use() {}

#endif