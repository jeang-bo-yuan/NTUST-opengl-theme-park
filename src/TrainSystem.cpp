
#include "TrainSystem.h"
#include <glad/gl.h>
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>
#include <iostream>
#include <cmath>

/// Control Point的大小
constexpr float CONTROL_POINT_SIZE = 0.1f;
/// 大小的斜邊
constexpr float HYPOT_CP_SIZE = 1.41421f /*sqrt(2)*/ * CONTROL_POINT_SIZE;

// Mouse Event //////////////////////////////////////////////////////////////////

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

            emit is_point_selected(true);
            return true;
        }
    }

    emit is_point_selected(false);
    return false;
}

bool TrainSystem::process_drag(glm::vec3 eye, glm::vec3 pos)
{
    if (m_selected_control_point < 0 || m_selected_control_point >= m_control_points.size())
        return false;

    if (m_is_vertical_move) {
        /// 若是鉛直移動。
        /// 計算 eye 和 pos 直線上的一點P，其中P.x 和原來 control point 的x座標一樣。
        /// 然後將control point的y座標設成P.y。
        float K;

        if (std::abs(eye.x - pos.x) < 1)
            /// 但如果 eye 和 pos 的x幾乎一樣，則計算P時改要求P.z 和原來 control point 的z座標一樣
            K = (m_control_points[m_selected_control_point].pos.z - eye.z) / (pos.z - eye.z);
        else
            K = (m_control_points[m_selected_control_point].pos.x - eye.x) / (pos.x - eye.x);

        // P = eye + K * (pos - eye)
        m_control_points[m_selected_control_point].pos.y = eye.y + K * (pos.y - eye.y);
    }
    else {
        /// 否則，是水平移動。
        /// 將選中的 control point 拖移到 eye 和 pos 的連線上的某點。
        /// 那個點的y座標和原本的y座標一樣。
        float K = (m_control_points[m_selected_control_point].pos.y - eye.y) / (pos.y - eye.y);
        glm::vec3 new_cp_pos = eye + K * (pos - eye);
        m_control_points[m_selected_control_point].pos = new_cp_pos;
    }

    return true;
}

// Control Point ///////////////////////////////////////////////////////////////////////////////

void TrainSystem::add_CP()
{
    /// 若沒有選中control point，則加在最後面
    if (m_selected_control_point < 0 || m_selected_control_point >= m_control_points.size()) {
        glm::vec3 new_pos = 0.5f * (m_control_points.front().pos + m_control_points.back().pos);

        m_control_points.emplace_back(ControlPoint{new_pos, glm::vec3(0, 1, 0)});
    }
    /// 否則加在選中的那個之後
    else {
        ControlPoint new_cp;
        int selected = m_selected_control_point, next = this->next_CP(selected);
        new_cp.pos = 0.5f * (m_control_points[selected].pos + m_control_points[next].pos);
        new_cp.orient = glm::vec3(0, 1, 0);

        m_control_points.insert(m_control_points.begin() + selected + 1, new_cp);
    }
}

void TrainSystem::delete_CP()
{
    if (m_selected_control_point < 0 || m_selected_control_point >= m_control_points.size())
        return;

    if (m_control_points.size() == 1) return;

    m_control_points.erase(m_control_points.begin() + m_selected_control_point);

    m_selected_control_point = -1;
    emit is_point_selected(false);
}

// Ctor /////////////////////////////////////////////////////////////////////////////////////////

TrainSystem::TrainSystem()
    : m_control_points{{glm::vec3(1, 2, 0), glm::vec3(0, 1, 0)},
                       {glm::vec3(0, 2, 1), glm::vec3(0, 1, 0)},
                       {glm::vec3(-1, 2, 0), glm::vec3(0, 1, 0)},
                       {glm::vec3(0, 2, -1), glm::vec3(0, 1, 0)}},
    m_selected_control_point(-1), m_is_vertical_move(false)
{

}

// Draw //////////////////////////////////////////////////////////////////////////////////////////

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


