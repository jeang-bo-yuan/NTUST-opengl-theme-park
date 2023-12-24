#version 430 core
in vec4 vs_color;
in vec3 vs_normal;
in vec3 vs_world_pos;

layout (std140, binding = 1) uniform LightBlock {
  vec4 eye_position;
  vec4 light_position;
} Light;
uniform vec4 color_ambient = vec4(0.3, 0.3, 0.3, 1.0);
uniform vec4 color_diffuse = vec4(0.5, 0.5, 0.5, 1.0);
uniform vec4 color_specular = vec4(0.5, 0.5, 0.5, 1.0);
uniform float shininess = 1000.0f;

layout (std140, binding = 2) uniform Cel_Shading_Block {
  int on;
  int levels;
} Cel;

out vec4 FragColor;

void main() {
  vec3 light_direction;
  if (Light.light_position.w == 0) {
    light_direction = normalize(Light.light_position.xyz);
  }
  else {
    light_direction = normalize(Light.light_position.xyz - vs_world_pos);
  }
  vec3 EyeDirection = normalize(Light.eye_position.xyz - vs_world_pos);
  vec3 half_vector = normalize(light_direction + EyeDirection);

  float diffuse = max(0.0, dot(vs_normal, light_direction));
  float specular = pow(max(0.0, dot(vs_normal, half_vector)), shininess);
  if (Cel.on == 1) {
    diffuse = floor(diffuse * Cel.levels) / Cel.levels;
    specular = floor(specular * Cel.levels) / Cel.levels;
  }

  FragColor =  min(vs_color * color_ambient, vec4(1.0)) + diffuse * color_diffuse + specular * color_specular;
}
