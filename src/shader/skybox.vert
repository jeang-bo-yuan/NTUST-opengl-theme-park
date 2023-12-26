#version 430 core
layout (location = 0) in vec3 position;

layout(std140, binding = 0) uniform MatricesBlock {
  uniform mat4 view;
  uniform mat4 proj;
} Matrices;

out vec3 tex_coord;

void main() {
  gl_Position = Matrices.proj * mat4(mat3(Matrices.view)) * vec4(position, 1);
  gl_ClipDistance[0] = 0;
  tex_coord = position;
}
