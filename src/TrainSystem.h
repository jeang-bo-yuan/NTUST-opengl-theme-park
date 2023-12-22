
#ifndef TRAINSYSTEM_H
#define TRAINSYSTEM_H

#include <glm/vec3.hpp>
#include <vector>
#include <utility>
#include <QObject>
#include <qtTextureCubeMap.h>
#include <Box_VAO.h>
#include <Shader.h>
#include "ParamEquation.h"

/// 控制點
struct ControlPoint {
    glm::vec3 pos;
    glm::vec3 orient;
};

/// 火車用的軌道樣式
enum class SplineType {
    LINEAR,   ///< 直線
    CARDINAL, ///< cardinal
    CUBIC_B   ///< cubic B spline
};

/// 火車
class TrainSystem : public QObject
{
    Q_OBJECT

private:
    /// 下個control point的index
    int next_CP(int old) const { return (old + 1) % m_control_points.size(); }
    /// 前個control point的index
    int prev_CP(int old) const { return (old <= 0 ? static_cast<int>(m_control_points.size()) - 1
                                                  : (old - 1) % m_control_points.size()); }

    /// 若沒有選中控制點，回傳true；否則回傳false
    bool nothing_is_selected() const { return m_selected_control_point < 0 || m_selected_control_point >= m_control_points.size(); }

private:
    /// @name Arc Length Accumulation
    /// @{

    /// @brief 曲線長累積表的型別
    /// @details Accumulation of arc length. elem.first = t in "param space", elem.second = s in "real space"
    typedef std::vector< std::pair< float, float > >  Arc_Len_Accum_T ;

    /// @brief 將參數空間中的T 換成 實際距離S
    /// @details 依據 this->Arc_Len_Accum 做轉換
    float  T_to_S (float T) const;

    /// @brief 將實際距離S 換成 參數空間中的T
    /// @details 依據 this->Arc_Len_Accum 做轉換
    float  S_to_T (float S) const;

    /// 更新 m_Arc_Len_Accum
    /// @post `m_please_update_arc_len_accum = false;`
    void update_arc_len_accum();

    /// @}

public:
    /// @name Edit
    /// @brief 下列function都可能會改變Arc Len Accum
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

    /**
     * @brief 設定spline type
     * @param type
     */
    void set_line_type(SplineType type) { m_line_type = type;  m_please_update_arc_len_accum = true; }

    /**
     * @brief 設定tension
     * @param tension
     */
    void set_tension(float tension) { m_cardinal_tension = tension; m_please_update_arc_len_accum = true; }

    /// @}

public:
    TrainSystem();

    /**
     * @brief 開關「鉛直移動」
     * @param on - true->開啟；false->關閉
     */
    void toggle_vertical_move(bool on) { m_is_vertical_move = on; }

    /// 取得控制點的個數
    int get_cp_num() const { return m_control_points.size(); }

    /// 畫出來
    /// @param wireframe - 是否是wireframe
    void draw(bool wireframe);

private:
    /// 畫控制點
    void draw_control_points(bool transparent);

    /// 畫出木頭支柱
    void draw_wood_with_shader();

    /// 畫出軌道的線
    void draw_line();

    /// 畫枕木
    void draw_sleeper() const;

    /**
     * @brief 設置好「每兩個」控制點間的參數式
     * @param[out] pos_eqs - 第i項為 i 和 i+1 間座標的參數式
     * @param[out] orient_eqs - 第i項為 i 和 i+1 間orient的參數式
     * @post pos_eqs 和 orient_eqs 會先清空，再填值
     */
    void set_equation(std::vector<Draw::Param_Equation>& pos_eqs, std::vector<Draw::Param_Equation>& orient_eqs) const;

    /**
     * @brief 設定兩控制點間的參數式
     * @param cp_id - 控制點id
     * @param[out] pos_eq - 點的參數式
     * @param[out] orient_eq - orient的參數式
     */
    void set_equation(int cp_id, Draw::Param_Equation& pos_eq, Draw::Param_Equation &orient_eq) const;

signals:
    /// 當有control point 被選中 or 被取消選取都會emit
    /// @param select - true->有被選中；false->沒被選中
    void is_point_selected(bool select);


private:
    std::vector<ControlPoint> m_control_points; ///< 所有控制點
    int m_selected_control_point;  ///< 選中的控制點

    SplineType m_line_type; ///< 線的型式
    float m_cardinal_tension;  ///< tension for cardinal spline

    TrainSystem::Arc_Len_Accum_T m_Arc_Len_Accum; ///< Accumulation of arc length. elem.first = t in "param space", elem.second = s in "real space".

    Shader m_wood_shader;  ///< 繪製木頭支柱
    qtTextureCubeMap m_wood_cube; ///< 木頭的材質

    Box_VAO m_unit_box_VAO; ///< 表示方塊的VAO

    bool m_is_vertical_move; ///< 是否鉛直移動 control point
    bool m_please_update_arc_len_accum; ///< 若為true，則在 TrainSystem::draw() 時會呼叫 TrainSystem::update_arc_len_accum
};

#endif // TRAINSYSTEM_H
