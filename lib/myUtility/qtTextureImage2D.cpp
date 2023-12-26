
#include "qtTextureImage2D.h"
#include <QImage>
#include <QDebug>
#include <iostream>

qtTextureImage2D::qtTextureImage2D(const QString &path)
{
    QImage img(path);
    img.convertTo(QImage::Format_RGBA8888);
    if (img.isNull()) throw std::invalid_argument(std::string("qtTextureImage2D: fail to open the image ").append(qPrintable(path)));

    std::cout << "Texture: " << qPrintable(path) << " is loaded." << std::endl;

    glGenTextures(1, &m_texture_id);
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    glTexImage2D(GL_TEXTURE_2D, /* mipmap level */ 0, /* internal */ GL_RGBA,
                 img.width(), img.height(), /* must be zero */ 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, img.mirrored().constBits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
}

qtTextureImage2D::qtTextureImage2D(qtTextureImage2D &&rvalue)
    : m_texture_id(std::exchange(rvalue.m_texture_id, 0))
{

}

qtTextureImage2D::~qtTextureImage2D()
{
    if (m_texture_id != 0)
        glDeleteTextures(1, &m_texture_id);
}

void qtTextureImage2D::bind_to(GLuint sampler)
{
//    qDebug() << "bind" << m_texture_id << "at" << sampler;
    glActiveTexture(GL_TEXTURE0 + sampler);
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
}

void qtTextureImage2D::unbind_from(GLuint sampler)
{
    glActiveTexture(GL_TEXTURE0 + sampler);
    glBindTexture(GL_TEXTURE_2D, 0);
}

