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
    if(!shaderPath) { SPDLOG_ERROR("null filepath"); return; }

    // open shader source file
    shaderFile.open(shaderPath);
    if(!shaderFile.is_open()) { SPDLOG_ERROR("no such shader source file ({})", shaderPath); return; }

    // fstream -> sstream -> string -> c_str
    shaderSS << shaderFile.rdbuf();
    shaderFile.close();
    shaderCode = shaderSS.str();
    const GLchar *const pShaderCode = shaderCode.c_str();

    // create and compile shader
    m_shaderID = glCreateShader(type);
    if(!m_shaderID) { SPDLOG_ERROR("failed to create shader"); return; }

    SPDLOG_INFO("compiling shader {} ({})", m_shaderID, shaderPath);
    glShaderSource(m_shaderID, 1, &pShaderCode, nullptr);
    glCompileShader(m_shaderID);
    if(checkCompileError())
    {
        SPDLOG_ERROR("failed to compile shader {} ({})", m_shaderID, shaderPath);
        glDeleteShader(m_shaderID);
        m_shaderID = NULL;
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
        SPDLOG_ERROR("compile error:\n{}", infoLog);
        return true;
    }
}

// ==== shader program class ====

class ShaderProgram
{
    private:
    GLuint m_ShaderProgramID;

    public:
    ShaderProgram(char* vsp, char* fsp, char* gsp);
    ~ShaderProgram() {};

    void use();

    private:
    bool checkLinkError();

    private:
    ShaderProgram() {};
    ShaderProgram(const ShaderProgram& sp) {};
    ShaderProgram& operator=(const ShaderProgram& sp) {};
};

// TODO
// add implemetations of geometry shader and others
ShaderProgram::ShaderProgram(char* vertShaderPath, char* fragShaderPath, char* geoShaderPath = nullptr)
{
    Shader vertShader(vertShaderPath, GL_VERTEX_SHADER), fragShader(fragShaderPath, GL_FRAGMENT_SHADER);
    if(!vertShader.getShaderID()) { SPDLOG_WARN("ignored invalid vertex shader ({})", vertShaderPath); }
    if(!fragShader.getShaderID()) { SPDLOG_WARN("ignored invalid fragment shader ({})", fragShaderPath); }

    m_ShaderProgramID = glCreateProgram();
    if(!m_ShaderProgramID) { SPDLOG_ERROR("failed to create shader program"); return; }

    SPDLOG_INFO("linking shader program {} ({} {})", m_ShaderProgramID, vertShaderPath, fragShaderPath);
    glAttachShader(m_ShaderProgramID, vertShader.getShaderID());
    glAttachShader(m_ShaderProgramID, fragShader.getShaderID());
    glLinkProgram(m_ShaderProgramID);
    if(checkLinkError())
    {
        SPDLOG_ERROR("failed to link shader program {} ({} {})", m_ShaderProgramID, vertShaderPath, fragShaderPath);
        glDeleteProgram(m_ShaderProgramID);
        m_ShaderProgramID = NULL;
        return;
    }
}

void ShaderProgram::use() { glUseProgram(m_ShaderProgramID); }

// check error from glLinkProgram()
// return: true (error occurred), false (no error)
bool ShaderProgram::checkLinkError()
{
    GLint success;

    glGetProgramiv(m_ShaderProgramID, GL_LINK_STATUS, &success);
    if(success) { return false; }
    else
    {
        GLchar infoLog[1024];
        glGetProgramInfoLog(m_ShaderProgramID, 1024, nullptr, infoLog);
        SPDLOG_ERROR("link error:\n{}", m_ShaderProgramID, infoLog);
        return true;
    }
}

#endif