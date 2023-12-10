#version 430 core
in vec3 tex_coord;

uniform samplerCube skybox;

out vec4 FragColor;

void main() {
  FragColor = texture(skybox, tex_coord);
}
