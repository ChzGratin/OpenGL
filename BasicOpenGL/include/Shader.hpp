#ifndef _SHADER_
#define _SHADER_

// spdlog
#include <spdlog/spdlog.h>

// opengl
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// glm
#include <glm/glm.hpp>

// std
#include <fstream>
#include <sstream>
#include <string>

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

// e.g.) Shader myS("vertshader.vs", GL_VERTEX_SHADER);
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

    void setBool(char* n, bool b);
    void setInt(char* n, int i);
    void setFloat(char* n, float f);

    void setVec2(char* n, float x, float y);
    void setVec2(char* n, glm::vec2 v2);

    void setVec3(char* n, float x, float y, float z);
    void setVec3(char* n, glm::vec3 v3);

    void setVec4(char* n, float x, float y, float z, float w);
    void setVec4(char* n, glm::vec4 v4);

    void setMat2(char* n, glm::mat2 m2);
    void setMat3(char* n, glm::mat3 m3);
    void setMat4(char* n, glm::mat4 m4);

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
    // prepare shaders
    Shader vertShader(vertShaderPath, GL_VERTEX_SHADER);
    Shader fragShader(fragShaderPath, GL_FRAGMENT_SHADER);
    if(!vertShader.getShaderID()) { SPDLOG_WARN("ignored invalid vertex shader ({})", vertShaderPath); }
    if(!fragShader.getShaderID()) { SPDLOG_WARN("ignored invalid fragment shader ({})", fragShaderPath); }

    // create shader program
    m_ShaderProgramID = glCreateProgram();
    if(!m_ShaderProgramID) { SPDLOG_ERROR("failed to create shader program"); return; }

    // attach shaders and link shader program
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

void ShaderProgram::setBool(char* name, bool b) { glUniform1i(glGetUniformLocation(m_ShaderProgramID, name), (int)b); };
void ShaderProgram::setInt(char* name, int i) { glUniform1i(glGetUniformLocation(m_ShaderProgramID, name), i); };
void ShaderProgram::setFloat(char* name, float f) { glUniform1f(glGetUniformLocation(m_ShaderProgramID, name), f); };

void ShaderProgram::setVec2(char* name, float x, float y) { glUniform2f(glGetUniformLocation(m_ShaderProgramID, name), x, y); };
void ShaderProgram::setVec2(char* name, glm::vec2 v2) { glUniform2fv(glGetUniformLocation(m_ShaderProgramID, name), 1, &v2[0]); };

void ShaderProgram::setVec3(char* name, float x, float y, float z) { glUniform3f(glGetUniformLocation(m_ShaderProgramID, name), x, y, z); };
void ShaderProgram::setVec3(char* name, glm::vec3 v3) { glUniform3fv(glGetUniformLocation(m_ShaderProgramID, name), 1, &v3[0]); };

void ShaderProgram::setVec4(char* name, float x, float y, float z, float w) { glUniform4f(glGetUniformLocation(m_ShaderProgramID, name), x, y, z, w); };
void ShaderProgram::setVec4(char* name, glm::vec4 v4) { glUniform4fv(glGetUniformLocation(m_ShaderProgramID, name), 1, &v4[0]); };

void ShaderProgram::setMat2(char* name, glm::mat2 m2) { glUniformMatrix2fv(glGetUniformLocation(m_ShaderProgramID, name), 1, GL_FALSE, &m2[0][0]); };
void ShaderProgram::setMat3(char* name, glm::mat3 m3) { glUniformMatrix3fv(glGetUniformLocation(m_ShaderProgramID, name), 1, GL_FALSE, &m3[0][0]); };
void ShaderProgram::setMat4(char* name, glm::mat4 m4) { glUniformMatrix4fv(glGetUniformLocation(m_ShaderProgramID, name), 1, GL_FALSE, &m4[0][0]); };

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