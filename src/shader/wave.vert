#version 430 core
layout(location = 0) in vec3 pos;

layout(std140, binding = 0) uniform MatricesBlock {
  uniform mat4 view;
  uniform mat4 proj;
} Matrices;
uniform uint frame;
uniform sampler2D height_map;
uniform float WAVE_SIZE;
uniform bool use_height_map;

out vec3 vs_world_pos;
out vec3 vs_normal;


void main() {
  if (use_height_map) {
    vec2 TexCoord = clamp((vec2(pos.x , pos.z) + WAVE_SIZE) / (2.f * WAVE_SIZE), 0, 1);

    vec4 info = texture2D(height_map, TexCoord);
    vs_world_pos = vec3(pos.x, (info.r * 0.5) - 0.5, pos.z);
    gl_Position = Matrices.proj * Matrices.view * vec4(vs_world_pos, 1);

    float delta = 0.00005;
    // TexCoord上x加delta後，y的變化量
    float dy_x = (texture2D(height_map, TexCoord + vec2(delta, 0)) - info).r;
    dy_x *= 0.5;
    // TexCoord上z加delta後，y的變化量
    float dy_z = (texture2D(height_map, TexCoord + vec2(0, delta)) - info).r;
    dy_z *= 0.5;

    vs_normal = normalize(
      cross(vec3(0, dy_z, delta), vec3(delta, dy_x, 0))
    );
  }
  else {
    // changes over frames
    float offset = float(frame) / 20;

    // y  = 0.05 * sin(2 * pi * x)
    // y' = 0.05 * 2 * pi * cos(2 * pi * x)
    vs_world_pos = vec3(pos.x, 0.05 * sin(2 * 3.14 * (pos.x + offset)), pos.z);
    vs_world_pos.y -= 0.5;
    gl_Position = Matrices.proj * Matrices.view * vec4(vs_world_pos, 1);

    float slope = 0.05 * 2 * 3.14 * cos(2 * 3.14 * (pos.x + offset));
    vs_normal = normalize(vec3(-slope, 1, 0));
  }
}
