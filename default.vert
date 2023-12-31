#version 330 core
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

out vec3 ourColor;

void main() {
  gl_Position = vec4(inPos.x, inPos.y, inPos.z, 1.0);
  ourColor = inColor;
}
