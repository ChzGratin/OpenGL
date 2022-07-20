#ifndef _MESH_
#define _MESH_

// spdlog
#include <spdlog/spdlog.h>

// glm
#include <glm/glm.hpp>

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// opengl
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// include
#include <Shader.hpp>

// std
#include <vector>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

struct Texture
{
    // texture type
    enum TYPE
    {
        DIFFUSE = aiTextureType_DIFFUSE,
        SPECULAR = aiTextureType_SPECULAR,
        NORMAL = aiTextureType_NORMALS,
        HEIGHT = aiTextureType_HEIGHT
    };

    GLuint textureID; // Image.m_imageID
    int type;
};

class Mesh
{
    private:
    GLuint m_VAO, m_VBO, m_EBO;
    GLsizei m_numIndices;
    std::vector<Texture> m_textures;
    inline void nullify();

    public:
    Mesh();
    Mesh(std::vector<Vertex>&, std::vector<unsigned int>&, std::vector<Texture>&);
    ~Mesh();

    public:
    void load(std::vector<Vertex>&, std::vector<unsigned int>&, std::vector<Texture>&);
    void draw(ShaderProgram&);

    private:
    Mesh(const Mesh& m) {};
    Mesh& operator=(const Mesh& m) {};
};

void Mesh::nullify()
{
    m_VAO = m_VBO = m_EBO = 0;
    std::vector<Texture>().swap(m_textures); // anonymous object
}

Mesh::Mesh() { nullify(); }

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures)
{
    nullify();
    load(vertices, indices, textures);
}

Mesh::~Mesh()
{
    if(m_VAO)
    {
        glDeleteVertexArrays(1, &m_VAO);
        if(m_VBO) { glDeleteBuffers(1, &m_VBO); }
        if(m_EBO) { glDeleteBuffers(1, &m_EBO); }
        nullify();
    }
}

void Mesh::load(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures)
{
    SPDLOG_INFO("Mesh::load()");

    // delete existing mesh
    if(m_VAO)
    {
        SPDLOG_WARN("delete existing Mesh (VAO={})", m_VAO);
        glDeleteVertexArrays(1, &m_VAO);
        if(m_VBO) { glDeleteBuffers(1, &m_VBO); }
        if(m_EBO) { glDeleteBuffers(1, &m_EBO); }
        nullify();
    }

    // generate VAO, EBO, and EBO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    if(!(m_VAO && m_VBO && m_EBO))
    {
        SPDLOG_ERROR("failed to generate VAO, VBO, or EBO");
        if(m_VAO) { glDeleteVertexArrays(1, &m_VAO); }
        if(m_VBO) { glDeleteBuffers(1, &m_VBO); }
        if(m_EBO) { glDeleteBuffers(1, &m_EBO); }
        nullify();
        return;
    }

    // bind VAO
    glBindVertexArray(m_VAO);

    // bind and buffer VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
    glEnableVertexAttribArray(4);

    // bind and buffer EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    m_numIndices = static_cast<GLuint>(indices.size());

    // store textures (ID and type)
    m_textures = textures;

    // unbind VAO
    glBindVertexArray(0);

    SPDLOG_INFO("Mesh.VAO={}", m_VAO);
}

// call this method after calling shaderProgram.use()
//
// uniform sampler2D in shaderProgram:
// diffuseMap<n>, specularMap<n>, normalMap<n>, heightMap<n> (n = 0, 1, 2, ...)
void Mesh::draw(ShaderProgram& shaderProgram)
{
    // local vars
    int numBoundedTexture, numDiffuse, numSpecular, numNormal, numHeight;
    numBoundedTexture = numDiffuse = numSpecular = numNormal = numHeight = 0;

    // bind textures
    for(int i = 0; i < m_textures.size(); i++)
    {
        std::string uniformName;
        switch (m_textures[i].type)
        {
        case Texture::TYPE::DIFFUSE:
            uniformName = "diffuseMap";
            uniformName += std::to_string(numDiffuse++);
            break;
        case Texture::TYPE::SPECULAR:
            uniformName = "specularMap";
            uniformName += std::to_string(numSpecular++);
            break;
        case Texture::TYPE::NORMAL:
            uniformName = "normalMap";
            uniformName += std::to_string(numNormal++);
            break;
        case Texture::TYPE::HEIGHT:
            uniformName = "heightMap";
            uniformName += std::to_string(numHeight++);
            break;
        default:
            SPDLOG_WARN("wrong or unimplemented texture type");
            continue;
        }
        
        // select texture unit and bind texture
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_textures[i].textureID);
        numBoundedTexture++;

        // assign the number of texture unit to uniform
        shaderProgram.setSampler(uniformName.c_str(), i);
    }

    // draw mesh
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); // unbind VAO

    // set all texture units to defaults
    for(int i = 0; i < numBoundedTexture; i++) { glActiveTexture(GL_TEXTURE0 + i); }
}

#endif