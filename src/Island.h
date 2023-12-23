
#ifndef ISLAND_H
#define ISLAND_H

#include "Shader.h"
#include "Model.h"


class Island
{
private:
    Shader m_shader;
    Model m_model;

public:
    Island();

    void draw(bool wireframe);
};

#endif // ISLAND_H
