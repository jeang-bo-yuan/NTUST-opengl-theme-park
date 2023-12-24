#version 430 core
in vec2 texCoord;

uniform sampler2D color_buffer;
uniform sampler2D depth_buffer;
uniform ivec2  size;  // size.x -> buffer width;   size.y -> buffer height
uniform int type = 0;

out vec4 FragColor;
out float gl_FragDepth;

// 計算灰階
float grayscale(vec4 color) { return 0.299 * color.r + 0.587 * color.g + 0.114 * color.b; }

void main () {
  // NoProcess
  if (type == 0) {
    FragColor = texture(color_buffer, texCoord);
  }
  // Pixelization
  else if (type == 1) {
    FragColor = texture(color_buffer, floor(texCoord * 100.f) / 100.f);
  }
  // Grayscale
  else if (type == 2) {
    vec4 color = texture(color_buffer, texCoord);
    float gray = grayscale(color);
    FragColor = vec4(gray, gray, gray, 1);
  }
  // DepthImage
  else if (type == 3) {
    float depth = texture(depth_buffer, texCoord).r;
    depth = 3 * cos(3.14159f / 2.f * depth);
    FragColor = vec4(depth, depth, depth, 1);
  }
  // Sobel Operator
  else if (type == 4) {
    vec4 image[9];  // (dx,dy)   col0       col1        col2
                    // row0      左上(-1,1)  中上(0,1)   右上(1,1)
                    // row1      左(-1,0)    中(0,0)    右(1,0)
                    // row2      左下(-1,-1) 中下(0,-1)  右下(1,-1)
    for(int dy = 1; dy >= -1; --dy) {
      for (int dx = -1; dx <= 1; ++dx) {
        vec4 color = texture(color_buffer, texCoord + vec2(dx / float(size.x), dy / float(size.y)));
        int row = -dy + 1;
        int col = dx + 1;

        image[row * 3 + col] = color;
      }
    }

    vec4 Gx = image[0] + 2 * image[3] + image[6] - image[2] - 2 * image[5] - image[8];
    vec4 Gy = image[0] + 2 * image[1] * image[2] - image[6] - 2 * image[7] - image[8];
    vec4 G = sqrt(Gx * Gx + Gy * Gy);

    FragColor = vec4(G.xyz, 1);
  }


  gl_FragDepth = texture(depth_buffer, texCoord).r;
  if (gl_FragDepth == 1)
    gl_FragDepth = 0.999;
}
