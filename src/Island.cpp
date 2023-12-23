
#include "Island.h"

Island::Island()
    : m_shader("shader/model.vert", nullptr, nullptr, nullptr, "shader/model.frag"), m_model("asset/model/island/Island.fbx")
{

}

void Island::draw(bool wireframe)
{
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    m_shader.Use();
    m_model.draw();
    glUseProgram(0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

