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
    inline void nullify();

    public:
    Image();
    Image(char*, GLenum);
    ~Image();

    public:
    GLuint getImageID() { return m_imageID; };
    int getWidth() { return m_width; };
    int getHeight() { return m_height; };
    int getNrChannels() { return m_nrChannels; };

    public:
    static void setFlipVerticallyOnLoad(int);
    static void setTexParameter(GLenum, GLenum, GLint);
    static void setTexParameter(GLenum, GLenum, GLfloat);
    void loadFromFile(char*, GLenum);

    private:
    Image(const Image&) {};
    Image& operator=(const Image&) {};
};

inline void Image::nullify()
{
    m_imageID = NULL;
    m_width = m_height = m_nrChannels = 0;
}

Image::Image() { nullify(); }

// e.g.) Shader myS("lena_std.tif", GL_TEXTURE_2D);
Image::Image(char* imagePath, GLenum target)
{
    nullify();
    loadFromFile(imagePath, target);
}

Image::~Image()
{
    if(m_imageID)
    {
        glDeleteTextures(1, &m_imageID);
        nullify();
    }
}

void Image::setFlipVerticallyOnLoad(int flag_true_if_should_flip) { stbi_set_flip_vertically_on_load(flag_true_if_should_flip); }
void Image::setTexParameter(GLenum target, GLenum pname, GLint param) { glTexParameteri(target, pname, param); }
void Image::setTexParameter(GLenum target, GLenum pname, GLfloat param) { glTexParameterf(target, pname, param); }

// e.g.) Shader myS("lena_std.tif", GL_TEXTURE_2D);
// TODO
// add implemetations for GL_TEXTURE_CUBE_MAP and others
void Image::loadFromFile(char* imagePath, GLenum target)
{
    // local vars
    unsigned char* data;

    // update members
    if(m_imageID)
    {
        SPDLOG_WARN("delete existing image (ImageID={})", m_imageID);
        glDeleteTextures(1, &m_imageID);
        nullify();
    }

    // check filepath
    if(!imagePath) { SPDLOG_ERROR("Image::loadFromFile(nullptr): null filepath"); return; }
    SPDLOG_INFO("Image::loadFromFile(\"{}\")", imagePath);

    // open image file
    data = stbi_load(imagePath, &m_width, &m_height, &m_nrChannels, 0);
    if(!data) { SPDLOG_ERROR("no such image file"); return; }

    // generate texture object
    glGenTextures(1, &m_imageID);
    if(!m_imageID)
    {
        SPDLOG_ERROR("failed to generate texture");
        nullify(); // due to stbi_load()
        return;
    }

    // bind texture object and generate texture
    switch (target)
    {
    case GL_TEXTURE_2D:
        glBindTexture(target, m_imageID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        break;
    
    default:
        SPDLOG_ERROR("wrong or unimplemented target");
        glDeleteTextures(1, &m_imageID);
        nullify();
        return;
    }

    // free image buffer
    stbi_image_free(data);
    
    SPDLOG_INFO("ImageID={}", m_imageID);
}

#endif