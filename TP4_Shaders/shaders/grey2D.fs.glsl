#version 330 core

in vec3 vFragColor;

out vec3 fFragColor;

void main() {
  fFragColor = vec3((vFragColor.r + vFragColor.g + vFragColor.b) / 3.);
};