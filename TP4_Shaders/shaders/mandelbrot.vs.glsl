#version 330 core

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec3 aVertexColor;

out vec3 vFragColor;
out vec2 vFragPosition;

void main() {

  vFragColor = aVertexColor;
  vFragPosition = aVertexPosition;
  gl_Position = vec4(aVertexPosition, 0, 1);
};
