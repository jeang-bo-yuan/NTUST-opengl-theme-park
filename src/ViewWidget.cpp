
#include "ViewWidget.h"
#include <QMessageBox>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

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
    m_matrices_UBO_p->BufferSubData(0, sizeof(glm::mat4), glm::value_ptr(m_arc_ball.view_matrix()));
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
}

void ViewWidget::resizeGL(int w, int h)
{
    m_proj_matrix = glm::perspective<float>(glm::radians(50.f), (float)w / h, 0.1f, 200.f);
    m_matrices_UBO_p->BufferSubData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(m_proj_matrix));
}

void ViewWidget::paintGL()
{
    m_matrices_UBO_p->bind_to(0);

    m_skybox_obj_p->draw();
}

