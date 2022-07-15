#ifndef _IMAGE_
#define _IMAGE_

// spdlog
#include <spdlog/spdlog.h>

// stb
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

// opengl
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Image
{
    private:
    GLuint m_imageID;
    int m_width, m_height, m_nrChannels;

    public:
    Image(char* ip, GLenum t);
    ~Image();

    public:
    GLuint getImageID() { return m_imageID; };
    int getWidth() { return m_width; };
    int getHeight() { return m_height; };
    int getNrChannels() { return m_nrChannels; };

    private:
    Image() {};
    Image(const Image& i) {};
    Image& operator=(const Image& i) {};
};

// e.g.) Shader myS("lena_std.tif", GL_TEXTURE_2D);
// TODO
// add implemetations for GL_TEXTURE_CUBE_MAP and others
Image::Image(char* imagePath, GLenum target)
{
    // init member
    m_imageID = NULL;
    m_width = m_height = m_nrChannels = 0;

    // check filepath
    if(!imagePath) { SPDLOG_ERROR("null filepath"); return; }

    // open image file
    unsigned char* data = stbi_load(imagePath, &m_width, &m_height, &m_nrChannels, 0);
    if(!data) { SPDLOG_ERROR("no such image file ({})", imagePath); return; }

    // generate and bind texture object
    glGenTextures(1, &m_imageID);
    if(!m_imageID) { SPDLOG_ERROR("failed to generate texture"); return; }

    glBindTexture(target, m_imageID);

    // generate texture
    SPDLOG_INFO("generating texture {} ({})", m_imageID, imagePath);
    switch (target)
    {
    case GL_TEXTURE_2D:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        break;
    
    default:
        SPDLOG_ERROR("wrong or unimplemented target");
        glDeleteTextures(1, &m_imageID);
        m_imageID = NULL;
        m_width = m_height = m_nrChannels = 0;
        return;
    }

    // free image buffer
    stbi_image_free(data);
}

Image::~Image()
{
    if(m_imageID)
    {
        glDeleteTextures(1, &m_imageID);
        m_imageID = NULL;
    }
}

#endif