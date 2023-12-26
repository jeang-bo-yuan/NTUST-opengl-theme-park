#version 430 core
in vec3 vs_world_pos;
in vec4 vs_clipspace;
in vec3 vs_normal;

layout (std140, binding = 1) uniform LightBlock {
  vec4 eye_position;
  vec4 light_position;
} Light;
uniform uint how_to_render = 0;
uniform float factor;
uniform float WAVE_SIZE;
uniform sampler2D reflection_texture;
uniform sampler2D refraction_texture;

out vec4 FragColor;

vec4 applyLight();

void main() {
  if (how_to_render == 0) {
    FragColor = vec4(1, 1, 1, 1);
    FragColor = applyLight();
  }
  else {
    vec2 NDC = (vs_clipspace.xy / vs_clipspace.w) / 2.f + 0.5f;

    vec4 reflect_color = texture(reflection_texture, vec2(NDC.x, 1 - NDC.y));
    vec4 refract_color = texture(refraction_texture, NDC);
    if (how_to_render == 1)
      FragColor = mix(reflect_color, refract_color, factor);
    else {
      // TODO: Fresnel
      FragColor = mix(reflect_color, refract_color, 0.5);
    }
    FragColor = applyLight();
  }

}

uniform vec4 color_ambient = vec4(0.1, 0.2, 0.5, 1.0);
uniform vec4 color_diffuse = vec4(1, 1, 1, 1.0);
uniform vec4 color_specular = vec4(1, 1, 1, 1.0);
uniform float shininess = 77.0f;

layout (std140, binding = 2) uniform Cel_Shading_Block {
  int on;
  int levels;
} Cel;

vec4 applyLight() {
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

    return min(FragColor * color_ambient, vec4(1.0)) + diffuse * color_diffuse + specular * color_specular;
}
