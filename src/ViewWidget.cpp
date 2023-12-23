
#include "ViewWidget.h"
#include <QDebug>
#include <QKeyEvent>
#include <QMessageBox>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <QMouseEvent>
#include <QWheelEvent>

void GLAPIENTRY
MessageCallback( GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar* message,
                const void* userParam )
{
    fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}

// Ctor & Dtor ////////////////////////////////////////////////////////////////////

ViewWidget::ViewWidget(QWidget *parent)
    : QOpenGLWidget(parent),
    m_arc_ball(glm::vec3(0, 0, 0), 10, glm::radians(45.f), glm::radians(20.f)),
    m_old_arc_ball(m_arc_ball), m_start_drag_point(), m_train_speed(0.1), m_wireframe_mode(false), m_tracking_train(false)
{
    this->setFocusPolicy(Qt::StrongFocus);
}

ViewWidget::~ViewWidget()
{
    this->makeCurrent();
}

// Private Method /////////////////////////////////////////////////////////////////

void ViewWidget::update_view_from_arc_ball()
{
    glm::mat4 view_matrix = m_arc_ball.view_matrix();
    m_matrices_UBO_p->BufferSubData(0, sizeof(glm::mat4), glm::value_ptr(view_matrix));
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(view_matrix));

    m_light_UBO_p->BufferSubData(0, sizeof(glm::vec4), glm::value_ptr(m_arc_ball.calc_pos()));
}

void ViewWidget::process_click_for_obj(QPoint winPos, bool is_drag)
{
    // Qt的y是從上往下算，OpenGL是從下往上算
    glm::vec3 winPos3D(winPos.x(), this->height() - winPos.y(), 0);
    glReadPixels(winPos3D.x, winPos3D.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winPos3D.z);

    glm::mat4 view_matrix = m_arc_ball.view_matrix();
    glm::vec4 viewport(0, 0, this->width(), this->height());
    // 計算點在世界座標的哪裡
    glm::vec3 pos = glm::unProject(winPos3D, view_matrix, m_proj_matrix, viewport);

    if (is_drag) {
        m_train_obj_p->process_drag(m_arc_ball.calc_pos(), pos) ||
            m_water_obj_p->process_click(pos);
    }
    else {
        std::cout << "Clicked on (" << pos.x << ", " << pos.y << ", " << pos.z << ')' << std::endl;

        m_train_obj_p->process_click(pos) ||
            m_water_obj_p->process_click(pos);
    }
}

// OpenGL /////////////////////////////////////////////////////////////////////////

void ViewWidget::initializeGL()
{
    int version = gladLoaderLoadGL();
    if (version == 0) {
        QMessageBox::critical(nullptr, "Load Failed", "Unable to Load OpenGL");
        exit(EXIT_FAILURE);
    }
    std::cerr << "Load OpenGL" << GLAD_VERSION_MAJOR(version) << '.' << GLAD_VERSION_MINOR(version) << '\n';

    /// @todo load UBO
    m_matrices_UBO_p = std::make_unique<UBO>(2 * sizeof(glm::mat4), GL_DYNAMIC_DRAW);
    m_light_UBO_p = std::make_unique<UBO>(2 * sizeof(glm::vec4), GL_DYNAMIC_DRAW);
    m_light_UBO_p->BufferSubData(sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(glm::vec4(0, 5, 10, 1)));
    this->update_view_from_arc_ball();

    /// @todo initialize drawable object
    try {
        m_skybox_obj_p = std::make_unique<Skybox>();
        m_water_obj_p = std::make_unique<Water>();

        m_train_obj_p = std::make_unique<TrainSystem>();
        connect(m_train_obj_p.get(), &TrainSystem::is_point_selected, this, &ViewWidget::is_point_selected);

        m_island_obj_p = std::make_unique<Island>();
    }
    catch (std::exception& ex) {
        QMessageBox::critical(nullptr, "Failed", ex.what());
        exit(EXIT_FAILURE);
    }

    /// @todo Light
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat lightPosition1[] = {0,1,5,0};
    GLfloat whiteLight[]	 = {1.0f, 1.0f, 1.0f, 1.0};
    GLfloat grayLight[]	     = {0.5f, 0.5f, 0.5f, 1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);
    glLightfv(GL_LIGHT0, GL_AMBIENT, grayLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, whiteLight);

    /// start timer
    m_timer.setInterval(20);
    connect(&m_timer, &QTimer::timeout, this, QOverload<>::of(&ViewWidget::update));
    m_timer.start();


    glEnable(GL_DEPTH_TEST);
    // During init, enable debug output
    glEnable              ( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, 0 );


    // unbind everything
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ViewWidget::resizeGL(int w, int h)
{
    m_proj_matrix = glm::perspective<float>(glm::radians(50.f), (float)w / h, 0.1f, 200.f);
    m_matrices_UBO_p->BufferSubData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(m_proj_matrix));
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(m_proj_matrix));
}

void ViewWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_train_obj_p->updateTrainPos(m_train_speed);

    if (m_tracking_train) {
        m_arc_ball.set_center(m_train_obj_p->getTrainPos());
        this->update_view_from_arc_ball();
    }

    // bind UBO
    m_matrices_UBO_p->bind_to(0);
    m_light_UBO_p->bind_to(1);

    m_skybox_obj_p->draw(m_wireframe_mode);
    m_water_obj_p->draw(m_wireframe_mode);
    m_train_obj_p->draw(m_wireframe_mode);
    m_island_obj_p->draw(m_wireframe_mode);
}

// Mouse Event ////////////////////////////////////////////////////////////////////////

void ViewWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::RightButton) {
        m_old_arc_ball = m_arc_ball;
        m_start_drag_point = e->pos();
    }
    else {
        this->makeCurrent();
        this->process_click_for_obj(e->pos(), false);
        this->doneCurrent();
    }

    this->setMouseTracking(true);
}

void ViewWidget::mouseMoveEvent(QMouseEvent *e)
{
    this->makeCurrent();

    if (e->buttons() & Qt::RightButton) {
        int delta_x = e->x() - m_start_drag_point.x();
        int delta_y = e->y() - m_start_drag_point.y();

        m_arc_ball.set_alpha(m_old_arc_ball.alpha() + glm::radians<float>(delta_x));
        m_arc_ball.set_beta(m_old_arc_ball.beta() + glm::radians<float>(delta_y));

        this->update_view_from_arc_ball();
    }
    else {
        process_click_for_obj(e->pos(), true);
    }

    this->doneCurrent();
}

void ViewWidget::mouseReleaseEvent(QMouseEvent *e)
{
    this->setMouseTracking(false);
}

// Wheel Event ////////////////////////////////////////////////////////////////////////

void ViewWidget::wheelEvent(QWheelEvent *e)
{
    QPoint degree_move = e->angleDelta();

    if (!degree_move.isNull()) {
        m_arc_ball.set_r(m_arc_ball.r() + degree_move.y() / 120.f);
    }

    this->makeCurrent();
    this->update_view_from_arc_ball();
    this->doneCurrent();
}

// Key Event //////////////////////////////////////////////////////////////////////////

void ViewWidget::keyPressEvent(QKeyEvent *e)
{
    this->makeCurrent();

    switch(e->key()) {
    case Qt::Key_Delete:
        m_train_obj_p->delete_CP();
        break;

    case Qt::Key_Shift:
        m_train_obj_p->toggle_vertical_move(true);
        break;

    case Qt::Key_Space:  // up or down
        m_arc_ball.set_center(e->modifiers() == Qt::ShiftModifier ?
                              m_arc_ball.center() - glm::vec3(0, 0.05f, 0) :  // 有按shift則往下
                              m_arc_ball.center() + glm::vec3(0, 0.05f, 0));  // 否則，往上
        this->update_view_from_arc_ball();
        break;

    case Qt::Key_W:  // go forward
        m_arc_ball.set_center(m_arc_ball.center() + m_arc_ball.face_dir() * 0.05f);
        this->update_view_from_arc_ball();
        break;

    case Qt::Key_S:  // go backward
        m_arc_ball.set_center(m_arc_ball.center() - m_arc_ball.face_dir() * 0.05f);
        this->update_view_from_arc_ball();
        break;

    case Qt::Key_D:  // go right
        m_arc_ball.set_center(m_arc_ball.center() + m_arc_ball.right_dir() * 0.05f);
        this->update_view_from_arc_ball();
        break;

    case Qt::Key_A:  // go left
        m_arc_ball.set_center(m_arc_ball.center() - m_arc_ball.right_dir() * 0.05f);
        this->update_view_from_arc_ball();
        break;
    }

    this->doneCurrent();
}

void ViewWidget::keyReleaseEvent(QKeyEvent *e)
{
    this->makeCurrent();

    switch(e->key()) {
    case Qt::Key_Shift:
        m_train_obj_p->toggle_vertical_move(false);
        break;
    }

    this->doneCurrent();
}

// Slots //////////////////////////////////////////////////////////////////////////////

void ViewWidget::toggle_wireframe(bool on) {
    m_wireframe_mode = on;
}

