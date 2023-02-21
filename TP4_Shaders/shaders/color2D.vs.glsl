#version 330 core

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec3 aVertexColor;

out vec3 vFragColor;

void main() {
  vFragColor = aVertexColor;
  gl_Position = vec4(aVertexPosition, 0, 1) + vec4(0.5, 0.5, 0, 1);
};