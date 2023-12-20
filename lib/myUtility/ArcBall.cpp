
#include "ArcBall.h"
#include <glm/gtc/matrix_transform.hpp>

ArcBall::ArcBall(glm::vec3 center, float r, float alpha, float beta)
{
    this->set_center(center);
    this->set_r(r);
    this->set_alpha(alpha);
    this->set_beta(beta);
}

glm::vec3 ArcBall::calc_pos() const
{
    return m_center + glm::vec3(
        m_r * glm::cos(m_beta) * glm::cos(m_alpha),
        m_r * glm::sin(m_beta),
        m_r * glm::cos(m_beta) * glm::sin(m_alpha)
        );
}

glm::mat4 ArcBall::view_matrix() const
{
    glm::vec3 eye = this->calc_pos();
    glm::vec3 up(
        - glm::cos(m_alpha) * glm::sin(m_beta),
          glm::cos(m_beta),
        - glm::sin(m_alpha) * glm::sin(m_beta)
    );

    return glm::lookAt(eye, m_center, up);
}

glm::vec3 ArcBall::face_dir() const
{
    return glm::vec3(
        -cos(m_alpha),
        0,
        -sin(m_alpha)
    );
}

glm::vec3 ArcBall::right_dir() const
{
    return glm::vec3(
        sin(m_alpha),
        0,
        -cos(m_alpha)
    );
}

