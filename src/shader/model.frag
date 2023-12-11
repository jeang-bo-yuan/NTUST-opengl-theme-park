#version 430 core
in vec2 vs_texcoord;

uniform sampler2D diffuse1;

out vec4 FragColor;

void main() {
  FragColor = texture(diffuse1, vs_texcoord);
}
