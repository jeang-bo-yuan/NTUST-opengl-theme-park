#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 2) in vec3 aNormal;

layout(std140, binding = 0) uniform MatricesBlock {
  uniform mat4 view;
  uniform mat4 proj;
} Matrices;
layout (std140, binding = 3) uniform ClipBlock {
  vec4 plane;
} Clip;
uniform mat4 model_matrix;

out vec3 vs_world_pos;
out vec3 vs_normal;


void main() {
  vec4 world_pos = model_matrix * vec4(aPos, 1);
  vs_world_pos = world_pos.xyz;
  gl_Position = Matrices.proj * Matrices.view * world_pos;
  gl_ClipDistance[0] = dot(Clip.plane, world_pos);

  vec4 normal = model_matrix * vec4(aNormal, 0);
  vs_normal = normalize(normal.xyz);
}
