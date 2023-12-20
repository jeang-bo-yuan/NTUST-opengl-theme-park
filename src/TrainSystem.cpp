
#include "TrainSystem.h"
#include <glad/gl.h>
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>
#include <iostream>
#include <cmath>

/// Control Point的大小
constexpr float CONTROL_POINT_SIZE = 0.1;
/// 大小的斜邊
constexpr float HYPOT_CP_SIZE = 1.41421f /*sqrt(2)*/ * CONTROL_POINT_SIZE;

bool TrainSystem::process_click(glm::vec3 pos)
{
    m_selected_control_point = -1;

    for (int i = 0; i < m_control_points.size(); ++i) {
        glm::vec3 delta = pos - m_control_points[i].pos;
        float horizontal_d = std::hypot(delta.x, delta.z); // 水平距離
        float vertical_d = std::abs(delta.y); // 垂直距離

        if (horizontal_d < HYPOT_CP_SIZE && vertical_d < 3 * CONTROL_POINT_SIZE) {
            m_selected_control_point = i;
            std::cout << "Select Control Point: " << m_selected_control_point << std::endl;

            return true;
        }
    }

    return false;
}

TrainSystem::TrainSystem()
    : m_control_points{{glm::vec3(1, 2, 0), glm::vec3(0, 1, 0)},
                       {glm::vec3(0, 2, 1), glm::vec3(0, 1, 0)},
                       {glm::vec3(-1, 2, 0), glm::vec3(0, 1, 0)},
                       {glm::vec3(0, 2, -1), glm::vec3(0, 1, 0)}},
    m_selected_control_point(-1)
{

}

void TrainSystem::draw(bool wireframe)
{
    glUseProgram(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    this->draw_control_points();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void TrainSystem::draw_control_points()
{
    glMatrixMode(GL_MODELVIEW);

    for (size_t i = 0; i < m_control_points.size(); ++i) {
        const glm::vec3& pos = m_control_points[i].pos;
        const glm::vec3& orient = m_control_points[i].orient;

        float size = CONTROL_POINT_SIZE;

        if (i == m_selected_control_point)
            glColor3ub(255, 0, 0);
        else
            glColor3ub(255, 255, 255);

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


