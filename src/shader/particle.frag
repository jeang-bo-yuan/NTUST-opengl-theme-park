#version 430 core
in vec2 vs_texcoord;

uniform sampler2D img;

out vec4 FragColor;

void main() {
  FragColor = texture(img, vs_texcoord);
  if (FragColor.a == 0) discard;
}
