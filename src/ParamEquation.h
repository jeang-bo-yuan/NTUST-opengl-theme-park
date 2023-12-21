/*****************************************************************//**
 * \file ParamEquation.H
 * \author 蔣博元
 * \date 2023/10/14
 * \brief 畫東西的函數
 *********************************************************************/
#pragma once
#include <functional>
#include <glm/vec3.hpp>

/// @brief 參數式
namespace Draw {

	/// @brief Param_Equation 是一個function object，用來表示線的參數式
	/// @details 參數 - 參數式的t， 回傳 - 點
    typedef std::function<glm::vec3(float)> Param_Equation;

	/// @brief 建立一個直線的參數式
	/// @param p1 - t=0 時的點
	/// @param p2 - t=1 時的點
    Param_Equation make_line(const glm::vec3 p1, const glm::vec3 p2);

	/// @brief 建立Cubic B-spline的參數式
	/// @param p0 - 更後面的控制點
	/// @param p1 - 開始
	/// @param p2 - 結束
	/// @param p3 - 更前面的控制點
    Param_Equation make_cubic_b_spline(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);

	/// @brief 建立Cardinal Spline的參數式
	/// @param p0 - 更後面的控制點
	/// @param p1 - 開始
	/// @param p2 - 結束
	/// @param p3 - 更前面的控制點
	/// @param tension - 
    Param_Equation make_cardinal(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float tension);

} // namespace Draw
