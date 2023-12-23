#version 430 core
in vec2 texCoord;

uniform sampler2D color_buffer;
uniform sampler2D depth_buffer;
uniform int type = 0;

out vec4 FragColor;
out float gl_FragDepth;

void main () {
  // NoProcess
  if (type == 0) {
    FragColor = texture(color_buffer, texCoord);
  }
  // Pixelization
  else if (type == 1) {
    FragColor = texture(color_buffer, floor(texCoord * 100.f) / 100.f);
  }


  gl_FragDepth = texture(depth_buffer, texCoord).r;
  if (gl_FragDepth == 1)
    gl_FragDepth = 0.999;
}
