
#ifndef TRAINSYSTEM_H
#define TRAINSYSTEM_H

#include <glm/vec3.hpp>
#include <vector>

struct ControlPoint {
    glm::vec3 pos;
    glm::vec3 orient;
};

class TrainSystem
{
private:
    std::vector<ControlPoint> m_control_points;
    int m_selected_control_point;

public:
    /// 處理點擊，並選擇控制點
    /// @param pos - 世界座標
    /// @return 是否處理點擊事件
    bool process_click(glm::vec3 pos);

public:
    TrainSystem();

    /// 畫出來
    /// @param wireframe - 是否是wireframe
    void draw(bool wireframe);

private:
    /// 畫控制點
    void draw_control_points();
};

#endif // TRAINSYSTEM_H
