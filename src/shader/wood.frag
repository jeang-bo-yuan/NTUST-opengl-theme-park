#version 430 core
in vec3 vs_texCoord;

out vec4 FragColor;

uniform samplerCube wood;

void main() {
  FragColor = texture(wood, vs_texCoord);
}
