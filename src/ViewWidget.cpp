
#include "ViewWidget.h"
#include "qdebug.h"
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
    m_old_arc_ball(m_arc_ball), m_start_drag_point()
{

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
    this->update_view_from_arc_ball();

    /// @todo initialize drawable object
    try {
        m_skybox_obj_p = std::make_unique<Skybox>();
    }
    catch (std::exception& ex) {
        QMessageBox::critical(nullptr, "Failed", ex.what());
        exit(EXIT_FAILURE);
    }

    /// start timer
    m_timer.setInterval(20);
    connect(&m_timer, &QTimer::timeout, this, QOverload<>::of(&ViewWidget::update));
    m_timer.start();


    glEnable(GL_DEPTH_TEST);
    // During init, enable debug output
    glEnable              ( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, 0 );
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

    m_matrices_UBO_p->bind_to(0);

    m_skybox_obj_p->draw();

    glBegin(GL_QUADS);
    glVertex3i(-1, 0, -1);
    glVertex3i(-1, 0, 1);
    glVertex3i(1, 0, 1);
    glVertex3i(1, 0, -1);
    glEnd();
}

// Mouse Event ////////////////////////////////////////////////////////////////////////

void ViewWidget::mousePressEvent(QMouseEvent *e)
{
    m_old_arc_ball = m_arc_ball;
    m_start_drag_point = e->pos();
    this->setMouseTracking(true);
}

void ViewWidget::mouseMoveEvent(QMouseEvent *e)
{
    int delta_x = e->x() - m_start_drag_point.x();
    int delta_y = e->y() - m_start_drag_point.y();

    m_arc_ball.set_alpha(m_old_arc_ball.alpha() + glm::radians<float>(delta_x));
    m_arc_ball.set_beta(m_old_arc_ball.beta() + glm::radians<float>(delta_y));

    this->makeCurrent();
    this->update_view_from_arc_ball();
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
    qDebug() << "Wheel is scrolled";

    if (!degree_move.isNull()) {
        m_arc_ball.set_r(m_arc_ball.r() + degree_move.y() / 120.f);
    }

    this->makeCurrent();
    this->update_view_from_arc_ball();
    this->doneCurrent();

    qDebug() << "new r:" << m_arc_ball.r();
}

