#version 330 core

in vec3 ourColor;

out vec4 outColor;

void main() {
  //vec3 color = vec3(1.0f, 0.5f, 0.2f);
  vec3 color = ourColor;
  outColor = vec4(color, 1.0f);
}
