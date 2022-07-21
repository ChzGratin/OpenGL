#ifndef _MODEL_
#define _MODEL_

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// include
#include <Shader.hpp>
#include <Image.hpp>
#include <Mesh.hpp>

// std
#include <stdio.h>

class Model
{
    private:
    // IMPORTANT: when you use a std::vector<T>, T must be...
    // CopyInsertable and MoveInsertable ( push_back() )
    // MoveInsertable and EmplaceConstructible ( emplace_back() )
    std::vector<Mesh*> m_meshes;
    std::vector<Image*> m_images;
    inline void nullify();

    public:
    Model();
    Model(const char*);
    ~Model();

    public:
    void loadFromFile(const char*);
    void draw(ShaderProgram&);
    private:
    void loadVertices(aiMesh*, std::vector<Vertex>&);
    void loadIndices(aiMesh*, std::vector<unsigned int>&);
    void loadTextures(aiMesh*, std::vector<Texture>&, aiMaterial**, std::string&);
    void loadTextureByType(std::vector<Texture>&, aiMaterial**, unsigned int, aiTextureType, std::string&);

    private:
    Model(const Model&) {};
    Model& operator=(const Model&) {};
};

void Model::nullify()
{
    for(size_t i = 0; i < m_meshes.size(); i++) { delete m_meshes[i]; }
    for(size_t i = 0; i < m_images.size(); i++) { delete m_images[i]; }
    std::vector<Mesh*>().swap(m_meshes);
    std::vector<Image*>().swap(m_images);
}

Model::Model() { nullify(); }

Model::Model(const char* modelPath)
{
    nullify();
    loadFromFile(modelPath);
}

Model::~Model() { nullify(); }

void Model::loadFromFile(const char* modelPath)
{
    // local vars
    const aiScene* scene;
    std::string modelDir;
    unsigned int numMeshes;

    // check filepath
    if(!modelPath) { SPDLOG_ERROR("Image::loadFromFile(nullptr): null filepath"); return; }
    SPDLOG_INFO("Model::loadFromFile(\"{}\")", modelPath);
    
    // delete existing model
    if(m_meshes.size() || m_images.size())
    {
        SPDLOG_WARN("delete existing Model");
        nullify();
    }

    // open model file
	Assimp::Importer importer;
    scene = importer.ReadFile(modelPath,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
        SPDLOG_ERROR("assimp error:\n{}", importer.GetErrorString());
		return;
	}

    // get model directory
    modelDir = modelPath;
	modelDir = modelDir.substr(0, modelDir.find_last_of("/\\")) + '/'; // find both '/' and '\'
    SPDLOG_INFO("modelDir = \"{}\"", modelDir);
    
    numMeshes = scene->mNumMeshes;
    SPDLOG_INFO("found {} meshes belonging to \"{}\"", numMeshes, modelPath);
	for (unsigned int i = 0; i < numMeshes; i++)
	{
        SPDLOG_INFO("{}-th mesh", i);

		aiMesh* mesh = scene->mMeshes[i];
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        // load vertices, indices, and textures
        loadVertices(mesh, vertices);
        loadIndices(mesh, indices);
        loadTextures(mesh, textures, scene->mMaterials, modelDir);

        m_meshes.push_back(new Mesh(vertices, indices, textures));
	}
}

void Model::draw(ShaderProgram& ShaderProgram)
{
    size_t numMeshes;
    
    numMeshes = m_meshes.size();
    for(int i = 0; i < numMeshes; i++) { m_meshes[i]->draw(ShaderProgram); }
}

void Model::loadVertices(aiMesh* mesh, std::vector<Vertex>& vertices)
{
    if(!mesh) { return; }

    unsigned int numVertices = mesh->mNumVertices;
    for (unsigned int i = 0; i < numVertices; i++)
    {
        Vertex vert;
        glm::vec3 v3;
        memset(&vert, 0, sizeof(Vertex)); // nullify

        // aPos
        v3.x = mesh->mVertices[i].x;
        v3.y = mesh->mVertices[i].y;
        v3.z = mesh->mVertices[i].z;
        vert.position = v3;

        // aNormal
        if(mesh->HasNormals())
        {
            v3.x = mesh->mNormals[i].x;
            v3.y = mesh->mNormals[i].y;
            v3.z = mesh->mNormals[i].z;
            vert.normal = v3;
        }
        // aTexCoord
        if(mesh->mTextureCoords[0])
        {
            glm::vec2 v2;
            v2.x = mesh->mTextureCoords[0][i].x;
            v2.y = mesh->mTextureCoords[0][i].y;
            vert.texCoord = v2;
        }
        
        // aTangent and aBitangent
        if(mesh->HasTangentsAndBitangents())
        {
            v3.x = mesh->mTangents[i].x;
            v3.y = mesh->mTangents[i].y;
            v3.z = mesh->mTangents[i].z;
            vert.tangent = v3;

            v3.x = mesh->mBitangents[i].x;
            v3.y = mesh->mBitangents[i].y;
            v3.z = mesh->mBitangents[i].z;
            vert.bitangent = v3;
        }
        
        vertices.push_back(vert);
    }
}

void Model::loadIndices(aiMesh* mesh, std::vector<unsigned int>& indices)
{
    if(!mesh) { return; }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }
}

void Model::loadTextures(aiMesh* mesh, std::vector<Texture>& textures, aiMaterial** materials, std::string& modelDir)
{
    if(!mesh) { return; }

    unsigned int materialIndex = mesh->mMaterialIndex;

    loadTextureByType(textures, materials, materialIndex, aiTextureType_DIFFUSE, modelDir);
    loadTextureByType(textures, materials, materialIndex, aiTextureType_SPECULAR, modelDir);
    loadTextureByType(textures, materials, materialIndex, aiTextureType_NORMALS, modelDir);
    loadTextureByType(textures, materials, materialIndex, aiTextureType_HEIGHT, modelDir);
}

void Model::loadTextureByType(std::vector<Texture>& textures, aiMaterial** materials, unsigned int materialIndex, aiTextureType type, std::string& modelDir)
{
    Texture tex;
    unsigned int textureCount;

    memset(&tex, 0, sizeof(Texture)); // nullify
    switch(type)
    {
    case aiTextureType_DIFFUSE:
        tex.type = Texture::TYPE::DIFFUSE;
        break;
    case aiTextureType_SPECULAR:
        tex.type = Texture::TYPE::SPECULAR;
        break;
    case aiTextureType_NORMALS:
        tex.type = Texture::TYPE::NORMAL;
        break;
    case aiTextureType_HEIGHT:
        tex.type = Texture::TYPE::HEIGHT;
        break;
    default:
        SPDLOG_ERROR("wrong or unimplemented texture type");
        return;
    }

    textureCount = materials[materialIndex]->GetTextureCount(type);
	for(unsigned int i = 0; i < textureCount; i++)
	{
        aiString path;
		materials[materialIndex]->GetTexture(type, i, &path);

        int skip = -1;
        size_t numImages = m_images.size();
        for(int j = 0; j < numImages; j++)
        {
            if( m_images[j]->getImagePath() == path.C_Str() )
            {
                skip = j;
                break;
            }
        }

        if(skip >= 0) { tex.textureID = m_images[skip]->getImageID(); }
        else
        {
            Image* pImage = new Image((modelDir+path.C_Str()).c_str(), GL_TEXTURE_2D);
            m_images.push_back(pImage);
            tex.textureID = m_images.back()->getImageID();
        }

        textures.push_back(tex);
	}
}
#endif