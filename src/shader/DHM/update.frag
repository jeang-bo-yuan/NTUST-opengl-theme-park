#version 430 core
in vec2 TexCoord;

uniform sampler2D u_water;
uniform vec2 u_dx = vec2(0.01, 0);
uniform vec2 u_dy = vec2(0, 0.01);

out vec4 FragColor;

void main() {
  vec4 info = texture2D(u_water, TexCoord);

  float average_height = (
    texture2D(u_water, TexCoord + u_dx).r +
    texture2D(u_water, TexCoord - u_dx).r +
    texture2D(u_water, TexCoord + u_dy).r +
    texture2D(u_water, TexCoord - u_dy).r
  ) * 0.25;

  float velocity = (info.g) + (average_height - (info.r)) * 2.0;

  velocity *= 0.993;

  info.r += velocity;
  info.g = velocity;

  FragColor = info;
}
