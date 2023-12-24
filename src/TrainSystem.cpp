
#include "TrainSystem.h"
#include <glad/gl.h>
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <ArcBall.h>

/// Control Point的大小
constexpr float CONTROL_POINT_SIZE = 0.1f;
/// 大小的斜邊
constexpr float HYPOT_CP_SIZE = 1.41421f /*sqrt(2)*/ * CONTROL_POINT_SIZE;

constexpr float Track_Interval = 0.1f;
constexpr float Param_Interval = 0.0625f;

// Arc Len Accum ////////////////////////////////////////////////////////////////

float TrainSystem::T_to_S(float T) const
{
    /// @note 超出範圍時，做wrap
    while (T < 0) T += m_Arc_Len_Accum.back().first;
    while (T >= m_Arc_Len_Accum.back().first) T -= m_Arc_Len_Accum.back().first;

    /// @note 處理極端特例，當T很接近0或最大參數時，回傳0
    if (fabs(T) < 1.e-4 || fabs(T - m_Arc_Len_Accum.back().first) < 1.e-4) return 0;

    for (size_t i = 0; i < m_Arc_Len_Accum.size() - 1; ++i) {
        const float lowU = m_Arc_Len_Accum[i].first;
        const float highU = m_Arc_Len_Accum[i + 1].first;
        if (lowU <= T && T < highU) {
            const float lowS = m_Arc_Len_Accum[i].second;
            const float highS = m_Arc_Len_Accum[i + 1].second;
            return lowS + (T - lowU) / (highU - lowU) * (highS - lowS);
        }
    }

    assert(false); // should not go here
    return NAN;
}

float TrainSystem::S_to_T(float S) const
{
    /// @note 超出範圍時，做wrap
    while (S < 0) S += m_Arc_Len_Accum.back().second;
    while (S >= m_Arc_Len_Accum.back().second) S -= m_Arc_Len_Accum.back().second;

    /// @note 處理極端特例，當S很接近0或最大長度時，回傳0
    if (fabs(S) < 1.e-4 || fabs(S - m_Arc_Len_Accum.back().second) < 1.e-4) return 0;

    // （實際空間） 轉回 （參數空間）
    for (size_t i = 0; i < m_Arc_Len_Accum.size() - 1; ++i) {
        const float lowS = m_Arc_Len_Accum[i].second;
        const float highS = m_Arc_Len_Accum[i + 1].second;
        if ((lowS <= S && S < highS)) {
            const float lowU = m_Arc_Len_Accum[i].first;
            const float highU = m_Arc_Len_Accum[i + 1].first;
            return lowU + (S - lowS) / (highS - lowS) * (highU - lowU);
        }
    }

    assert(false); // should not go here
    return NAN;
}

void TrainSystem::update_arc_len_accum()
{
    // reset
    m_Arc_Len_Accum.clear();
    m_Arc_Len_Accum.reserve(m_control_points.size() * 16 + 1);
    m_Arc_Len_Accum.emplace_back(std::pair<float, float>{ 0, 0 });

    // for each control point
    for (size_t cp_id = 0; cp_id < m_control_points.size(); ++cp_id) {
        Draw::Param_Equation point_eq, unused;
        set_equation(cp_id, point_eq, unused);

        glm::vec3 p1 = point_eq(0);
        for (float t = Param_Interval; t <= 1; t += Param_Interval) {
            glm::vec3 p2 = point_eq(t);
            glm::vec3 delta = p2 - p1;
            float len = sqrtf(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);

            m_Arc_Len_Accum.emplace_back(std::pair<float, float>{
                cp_id + t,
                len + m_Arc_Len_Accum.back().second
            });

            // store it for next iteration
            p1 = p2;
        }
    }

    m_please_update_arc_len_accum = false;
}


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
    if (nothing_is_selected())
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

    m_please_update_arc_len_accum = true;
    return true;
}

// Control Point ///////////////////////////////////////////////////////////////////////////////

void TrainSystem::add_CP()
{
    /// 若沒有選中control point，則加在最後面
    if (nothing_is_selected()) {
        glm::vec3 new_pos = 0.5f * (m_control_points.front().pos + m_control_points.back().pos);

        m_control_points.emplace_back(ControlPoint{new_pos, glm::vec3(0, 1, 0)});

        emit is_point_selected(false);
    }
    /// 否則加在選中的那個之後
    else {
        ControlPoint new_cp;
        int selected = m_selected_control_point, next = this->next_CP(selected);
        new_cp.pos = 0.5f * (m_control_points[selected].pos + m_control_points[next].pos);
        new_cp.orient = glm::vec3(0, 1, 0);

        m_control_points.insert(m_control_points.begin() + selected + 1, new_cp);

        emit is_point_selected(true);
    }

    m_please_update_arc_len_accum = true;
}

void TrainSystem::delete_CP()
{
    if (nothing_is_selected())
        return;

    if (m_control_points.size() <= 4) return;

    m_control_points.erase(m_control_points.begin() + m_selected_control_point);

    // 原本位置上的控制點被刪了，並由後一項遞補
    // 所以m_selected_control_point不用加1就已經指到原本的後一項
    m_selected_control_point %= m_control_points.size();
    emit is_point_selected(true);

    m_please_update_arc_len_accum = true;
}

void TrainSystem::orient_of_selected_CP(float &alpha, float &beta) const
{
    if (nothing_is_selected()) {
        alpha = NAN;
        beta = NAN;
        return;
    }

    const ControlPoint& point = m_control_points[m_selected_control_point];
    beta = std::asin(point.orient.y);
    if (std::fabs(point.orient.x) < 0.001 && std::fabs(point.orient.z) < 0.001)
        alpha = 0.f;
    else
        alpha = std::atan2(point.orient.z, point.orient.x);
}

void TrainSystem::set_orient_for_selected_CP(float alpha, float beta)
{
    if (nothing_is_selected())
        return;

    ArcBall ball(glm::vec3(0, 0, 0), 1.f, alpha, beta);
    m_control_points[m_selected_control_point].orient = ball.calc_pos();

    m_please_update_arc_len_accum = true;
}

// Ctor /////////////////////////////////////////////////////////////////////////////////////////

TrainSystem::TrainSystem()
    // 控制點初始化
    : m_control_points{{glm::vec3(2, 1, 0), glm::vec3(0, 1, 0)},
                       {glm::vec3(0, 1, 2), glm::vec3(0, 1, 0)},
                       {glm::vec3(-2, 1, 0), glm::vec3(0, 1, 0)},
                       {glm::vec3(0, 1, -2), glm::vec3(0, 1, 0)}},
    m_selected_control_point(-1), m_control_point_VAO(CONTROL_POINT_SIZE),
    m_control_point_shader("shader/control_point.vert", nullptr, nullptr, nullptr, "shader/control_point.frag"),
    // line type初始化
    m_line_type(SplineType::LINEAR), m_cardinal_tension(0.5f),
    m_Arc_Len_Accum(),
    // 木頭支柱初始化
    m_wood_shader("shader/wood.vert", nullptr, nullptr, nullptr, "shader/wood.frag"),
    m_wood_cube(":/wood.jpg", ":/wood.jpg", ":/wood.jpg", ":/wood.jpg", ":/wood.jpg", ":/wood.jpg"),
    m_unit_box_VAO(1.f),
    // 位置初始化
    m_train_pos(0, 0, 0), m_trainU(0.f),
    // 車子模型初始化
    m_train_models{ Model("asset/model/train/train.fbx"), Model("asset/model/train/train1.fbx"), Model("asset/model/train/train2.fbx"),
                   Model("asset/model/train/train3.fbx"), Model("asset/model/train/train4.fbx"), Model("asset/model/train/train5.fbx")},
    m_cart_models{ Model("asset/model/cart/cart.fbx"), Model("asset/model/cart/cart1.fbx"), Model("asset/model/cart/cart2.fbx"),
                   Model("asset/model/cart/cart3.fbx"), Model("asset/model/cart/cart4.fbx"), Model("asset/model/cart/cart5.fbx")},
    m_which_train(0), m_cart_num(0),
    // smoke
    m_smoke_obj([](const glm::vec3& pos, unsigned TTL)->glm::vec3 {
        return glm::vec3(pos.x, pos.y + TTL * CONTROL_POINT_SIZE * 0.02f, pos.z);
    }, CONTROL_POINT_SIZE, ":/smoke.png"), m_smoke_counter(0),
    // shader
    m_train_shader("shader/train.vert", nullptr, nullptr, nullptr, "shader/train.frag"),
    // flag 初始化
    m_is_vertical_move(false), m_please_update_arc_len_accum(true)
{
    glUseProgram(m_wood_shader.Program);
    glUniform1i(glGetUniformLocation(m_wood_shader.Program, "wood"), 0);
    glUseProgram(0);

    glUseProgram(m_train_shader.Program);
    glUniform1i(glGetUniformLocation(m_train_shader.Program, "diffuse"), 0);
    glUseProgram(0);

    this->update_arc_len_accum();
    this->updateTrainPos(0);
}

void TrainSystem::updateTrainPos(float distance)
{
    if (m_please_update_arc_len_accum)
        this->update_arc_len_accum();

    float S = T_to_S(m_trainU);
    S += distance;
    m_trainU = S_to_T(S);

    int cp_id = floor(m_trainU);
    Draw::Param_Equation pos_eq, orient_eq;
    set_equation(cp_id, pos_eq, orient_eq);
    m_train_pos = pos_eq(m_trainU - cp_id);

    if (distance > 0) m_which_train = (m_which_train + 1) % 6;
    else m_smoke_counter = 1;  // 如果火車沒有前進，則避免counter歸零，這樣draw_train時就不會加入更多的smoke

    m_smoke_obj.update();
}

// Draw //////////////////////////////////////////////////////////////////////////////////////////

void TrainSystem::draw(bool wireframe)
{
    if (m_please_update_arc_len_accum)
        this->update_arc_len_accum();

    glUseProgram(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    this->draw_control_points_with_shader(false);
    this->draw_wood_with_shader();
    this->draw_line();
    this->draw_sleeper();
    this->draw_train_with_shader();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    m_smoke_obj.draw();

    if (wireframe)
        this->draw_control_points_with_shader(true); // 畫一個透明的控制點
}

void TrainSystem::draw_control_points_with_shader(bool transparent)
{
    if (transparent) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ZERO, GL_ONE);
    }

    for (size_t i = 0; i < m_control_points.size(); ++i) {
        const glm::vec3& pos = m_control_points[i].pos;
        const glm::vec3& orient = m_control_points[i].orient;

        m_control_point_shader.Use();

        glm::mat4 model_matrix;
        model_matrix = glm::identity<glm::mat4>();
        model_matrix = glm::translate(model_matrix, pos);
        float theta1 = -atan2(orient.z,orient.x);
        model_matrix = glm::rotate(model_matrix, theta1, glm::vec3(0, 1, 0));
        float theta2 = -acos(orient.y);
        model_matrix = glm::rotate(model_matrix, theta2, glm::vec3(0, 0, 1));
        glUniformMatrix4fv(
            glGetUniformLocation(m_control_point_shader.Program, "model_matrix"),
            1,
            false, // no transpose
            glm::value_ptr(model_matrix)
        );

        glUniform1i(glGetUniformLocation(m_control_point_shader.Program, "is_selected"), i == m_selected_control_point);

        m_control_point_VAO.draw();

        glUseProgram(0);
    }

    glDisable(GL_BLEND);
}

void TrainSystem::draw_wood_with_shader()
{
    m_wood_shader.Use();
    glUniform1f(glGetUniformLocation(m_wood_shader.Program, "cp_size"), CONTROL_POINT_SIZE);
    m_wood_cube.bind_to(0);

    for (int i = 0; i < m_control_points.size(); ++i) {
        const glm::vec3& cp_pos = m_control_points[i].pos;
        if (cp_pos.y < -1) continue;

        glUniform3fv(glGetUniformLocation(m_wood_shader.Program, "cp_pos"), 1, glm::value_ptr(cp_pos));
        m_unit_box_VAO.draw();
    }


    m_wood_cube.unbind_from(0);
    glUseProgram(0);
}

void TrainSystem::draw_line()
{
    glColor3ub(0, 0, 0);

    std::vector<Draw::Param_Equation> pos_eq_vec, orient_eq_vec;
    this->set_equation(pos_eq_vec, orient_eq_vec);

    // 前一個點的資訊
    glm::vec3 P1, P1L, P1R;
    glm::vec3 orient1;
    bool is_P1_initialized = false;
    // 現在的點的資訊
    glm::vec3 P2, P2L, P2R;
    glm::vec3 orient2;

    constexpr float INTERVAL = 0.01f;
    for (int i = 0; i < m_control_points.size(); ++i) { // for each control point
        float t = 0.f;
        while (t <= 1.f) {
            if (!is_P1_initialized) {
                P1 = pos_eq_vec[i](t);
                orient1 = orient_eq_vec[i](t);
                t += INTERVAL;
            }

            P2 = pos_eq_vec[i](t);
            orient2 = orient_eq_vec[i](t);

            glm::vec3 U = P2 - P1; // 方向向量
            glm::vec3 RIGHT = glm::normalize(glm::cross(U, (orient1 + orient2) / 2.f)); // 向右
            glm::vec3 unit = CONTROL_POINT_SIZE * RIGHT;

            if (!is_P1_initialized) {
                P1L = P1 - unit;
                P1R = P1 + unit;
            }

            P2L = P2 - unit;
            P2R = P2 + unit;

            glBegin(GL_LINES);
            glVertex3fv(glm::value_ptr(P1L));
            glVertex3fv(glm::value_ptr(P2L));

            glVertex3fv(glm::value_ptr(P1R));
            glVertex3fv(glm::value_ptr(P2R));
            glEnd();

            // 前資訊往前移
            P1 = P2; P1L = P2L; P1R = P2R;
            orient1 = orient2;

            is_P1_initialized = true;
            t += INTERVAL;
        }
    }
}

void TrainSystem::draw_sleeper() const
{
    std::vector<Draw::Param_Equation> point_eq_vec;
    std::vector<Draw::Param_Equation> orient_eq_vec;
    point_eq_vec.reserve(m_control_points.size());
    orient_eq_vec.reserve(m_control_points.size());

    // for each control point
    for (size_t i = 0; i < m_control_points.size(); ++i) {
        Draw::Param_Equation point_eq, orient_eq;
        set_equation(i, point_eq, orient_eq);
        point_eq_vec.push_back(point_eq);
        orient_eq_vec.push_back(orient_eq); // store their equation
    }

    bool wrap_back = false; // 是否繞回S=0了，則是用來確保軌道頭尾相連
    glm::vec3 p1 = point_eq_vec[0](0);

    for (float S2 = Track_Interval; !wrap_back; S2 += Track_Interval) {
        // 繞回S=0（S大於等於最大值）了
        if (S2 >= m_Arc_Len_Accum.back().second) {
            wrap_back = true;
            S2 = m_Arc_Len_Accum.back().second;
        }

        float T2 = S_to_T(S2);
        glm::vec3 p2, orient; {
            size_t cp_id = static_cast<size_t>(floorf(T2));
            float t = T2 - cp_id;
            p2 = point_eq_vec[cp_id](t);
            orient = orient_eq_vec[cp_id](t - Param_Interval / 2.f);
        }

        // points
        glm::vec3 middle = (p1 + p2) * 0.5f;
        // 方向向量
        glm::vec3 u = (p2 - p1);
        float line_len = glm::length(u); // 這一段的長度
        u = glm::normalize(u);

        // u 和 orient 外積，得到軌道水平平移（向右）的方向
        glm::vec3 horizontal = glm::normalize(glm::cross(u, orient));

        // 畫sleeper
        glm::vec3 DOWN = glm::normalize(glm::cross(u, horizontal));
        GLfloat rotate_mat[16] = {
            horizontal.x, horizontal.y, horizontal.z, 0,
            DOWN.x, DOWN.y, DOWN.z, 0,
            u.x, u.y, u.z, 0,
            0, 0, 0, 1
        };
        middle = middle + DOWN * CONTROL_POINT_SIZE * 0.05f; // 住下一點點
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef(middle.x, middle.y, middle.z);
        glMultMatrixf(rotate_mat);
        glScalef(CONTROL_POINT_SIZE * 1.3f, CONTROL_POINT_SIZE * 0.1f, line_len * 0.3f);
        glColor3ub(255, 255, 255);
        glEnable(GL_NORMALIZE);
        {
            constexpr GLfloat vertex_arr[] = {
                1, 0, 1,
                1, 0, -1,
                -1, 0, -1,
                -1, 0, 1,
                1, 1 * 2, 1,
                1, 1 * 2, -1,
                -1, 1 * 2, -1,
                -1, 1 * 2, 1,
            };
            constexpr GLuint indices[] = {
                0, 1, 2, 3, // normal = (0, -1, 0)
                0, 1, 5, 4, // normal = (1, 0, 0)
                0, 3, 7, 4, // normal = (0, 0, 1)
                4, 5, 6, 7, // normal = (0, 1, 0)
                1, 5, 6, 2, // normal = (0, 0, -1)
                3, 2, 6, 7  // normal = (-1, 0, 0)
            };

            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, 0, vertex_arr);

            glNormal3f(0, -1, 0);
            glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, indices);

            glNormal3f(1, 0, 0);
            glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, indices + 4);

            glNormal3f(0, 0, 1);
            glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, indices + 8);

            glNormal3f(0, 1, 0);
            glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, indices + 12);

            glNormal3f(0, 0, -1);
            glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, indices + 16);

            glNormal3f(-1, 0, 0);
            glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, indices + 20);

            glVertexPointer(3, GL_FLOAT, 0, nullptr);
            glDisableClientState(GL_VERTEX_ARRAY);
        }
        glDisable(GL_NORMALIZE);
        glPopMatrix();

        // store for next iteration
        p1 = p2;
    }
}

void TrainSystem::draw_train_with_shader()
{
    m_train_shader.Use();
    glUniform1f(glGetUniformLocation(m_train_shader.Program, "scale"), 1.5f * CONTROL_POINT_SIZE);

    float S = T_to_S(m_trainU);

    for (int i = 0; i <= m_cart_num; ++i) { // i=0 -> 畫車頭； i>0 -> 畫車廂
        glUniform1i(glGetUniformLocation(m_train_shader.Program, "index"), i);

        float U = S_to_T(S); // 整個參數空間的位置
        int cp_id = floor(U);
        float T = U - cp_id; // 兩control point間 參數空間的位置

        Draw::Param_Equation point_eq, orient_eq;
        this->set_equation(cp_id, point_eq, orient_eq);

        // 位置
        glm::vec3 pos = point_eq(T);
        glUniform3fv(glGetUniformLocation(m_train_shader.Program, "translate"), 1, glm::value_ptr(pos));

        // 計算面向的方向
        glm::vec3 FRONT = glm::normalize(point_eq(T + 0.001) - point_eq(T));
        glm::vec3 LEFT = glm::normalize(glm::cross(orient_eq(T), FRONT));
        glm::vec3 TOP = glm::normalize(glm::cross(FRONT, LEFT));
        glUniform3fv(glGetUniformLocation(m_train_shader.Program, "FRONT"), 1, glm::value_ptr(FRONT));
        glUniform3fv(glGetUniformLocation(m_train_shader.Program, "LEFT"), 1, glm::value_ptr(LEFT));
        glUniform3fv(glGetUniformLocation(m_train_shader.Program, "TOP"), 1, glm::value_ptr(TOP));

        if (i == 0)
            m_train_models[m_which_train].draw();
        else
            m_cart_models[m_which_train].draw();

        S -= 5 * CONTROL_POINT_SIZE;

        if (i == 0 && m_smoke_counter == 0) m_smoke_obj.add(pos + (4.1f * CONTROL_POINT_SIZE) * TOP, 25);
        m_smoke_counter = (m_smoke_counter + 1) % 5;
    }

    glUseProgram(0);
}

void TrainSystem::set_equation(std::vector<Draw::Param_Equation> &pos_eqs, std::vector<Draw::Param_Equation> &orient_eqs) const
{
    pos_eqs.clear();
    orient_eqs.clear();

    for (int i = 0; i < m_control_points.size(); ++i) {
        Draw::Param_Equation pos_eq, orient_eq;

        this->set_equation(i, pos_eq, orient_eq);

        pos_eqs.push_back(pos_eq);
        orient_eqs.push_back(orient_eq);
    }
}

void TrainSystem::set_equation(int cp_id, Draw::Param_Equation &pos_eq, Draw::Param_Equation &orient_eq) const
{
    if (m_line_type == SplineType::LINEAR) {
        const ControlPoint& CP = m_control_points[cp_id];
        const ControlPoint& nCP = m_control_points[next_CP(cp_id)];

        pos_eq = Draw::make_line(CP.pos, nCP.pos);
        orient_eq = Draw::make_line(CP.orient, nCP.orient);
    }
    else {
        const ControlPoint& pCP = m_control_points[prev_CP(cp_id)];
        const ControlPoint& CP = m_control_points[cp_id];
        const ControlPoint& nCP = m_control_points[next_CP(cp_id)];
        const ControlPoint& nnCP = m_control_points[next_CP(next_CP(cp_id))];

        if (m_line_type == SplineType::CARDINAL) {
            pos_eq = Draw::make_cardinal(pCP.pos, CP.pos, nCP.pos, nnCP.pos, m_cardinal_tension);
            orient_eq = Draw::make_cardinal(pCP.orient, CP.orient, nCP.orient, nnCP.orient, m_cardinal_tension);
        }
        else {
            pos_eq = Draw::make_cubic_b_spline(pCP.pos, CP.pos, nCP.pos, nnCP.pos);
            orient_eq = Draw::make_cubic_b_spline(pCP.orient, CP.orient, nCP.orient, nnCP.orient);
        }
    }
}


