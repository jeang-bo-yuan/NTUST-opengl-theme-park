
#include "TrainSystem.h"
#include <glad/gl.h>
#include <glm/trigonometric.hpp>

TrainSystem::TrainSystem()
    : m_control_points{{glm::vec3(1, 2, 0), glm::vec3(0, 1, 0)},
                       {glm::vec3(0, 2, 1), glm::vec3(0, 1, 0)},
                       {glm::vec3(-1, 2, 0), glm::vec3(0, 1, 0)},
                       {glm::vec3(0, 2, -1), glm::vec3(0, 1, 0)}}
{

}

void TrainSystem::draw(bool wireframe)
{
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    this->draw_control_points();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void TrainSystem::draw_control_points()
{
    glUseProgram(0);
    glMatrixMode(GL_MODELVIEW);

    for (size_t i = 0; i < m_control_points.size(); ++i) {
        const glm::vec3& pos = m_control_points[i].pos;
        const glm::vec3& orient = m_control_points[i].orient;

        float size = 0.1;

        glPushMatrix();
        glTranslatef(pos.x,pos.y,pos.z);
        float theta1 = -glm::degrees(atan2(orient.z,orient.x));
        glRotatef(theta1,0,1,0);
        float theta2 = -glm::degrees(acos(orient.y));
        glRotatef(theta2,0,0,1);

        glBegin(GL_QUADS);
        glNormal3f( 0,0,1);
        glVertex3f( size, size, size);
        glVertex3f(-size, size, size);
        glVertex3f(-size,-size, size);
        glVertex3f( size,-size, size);

        glNormal3f( 0, 0, -1);
        glVertex3f( size, size, -size);
        glVertex3f( size,-size, -size);
        glVertex3f(-size,-size, -size);
        glVertex3f(-size, size, -size);

        // no top - it will be the point

        glNormal3f( 0,-1,0);
        glVertex3f( size,-size, size);
        glVertex3f(-size,-size, size);
        glVertex3f(-size,-size,-size);
        glVertex3f( size,-size,-size);

        glNormal3f( 1,0,0);
        glVertex3f( size, size, size);
        glVertex3f( size,-size, size);
        glVertex3f( size,-size,-size);
        glVertex3f( size, size,-size);

        glNormal3f(-1,0,0);
        glVertex3f(-size, size, size);
        glVertex3f(-size, size,-size);
        glVertex3f(-size,-size,-size);
        glVertex3f(-size,-size, size);
        glEnd();
        glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0,1.0f,0);
        glVertex3f(0,3.0f*size,0);
        glNormal3f( 1.0f, 0.0f , 1.0f);
        glVertex3f( size, size , size);
        glNormal3f(-1.0f, 0.0f , 1.0f);
        glVertex3f(-size, size , size);
        glNormal3f(-1.0f, 0.0f ,-1.0f);
        glVertex3f(-size, size ,-size);
        glNormal3f( 1.0f, 0.0f ,-1.0f);
        glVertex3f( size, size ,-size);
        glNormal3f( 1.0f, 0.0f , 1.0f);
        glVertex3f( size, size , size);
        glEnd();
        glPopMatrix();
    }
}


