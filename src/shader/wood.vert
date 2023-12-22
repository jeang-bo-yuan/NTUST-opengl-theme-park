#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec3 aNormal;

layout(std140, binding = 0) uniform MatricesBlock {
  uniform mat4 view;
  uniform mat4 proj;
} Matrices;

out vec3 vs_texCoord;

// Control Point的位置
uniform vec3 cp_pos;
// Control Point的大小
uniform float cp_size;

void main() {
  // scale
  vec3 world_pos = aPos * cp_size;
  // translate
  world_pos += cp_pos;
  world_pos.y -= 2 * cp_size;

  // 朝上的面
  if (aNormal == vec3(0, 1, 0)) {
    // do nothing
  }
  // 朝下的面
  else if (aNormal == vec3(0, -1, 0)) {
    world_pos.y = -1;
  }
  // 四周
  else {
    if (aPos.y == -1)
      world_pos.y = -1;
  }

  gl_Position = Matrices.proj * Matrices.view * vec4(world_pos, 1);
  vs_texCoord = aPos;
}
