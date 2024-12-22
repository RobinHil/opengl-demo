#version 330 core
layout (location = 0) out vec4 FragColor;

in vec3 ourColor;
in vec3 barycentric;

uniform float line_size;
uniform vec4 line_color;

void main() {
  vec3 l = step(barycentric, vec3(line_size));
  float k = max(max(l.x, l.y), l.z);

  FragColor = vec4(mix(ourColor, line_color.rgb, k  * line_color.a), 1.0);
}