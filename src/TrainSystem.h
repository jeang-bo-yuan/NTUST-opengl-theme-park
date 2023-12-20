
#ifndef TRAINSYSTEM_H
#define TRAINSYSTEM_H

#include <glm/vec3.hpp>
#include <vector>
#include <QObject>

struct ControlPoint {
    glm::vec3 pos;
    glm::vec3 orient;
};

class TrainSystem : public QObject
{
    Q_OBJECT

private:
    std::vector<ControlPoint> m_control_points;
    int m_selected_control_point;
    bool m_is_vertical_move; ///< 是否鉛直移動 control point

private:
    int next_CP(int old) { return (old + 1) % m_control_points.size(); }
    int prev_CP(int old) { return (old <= 0 ? static_cast<int>(m_control_points.size()) - 1
                                            : (old - 1) % m_control_points.size()); }

public:
    /// @name Edit Control Point
    /// @brief 下列function都可能會改變control point的位置
    /// @{

    /// @brief 處理點擊，並選擇控制點
    /// @param pos - 世界座標
    /// @return 是否處理點擊事件
    /// @post emit is_point_selected()
    bool process_click(glm::vec3 pos);

    /**
     * @brief 處理拖移
     * @param eye - 眼睛的位置
     * @param pos - 點的位置
     * @return 是否處理拖移事件
     */
    bool process_drag(glm::vec3 eye, glm::vec3 pos);

    /**
     * @brief 新增一個control point
     */
    void add_CP();

    /**
     * @brief 刪除被選中的control point
     * @note control point至少會留一個，所以只剩一個的時候無論如何都不會被刪
     * @post 若成功刪除，emit is_point_selected(false)
     */
    void delete_CP();

    /// @}

    /**
     * @brief 開關「鉛直移動」
     * @param on - true->開啟；false->關閉
     */
    void toggle_vertical_move(bool on) { m_is_vertical_move = on; }

public:
    TrainSystem();

    /// 畫出來
    /// @param wireframe - 是否是wireframe
    void draw(bool wireframe);

private:
    /// 畫控制點
    void draw_control_points();

signals:
    /// 當有control point 被選中 or 被取消選取都會emit
    /// @param select - true->有被選中；false->沒被選中
    void is_point_selected(bool select);
};

#endif // TRAINSYSTEM_H
