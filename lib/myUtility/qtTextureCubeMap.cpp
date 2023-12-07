
#include "qtTextureCubeMap.h"
#include <QImage>
#include <stdexcept>

qtTextureCubeMap::qtTextureCubeMap(const QString &pX, const QString &nX,
                                   const QString &pY, const QString &nY,
                                   const QString &pZ, const QString &nZ)
{
    GLint old_cube_map;
    glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &old_cube_map);

    // 產生texture，並綁定
    glGenTextures(1, &m_texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture_id);

    QImage each_face[6] = {
        QImage(pX), QImage(nX), QImage(pY), QImage(nY), QImage(pZ), QImage(nZ)
    };
    constexpr GLenum face_target[6] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    for (int i = 0; i < 6; ++i) {
        if (each_face[i].isNull()) throw std::invalid_argument("qtTextureCubeMap : cannot open image");

        each_face[i].convertTo(QImage::Format_RGBA8888);
        glTexImage2D(face_target[i], /* mipmap level */ 0, /* internal format */ GL_RGBA,
                     each_face[i].width(), each_face[i].height(), /* must be zero */0,
                     GL_RGBA, GL_UNSIGNED_BYTE, /* data */each_face[i].constBits());
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // 恢復原狀
    glBindTexture(GL_TEXTURE_CUBE_MAP, old_cube_map);
}

qtTextureCubeMap::~qtTextureCubeMap()
{
    glDeleteTextures(1, &m_texture_id);
}

void qtTextureCubeMap::bind_to(GLuint sampler)
{
    glActiveTexture(GL_TEXTURE0 + sampler);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture_id);
}

void qtTextureCubeMap::unbind_from(GLuint sampler)
{
    glActiveTexture(GL_TEXTURE0 + sampler);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
