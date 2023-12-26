#version 430 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aTranslate;

layout(std140, binding = 0) uniform MatricesBlock {
  uniform mat4 view;
  uniform mat4 proj;
} Matrices;
layout (std140, binding = 1) uniform LightBlock {
  vec4 eye_position;
  vec4 light_position;
} Light;
layout (std140, binding = 3) uniform ClipBlock {
  vec4 plane;
} Clip;
uniform float size;

out vec2 vs_texcoord;

// Reference: wikipedia
// 生成一個旋轉矩陣，當x軸朝上時，逆時針轉angle弧（右手定則）
mat4 rotationX(float angle) {
  return mat4(1,          0,           0, 0,
              0, cos(angle), sin(angle),  0,
              0, -sin(angle), cos(angle), 0,
              0,           0,          0, 1);
}
mat4 rotationY(float angle) {
  return mat4(cos(angle), 0, -sin(angle), 0,
              0,          1,          0, 0,
              sin(angle), 0, cos(angle), 0,
              0,          0,          0, 1);
}

void main() {
  vec3 eye = Light.eye_position.xyz - aTranslate;

  mat4 rotate;
  float theta1 = atan(eye.y / abs(eye.z));
  rotate = rotationX(-theta1);

  float theta2 = atan(eye.x, eye.z);
  rotate = rotationY(theta2) * rotate;

  vec4 world_pos = rotate * vec4(size * aPos, 0, 1);
  world_pos.xyz += aTranslate;
  gl_Position = Matrices.proj * Matrices.view * world_pos;
  vs_texcoord = aTexCoord;

  gl_ClipDistance[0] = dot(Clip.plane, world_pos);
}
