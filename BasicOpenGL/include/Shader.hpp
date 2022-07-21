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
    inline void nullify();
    
    public:
    Shader();
    Shader(const char*, GLenum);
    ~Shader();

    public:
    GLuint getShaderID() { return m_shaderID; };

    public:
    void loadFromFile(const char*, GLenum);
    private:
    bool checkCompileError();
    
    private:
    Shader(const Shader& s) {};
    Shader& operator=(const Shader& s) {};
};

inline void Shader::nullify() { m_shaderID = 0; }

Shader::Shader() { nullify(); }

// e.g.) Shader myShader("vertshader.vs", GL_VERTEX_SHADER);
Shader::Shader(const char* shaderPath, GLenum type)
{
    nullify();
    loadFromFile(shaderPath, type);
}

Shader::~Shader()
{
    if(m_shaderID)
    {
        glDeleteShader(m_shaderID);
        nullify();
    }
}

// e.g.) Shader myShader.loadFromFile("vertshader.vs", GL_VERTEX_SHADER);
void Shader::loadFromFile(const char* shaderPath, GLenum type)
{
    // local vars
    std::ifstream shaderFile;
    std::stringstream shaderSS;
    std::string shaderCode;
    const GLchar* pShaderCode = nullptr;

    // check filepath
    if(!shaderPath) { SPDLOG_ERROR("Shader::loadFromFile(nullptr, type={}): null filepath", type); return; }
    SPDLOG_INFO("Shader::loadFromFile(\"{}\", type={})", shaderPath, type);

    // delete existing shader
    if(m_shaderID)
    {
        SPDLOG_WARN("delete existing shader (ShaderID={})", m_shaderID);
        glDeleteShader(m_shaderID);
        nullify();
    }

    // open shader source file
    shaderFile.open(shaderPath);
    if(!shaderFile.is_open()) { SPDLOG_ERROR("no such shader source file"); return; }

    // fstream -> sstream -> string -> c_str
    shaderSS << shaderFile.rdbuf();
    shaderFile.close();
    shaderCode = shaderSS.str();
    pShaderCode = shaderCode.c_str();

    // create and compile shader
    switch (type)
    {
    case GL_VERTEX_SHADER:
    case GL_FRAGMENT_SHADER:
    case GL_GEOMETRY_SHADER:
        m_shaderID = glCreateShader(type);
        if(!m_shaderID) { SPDLOG_ERROR("failed to create shader"); return; }
        break;

    default:
        SPDLOG_ERROR("wrong or unimplemented type");
        return;
    }

    glShaderSource(m_shaderID, 1, &pShaderCode, nullptr);
    glCompileShader(m_shaderID);
    if(checkCompileError())
    {
        glDeleteShader(m_shaderID);
        nullify();
        return;
    }

    SPDLOG_INFO("ShaderID = {}", m_shaderID);
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
    GLuint m_shaderProgramID;
    inline void nullify();

    public:
    ShaderProgram();
    ShaderProgram(const char*, const char*, const char*);
    ~ShaderProgram();

    public:
    GLuint getShaderProgramID() { return m_shaderProgramID; };

    void use();

    void setBool(const char*, bool);
    void setInt(const char*, int);
    void setFloat(const char*, float);

    void setVec2(const char*, float, float);
    void setVec2(const char*, glm::vec2&);

    void setVec3(const char*, float, float, float);
    void setVec3(const char*, glm::vec3&);

    void setVec4(const char*, float, float, float, float);
    void setVec4(const char*, glm::vec4&);

    void setMat2(const char*, glm::mat2&);
    void setMat3(const char*, glm::mat3&);
    void setMat4(const char*, glm::mat4&);

    void setSampler(const char*, int);

    public:
    void loadFromFile(const char*, const char*, const char*);
    private:
    bool checkLinkError();

    private:
    ShaderProgram(const ShaderProgram& sp) {};
    ShaderProgram& operator=(const ShaderProgram& sp) {};
};

inline void ShaderProgram::nullify() { m_shaderProgramID = NULL; }

ShaderProgram::ShaderProgram() { nullify(); }

ShaderProgram::ShaderProgram(const char* vertShaderPath, const char* fragShaderPath, const char* geoShaderPath)
{
    nullify();
    loadFromFile(vertShaderPath, fragShaderPath, geoShaderPath);
}

ShaderProgram::~ShaderProgram()
{
    if(m_shaderProgramID)
    {
        glDeleteProgram(m_shaderProgramID);
        nullify();
    }
}

void ShaderProgram::use() { glUseProgram(m_shaderProgramID); }

void ShaderProgram::setBool(const char* name, bool b) { glUniform1i(glGetUniformLocation(m_shaderProgramID, name), (int)b); };
void ShaderProgram::setInt(const char* name, int i) { glUniform1i(glGetUniformLocation(m_shaderProgramID, name), i); };
void ShaderProgram::setFloat(const char* name, float f) { glUniform1f(glGetUniformLocation(m_shaderProgramID, name), f); };

void ShaderProgram::setVec2(const char* name, float x, float y) { glUniform2f(glGetUniformLocation(m_shaderProgramID, name), x, y); };
void ShaderProgram::setVec2(const char* name, glm::vec2& v2) { glUniform2fv(glGetUniformLocation(m_shaderProgramID, name), 1, &v2[0]); };

void ShaderProgram::setVec3(const char* name, float x, float y, float z) { glUniform3f(glGetUniformLocation(m_shaderProgramID, name), x, y, z); };
void ShaderProgram::setVec3(const char* name, glm::vec3& v3) { glUniform3fv(glGetUniformLocation(m_shaderProgramID, name), 1, &v3[0]); };

void ShaderProgram::setVec4(const char* name, float x, float y, float z, float w) { glUniform4f(glGetUniformLocation(m_shaderProgramID, name), x, y, z, w); };
void ShaderProgram::setVec4(const char* name, glm::vec4& v4) { glUniform4fv(glGetUniformLocation(m_shaderProgramID, name), 1, &v4[0]); };

void ShaderProgram::setMat2(const char* name, glm::mat2& m2) { glUniformMatrix2fv(glGetUniformLocation(m_shaderProgramID, name), 1, GL_FALSE, &m2[0][0]); };
void ShaderProgram::setMat3(const char* name, glm::mat3& m3) { glUniformMatrix3fv(glGetUniformLocation(m_shaderProgramID, name), 1, GL_FALSE, &m3[0][0]); };
void ShaderProgram::setMat4(const char* name, glm::mat4& m4) { glUniformMatrix4fv(glGetUniformLocation(m_shaderProgramID, name), 1, GL_FALSE, &m4[0][0]); };

void ShaderProgram::setSampler(const char* name, int i) { setInt(name, i); }

void ShaderProgram::loadFromFile(const char* vertShaderPath, const char* fragShaderPath, const char* geomShaderPath)
{
    SPDLOG_INFO("ShaderProgram::loadFromFile(...)");

    // local vars
    Shader vertShader, fragShader, geomShader;

    // delete existing shader
    if(m_shaderProgramID)
    {
        SPDLOG_WARN("delete existing shader program (ShaderProgramID={})", m_shaderProgramID);
        glDeleteShader(m_shaderProgramID);
        nullify();
    }

    // prepare shaders
    vertShader.loadFromFile(vertShaderPath, GL_VERTEX_SHADER);
    fragShader.loadFromFile(fragShaderPath, GL_FRAGMENT_SHADER);
    geomShader.loadFromFile(geomShaderPath, GL_GEOMETRY_SHADER);

    // create shader program
    m_shaderProgramID = glCreateProgram();
    if(!m_shaderProgramID) { SPDLOG_ERROR("failed to create shader program"); return; }

    // attach shaders and link shader program
    glAttachShader(m_shaderProgramID, vertShader.getShaderID());
    glAttachShader(m_shaderProgramID, fragShader.getShaderID());
    glAttachShader(m_shaderProgramID, geomShader.getShaderID());
    glLinkProgram(m_shaderProgramID);
    if(checkLinkError())
    {
        SPDLOG_ERROR("failed to link shader program");
        glDeleteProgram(m_shaderProgramID);
        nullify();
        return;
    }
    
    SPDLOG_INFO("ShaderProgramID = {}", m_shaderProgramID);
}

// check error from glLinkProgram()
// return: true (error occurred), false (no error)
bool ShaderProgram::checkLinkError()
{
    GLint success;

    glGetProgramiv(m_shaderProgramID, GL_LINK_STATUS, &success);
    if(success) { return false; }
    else
    {
        GLchar infoLog[1024];
        glGetProgramInfoLog(m_shaderProgramID, 1024, nullptr, infoLog);
        SPDLOG_ERROR("link error:\n{}", infoLog);
        return true;
    }
}

#endif