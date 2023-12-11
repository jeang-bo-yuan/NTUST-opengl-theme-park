#version 430 core
// 少寫了in，害我debug快三個小時
// layout (location = 0) vec2 aPos;
// layout (location = 1) vec2 aTex;

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTex;

out vec2 TexCoord;

void main() {
  gl_Position = vec4(aPos.x, aPos.y, 0, 1);
  TexCoord = aTex;
//  if (gl_VertexID == 0) {
//    gl_Position = vec4(-1, -1, 0, 1);
//    TexCoord = vec2(0, 0);
//  } else if (gl_VertexID == 1) {
//    gl_Position = vec4(-1, 1, 0, 1);
//    TexCoord = vec2(0, 1);
//  } else if (gl_VertexID == 2) {
//    gl_Position = vec4(1, 1, 0, 1);
//    TexCoord = vec2(1, 1);
//  } else {
//    gl_Position = vec4(1, -1, 0, 1);
//    TexCoord = vec2(1, 0);
//  }
}
