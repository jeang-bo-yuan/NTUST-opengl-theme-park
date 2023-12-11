#version 430 core
in vec2 TexCoord;

out vec4 FragColor;

const float PI = 3.1415926;
uniform sampler2D u_water;
uniform vec2 u_center;
uniform float u_radius = 0.05;
uniform float u_strength = 0.05;

void main() {
  vec4 info = texture2D(u_water, TexCoord);

  float drop = max(0, 1 - length(u_center - TexCoord) / u_radius);
  if (drop > 0) {
    float delta_height = (drop - 0.5) * u_strength;
    info.r += delta_height;
  }

  FragColor = info;
}
