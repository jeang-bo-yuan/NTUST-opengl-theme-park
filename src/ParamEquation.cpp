/*****************************************************************//**
 * \file Draw.cpp
 * \author 蔣博元
 * \date 2023/10/14
 * \brief 實作Draw.H中的function
 *********************************************************************/
#include "ParamEquation.h"
#include <math.h>
#include <glm/gtc/type_ptr.hpp>

Draw::Param_Equation Draw::make_line(const glm::vec3 p1, const glm::vec3 p2) {
    return [p1, p2](float t) -> glm::vec3 {
		return (1.f - t) * p1 + t * p2;
	};
}

Draw::Param_Equation Draw::make_cubic_b_spline(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
	static const glm::mat4 M (
		-1 / 6.f,  3 / 6.f, -3 / 6.f, 1 / 6.f, // first column
		 3 / 6.f, -6 / 6.f,  3 / 6.f,    0,    // second column
		-3 / 6.f,    0    ,  3 / 6.f,    0,    // third column
		 1 / 6.f,  4 / 6.f,  1 / 6.f,    0     // forth column
	);

	// 4 columns and 3 rows
	glm::mat4x3 G(
        p0, // first column
        p1, // second column
        p2, // third column
        p3  // forth column
	);

    return [G](float t) -> glm::vec3 {
		glm::vec4 T(t * t * t, t * t, t, 1);
		glm::vec3 Q = G * M * T;
        return Q;
	};
}

Draw::Param_Equation Draw::make_cardinal(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float tension) {
	const glm::mat4 M(
           -tension,  2 - tension,      tension - 2,   tension,  // first column
        2 * tension,  tension - 3,  3 - 2 * tension,  -tension,  // second column
           -tension,            0,          tension,         0,  // third column
                  0,            1,          0,               0   // forth column
	);

	// 4 columns and 3 rows
	glm::mat4x3 G(
        p0, // first column
        p1, // second column
        p2, // third column
        p3  // forth column
	);

    return [G, M](float t) -> glm::vec3 {
		glm::vec4 T(t * t * t, t * t, t, 1);
		glm::vec3 Q = G * M * T;
        return Q;
	};
}
