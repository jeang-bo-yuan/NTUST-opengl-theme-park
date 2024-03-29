#version 430 core
layout (location=0) in vec3 aPosition;
layout (location=1) in vec2 aTexCoord;
layout (location=2) in vec3 aNormal;
layout (location=3) in vec4 aColor;

layout(std140, binding = 0) uniform MatricesBlock {
  uniform mat4 view;
  uniform mat4 proj;
} Matrices;
layout (std140, binding = 3) uniform ClipBlock {
  vec4 plane;
} Clip;

out vec4 vs_color;
out vec2 vs_texcoord;
out vec3 vs_normal;
out vec3 vs_world_pos;

void main() {
  vs_world_pos = aPosition;
  gl_Position = Matrices.proj * Matrices.view * vec4(aPosition, 1);
  vs_color = aColor;
  vs_normal = normalize(aNormal);
  vs_texcoord = aTexCoord;

  gl_ClipDistance[0] = dot(Clip.plane, vec4(vs_world_pos, 1));
}
