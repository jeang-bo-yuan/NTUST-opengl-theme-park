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
#include <Shader.h>
#include <Model.h>

#include <QOpenGLWidget>
#include <QPoint>
#include <QTimer>

#include <memory>

#include "Skybox.h"
#include "TrainSystem.h"
#include "Water.h"
#include "Island.h"
#include "PostProcessor.h"

/**
 * @brief 用OpenGL繪製畫面
 * @details
 * ## UBOs
 * ```
 * layout(std140, binding = 0) uniform MatricesBlock {
 *    uniform mat4 view;
 *    uniform mat4 proj;
 * } Matrices;
 *
 * layout (std140, binding = 1) uniform LightBlock {
 *    vec4 eye_position;
 *    vec4 light_position;
 * } Light;
 *
 * layout (std140, binding = 2) uniform Cel_Shading_Block {
 *    int on;
 *    int levels;
 * } Cel;
 *
 * layout (std140, binding = 3) uniform ClipBlock {
 *    vec4 plane;
 * } Clip;
 * ```
 */
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

    /// light UBO
    std::unique_ptr<UBO> m_light_UBO_p;
    std::unique_ptr<UBO> m_cel_shading_p; ///< {int: on/off, int: levels}

    /// Clip Plane
    std::unique_ptr<UBO> m_clip_UBO_p; ///< 內含4-tuple，同glClipPlane，但是在世界座標下的平面

    /// skybox
    std::unique_ptr<Skybox> m_skybox_obj_p;

    // water
    std::unique_ptr<Water> m_water_obj_p;
    std::unique_ptr<FBO> m_reflection_FBO_p;
    std::unique_ptr<FBO> m_refraction_FBO_p;

    // train
    std::unique_ptr<TrainSystem> m_train_obj_p;
    float m_train_speed; ///< 火車的速度

    // island
    std::unique_ptr<Island> m_island_obj_p;

    // post processor
    std::unique_ptr<PostProcessor> m_post_processor_p;

    /// 每隔一段時間就updata一次
    QTimer m_timer;

    /// 是否繪製wireframe
    bool m_wireframe_mode;

    /// 是否追蹤火車
    bool m_tracking_train;

public:
    explicit ViewWidget(QWidget* parent = nullptr);
    ~ViewWidget();

    /// 取得火車
    TrainSystem& get_train() const { return *m_train_obj_p; }

private:
    void update_view_from_arc_ball();

    /// 點在視窗的winPos，並對每個物件處理點擊事件
    void process_click_for_obj(QPoint winPos, bool is_drag);

protected:
    /// initialize opengl things
    void initializeGL() override;
    /// resize window then update perspective matrix
    void resizeGL(int w, int h) override;
    /// paint opengl things
    void paintGL() override;
    /// 畫出水以外的東西
    void drawStuffs_without_water();


    /// mouse press -> remember where it press
    void mousePressEvent(QMouseEvent*) override;
    /// move camera
    void mouseMoveEvent(QMouseEvent*) override;
    /// 關閉mouse tracking
    void mouseReleaseEvent(QMouseEvent*) override;

    /// wheel scroll -> update distance
    void wheelEvent(QWheelEvent*) override;


    /// 按下鍵盤按鍵
    void keyPressEvent(QKeyEvent*) override;
    /// 釋放鍵盤按鍵
    void keyReleaseEvent(QKeyEvent* e) override;


public slots:
    void use_sine_wave() { m_water_obj_p->use_sine_wave(); }

    void use_ripple() { m_water_obj_p->use_ripple(); }

    void use_height_map() { m_water_obj_p->use_height_map(); }

    /// 替火車新增一個control point
    void add_train_CP() { m_train_obj_p->add_CP(); }
    /// 刪掉火車的一個control point
    void delete_train_CP() { m_train_obj_p->delete_CP(); }

    /// 設定速度
    void set_train_speed(int speed) { m_train_speed = (float)speed / 500; }

    void toggle_wireframe(bool on);

    void toggle_tracking_train(bool on) { m_tracking_train = on; }

    void set_post_process_type(PostProcessor::Type type);

    void toggle_Cel_Shading(bool on);

    void set_Cel_Levels(int levels);

    void import_control_points();

    void export_control_points();

signals:
    /// 轉發TrainSystem的signal。
    /// 見 TrainSystem::is_point_selected
    void is_point_selected(bool select);
};

#endif // VIEWWIDGET_H
