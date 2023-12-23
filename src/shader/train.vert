#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec3 aNormal;

layout(location = 3) in vec4 aColor1; // color set 1
layout(location = 4) in vec4 aColor2;
layout(location = 5) in vec4 aColor3;

// 平移
uniform vec3 translate;
// 縮放
uniform float scale;

uniform vec3 FRONT;
uniform vec3 LEFT;
uniform vec3 TOP;

uniform int index; // 0->火車頭  大於0->車廂

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

  vs_world_pos = rotate * (scale * aPos) + translate;
  gl_Position = Matrices.proj * Matrices.view * vec4(vs_world_pos, 1);
  vs_normal = normalize(aNormal);

  if (index == 0)
    vs_color = aColor1;
  else{
    switch(index % 3) {
    case 0:
      vs_color = aColor3;
      break;
    case 1:
      vs_color = aColor1;
      break;
    case 2:
      vs_color = aColor2;
      break;
    }
  }
}
