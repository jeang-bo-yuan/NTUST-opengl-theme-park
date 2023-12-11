#version 430 core
layout (location=0) in vec3 aPosition;
layout (location=1) in vec2 aTexCoord;
layout (location=2) in vec3 aNormal;

layout(std140, binding = 0) uniform MatricesBlock {
  uniform mat4 view;
  uniform mat4 proj;
} Matrices;

out vec2 vs_texcoord;

void main() {
  gl_Position = Matrices.proj * Matrices.view * vec4(aPosition, 1);
  vs_texcoord = aTexCoord;
}
