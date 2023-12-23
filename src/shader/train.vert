#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec4 aColor;

// 平移
uniform vec3 translate;
// 縮放
uniform float scale;

uniform vec3 FRONT;
uniform vec3 LEFT;
uniform vec3 TOP;

layout(std140, binding = 0) uniform MatricesBlock {
  uniform mat4 view;
  uniform mat4 proj;
} Matrices;

out vec3 vs_world_pos;
out vec3 vs_normal;
out vec4 vs_color;

void main() {
  mat3 rotate;
  rotate[0] = vec3(FRONT);
  rotate[1] = vec3(TOP);
  rotate[2] = vec3(LEFT);

  vs_world_pos = rotate * (1.8 * scale * aPos) + translate;
  gl_Position = Matrices.proj * Matrices.view * vec4(vs_world_pos, 1);
  vs_normal = normalize(aNormal);
  vs_color = aColor;
}
