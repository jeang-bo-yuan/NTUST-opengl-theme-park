
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

    /// 若沒有選中控制點，回傳true；否則回傳false
    bool nothing_is_selected() const { return m_selected_control_point < 0 || m_selected_control_point >= m_control_points.size(); }

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
     * @post 看原本有沒有控制點被選中，並emit is_point_selected()
     */
    void add_CP();

    /**
     * @brief 刪除被選中的control point
     * @note control point至少會留**4**個，所以只剩4個的時候無論如何都不會被刪
     * @post 若成功刪除，則選中後一個控制點，並emit is_point_selected(true)；否則，啥都不做
     */
    void delete_CP();

    /**
     * @brief 取得選中的control point的orient
     * @details 回傳兩個值alpha, beta。以`r=1`, `center=(0,0,0)`, `alpha=alpha`, `beta=beta`代入ArcBall後可算出實際的orient。
     * @param[out] alpha - 方位角(in radians)
     * @param[out] beta - 仰角(in radians)
     * @post 若沒有control point被選中，alpha, beta的值為NAN
     */
    void orient_of_selected_CP(float& alpha, float& beta) const;

    /**
     * @brief 替被選中的control point設定orient
     * @details 以`r=1`, `center=(0,0,0)`, `alpha=alpha`, `beta=beta`代入ArcBall後算出實際的orient。
     * @param alpha - 方位角(in radians)
     * @param beta - 仰角(in radians)
     * @post 若沒有選中control point，則什麼都不會做
     */
    void set_orient_for_selected_CP(float alpha, float beta);

    /// @}

    /**
     * @brief 開關「鉛直移動」
     * @param on - true->開啟；false->關閉
     */
    void toggle_vertical_move(bool on) { m_is_vertical_move = on; }

public:
    TrainSystem();

    /// 取得控制點的個數
    int get_cp_num() const { return m_control_points.size(); }

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
