
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

public:
    TrainSystem();

    void draw(bool wireframe);

private:
    void draw_control_points();
};

#endif // TRAINSYSTEM_H
