#version 430 core
in vec3 vs_world_pos;
in vec3 vs_normal;

layout (std140, binding = 1) uniform LightBlock {
  vec4 eye_position;
  vec4 light_position;
} Light;
uniform samplerCube skybox;
uniform uint how_to_render = 0;
uniform float WAVE_SIZE;

out vec4 FragColor;

vec4 applyLight();

// 計算交點
// P -> 線上一點，R -> 方向向量
// 和 y = WAVE_SIZE 的交點
vec3 intersect_pY(vec3 P, vec3 R) {
  float alpha = 1.0 / R.y * (WAVE_SIZE - P.y);
  return P + alpha * R;
}
// 和 y = -WAVE_SIZE 的交點
vec3 intersect_nY(vec3 P, vec3 R) {
  float alpha = 1.0 / R.y * (-WAVE_SIZE - P.y);
  return P + alpha * R;
}
// 和 x = WAVE_SIZE 的交點
vec3 intersect_pX(vec3 P, vec3 R) {
  float alpha = 1.0 / R.x * (WAVE_SIZE - P.x);
  return P + alpha * R;
}
// 和 x = -WAVE_SIZE 的交點
vec3 intersect_nX(vec3 P, vec3 R) {
  float alpha = 1.0 / R.x * (-WAVE_SIZE - P.x);
  return P + alpha * R;
}
// 和 z = WAVE_SIZE 的交點
vec3 intersect_pZ(vec3 P, vec3 R) {
  float alpha = 1.0 / R.z * (WAVE_SIZE - P.z);
  return P + alpha * R;
}
// 和 z = -WAVE_SIZE 的交點
vec3 intersect_nZ(vec3 P, vec3 R) {
  float alpha = 1.0 / R.z * (-WAVE_SIZE - P.z);
  return P + alpha * R;
}

void main() {
  if (how_to_render == 0) {
    FragColor = vec4(1, 1, 1, 1);
    FragColor = applyLight();
  }
  else {
    vec3 I = normalize(vs_world_pos - Light.eye_position.xyz);
    vec3 normal = vs_normal;

    if (dot(normal, I) > 0) normal = -normal;

    vec3 R;
    if (how_to_render == 1) {
      R = reflect(I, normal);
    }
    else if (how_to_render == 2) {
      R = refract(I, normal, 1.f / 1.33f);
    }

    vec3 P = vs_world_pos;
    vec3 intersection[3];
    if (R.x > 0) intersection[0] = intersect_pX(P, R);
    else         intersection[0] = intersect_nX(P, R);

    if (R.y > 0) intersection[1] = intersect_pY(P, R);
    else         intersection[1] = intersect_nY(P, R);

    if (R.z > 0) intersection[2] = intersect_pZ(P, R);
    else         intersection[2] = intersect_nZ(P, R);

    // 找最近的intersection
    float min_len = length(P - intersection[0]);
    int min_id = 0;
    for (int i = 1; i < 3; ++i) {
      float len = length(P - intersection[i]);
      if (len < min_len) {
        min_len = len;
        min_id = i;
      }
    }

    FragColor = texture(skybox, intersection[min_id]);
//    FragColor = applyLight();
  }

}

uniform vec4 color_ambient = vec4(0.1, 0.2, 0.5, 1.0);
uniform vec4 color_diffuse = vec4(0.2, 0.3, 0.6, 1.0);
uniform vec4 color_specular = vec4(0.2, 0.3, 0.6, 1.0);
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
