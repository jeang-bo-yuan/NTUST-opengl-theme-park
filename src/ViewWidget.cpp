
#include "ViewWidget.h"
#include <QDebug>
#include <QKeyEvent>
#include <QMessageBox>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QFileDialog>

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
    m_arc_ball(glm::vec3(0, 1, 0), 5, glm::radians(45.f), glm::radians(20.f)),
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
    //
    m_light_UBO_p = std::make_unique<UBO>(2 * sizeof(glm::vec4), GL_DYNAMIC_DRAW);
    m_light_UBO_p->BufferSubData(sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(glm::vec4(0, 5, 10, 1)));
    this->update_view_from_arc_ball();
    //
    m_cel_shading_p = std::make_unique<UBO>(2 * sizeof(int), GL_STATIC_DRAW);
    int cel_option[2] = { 0, 4 };
    m_cel_shading_p->BufferData(cel_option);
    //
    m_clip_UBO_p = std::make_unique<UBO>(sizeof(glm::vec4), GL_DYNAMIC_DRAW);

    /// @todo initialize drawable object
    try {
        m_skybox_obj_p = std::make_unique<Skybox>();
        m_water_obj_p = std::make_unique<Water>();
        m_reflection_FBO_p = std::make_unique<FBO>(width(), height());
        m_refraction_FBO_p = std::make_unique<FBO>(width(), height());

        m_train_obj_p = std::make_unique<TrainSystem>();
        connect(m_train_obj_p.get(), &TrainSystem::is_point_selected, this, &ViewWidget::is_point_selected);

        m_island_obj_p = std::make_unique<Island>();

        m_post_processor_p = std::make_unique<PostProcessor>(width(), height());
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
    glEnable(GL_CLIP_PLANE0);
    glEnable(GL_CLIP_DISTANCE0);
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
    // update projection matrix
    m_proj_matrix = glm::perspective<float>(glm::radians(50.f), (float)w / h, 0.1f, 200.f);
    m_matrices_UBO_p->BufferSubData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(m_proj_matrix));
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(m_proj_matrix));

    // update post processor's buffer size
    m_post_processor_p->resize(w, h);
    m_reflection_FBO_p->resize(w, h);
    m_refraction_FBO_p->resize(w, h);
}

void ViewWidget::paintGL()
{
    constexpr float WATER_HEIGHT = -0.3f;
    constexpr float  NO_CLIP[4]   = {0, 0, 0, 0}, ABOVE_WATER[4]   = {0, 1, 0, -WATER_HEIGHT}, UNDER_WATER[4]   = {0, -1, 0, WATER_HEIGHT};
    constexpr double NO_CLIP_D[4] = {0, 0, 0, 0}, ABOVE_WATER_D[4] = {0, 1, 0, -WATER_HEIGHT}, UNDER_WATER_D[4] = {0, -1, 0, WATER_HEIGHT};
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_train_obj_p->updateTrainPos(m_train_speed);

    if (m_tracking_train) {
        m_arc_ball.set_center(m_train_obj_p->getTrainPos());
        this->update_view_from_arc_ball();
    }

    // bind UBO
    m_matrices_UBO_p->bind_to(0);
    m_light_UBO_p->bind_to(1);
    m_cel_shading_p->bind_to(2);
    m_clip_UBO_p->bind_to(3);

    int old_FBO;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &old_FBO);

    // reflection FBO
    m_reflection_FBO_p->bind_FBO_and_set_viewport(GL_DRAW_FRAMEBUFFER);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 將相機對稱水面
    glm::vec3 delta(0, 2 * (m_arc_ball.center().y - WATER_HEIGHT), 0); // 水面在 y = WATER_HEIGHT
    m_arc_ball.set_center(m_arc_ball.center() - delta);
    m_arc_ball.set_beta(-m_arc_ball.beta());
    this->update_view_from_arc_ball();
    // draw
    glClipPlane(GL_CLIP_PLANE0, ABOVE_WATER_D);       // glClipPlane會將這平面轉成視空間的座標，所以要改完ModelView Matrix才能設定
    m_clip_UBO_p->BufferData((void*)ABOVE_WATER);
    this->drawStuffs_without_water();
    // 復原相機
    m_arc_ball.set_center(m_arc_ball.center() + delta);
    m_arc_ball.set_beta(-m_arc_ball.beta());
    this->update_view_from_arc_ball();

    // refraction FBO
    m_refraction_FBO_p->bind_FBO_and_set_viewport(GL_DRAW_FRAMEBUFFER);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClipPlane(GL_CLIP_PLANE0, UNDER_WATER_D);
    m_clip_UBO_p->BufferData((void*)UNDER_WATER);
    this->drawStuffs_without_water();

    // 繪製最終畫面 + 後處理
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, old_FBO);
    glClipPlane(GL_CLIP_PLANE0, NO_CLIP_D);
    m_clip_UBO_p->BufferData((void*)NO_CLIP);
    m_post_processor_p->prepare();
    this->drawStuffs_without_water();
    m_water_obj_p->draw(m_wireframe_mode, *m_reflection_FBO_p, *m_refraction_FBO_p);
    m_post_processor_p->start_post_process();
}

void ViewWidget::drawStuffs_without_water()
{
    m_skybox_obj_p->draw(m_wireframe_mode);
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

void ViewWidget::set_water_reflect_refract(Water::ReflectRefract type, float factor)
{
    this->makeCurrent();
    m_water_obj_p->setReflectRefract(type, factor);
    this->doneCurrent();
}

void ViewWidget::toggle_wireframe(bool on) {
    m_wireframe_mode = on;
}

void ViewWidget::set_post_process_type(PostProcessor::Type type) {
    this->makeCurrent();
    m_post_processor_p->changeType(type);
    this->doneCurrent();
}

void ViewWidget::toggle_Cel_Shading(bool on)
{
    this->makeCurrent();
    int value = (on ? 1 : 0);
    m_cel_shading_p->BufferSubData(0, sizeof(int), &value);
    this->doneCurrent();
}

void ViewWidget::set_Cel_Levels(int levels)
{
    if (levels <= 0) return;

    m_cel_shading_p->BufferSubData(sizeof(int), sizeof(int), &levels);
}

void ViewWidget::import_control_points()
{
    QString path = QFileDialog::getOpenFileName(nullptr, "Import Control Points", ".", "Text (*.txt)");
    if (path.isEmpty()) return;
    m_train_obj_p->import_control_points(path.toStdString());
}

void ViewWidget::export_control_points()
{
    QString path = QFileDialog::getSaveFileName(nullptr, "Export Control Points", ".", "Text (*.txt)");
    if (path.isEmpty()) return;
    m_train_obj_p->export_control_points(path.toStdString());
}

