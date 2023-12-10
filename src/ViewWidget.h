/**
 * @file ViewWidget.h
 * @brief 用OpenGL顯示畫面
 */
#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <glad/gl.h>
#include <ArcBall.h>
#include <qtTextureCubeMap.h>
#include <UBO.h>
#include <Box_VAO.h>

#include <QOpenGLWidget>
#include <QPoint>
#include <QTimer>

#include <memory>

#include "Skybox.h"

class ViewWidget : public QOpenGLWidget
{
    Q_OBJECT

private:
    /// 視角
    ArcBall m_arc_ball;

    /// 拖動前的視角
    ArcBall m_old_arc_ball;
    /// 開始拖動的點
    QPoint m_start_drag_point;

    /// view matrix & projection matrix
    glm::mat4 m_proj_matrix;
    std::unique_ptr<UBO> m_matrices_UBO_p;

    /// skybox
    std::unique_ptr<Skybox> m_skybox_obj_p;

    /// 每隔一段時間就updata一次
    QTimer m_timer;

public:
    explicit ViewWidget(QWidget* parent = nullptr);
    ~ViewWidget();

private:
    void update_view_from_arc_ball();

protected:
    /// initialize opengl things
    void initializeGL() override;
    /// resize window then update perspective matrix
    void resizeGL(int w, int h) override;
    /// paint opengl things
    void paintGL() override;

    /// mouse press -> remember where it press
    void mousePressEvent(QMouseEvent*) override;
    /// move camera
    void mouseMoveEvent(QMouseEvent*) override;
    /// 關閉mouse tracking
    void mouseReleaseEvent(QMouseEvent*) override;

    /// wheel scroll -> update distance
    void wheelEvent(QWheelEvent*) override;
};

#endif // VIEWWIDGET_H
