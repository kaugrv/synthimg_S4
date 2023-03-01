#version 330 core

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec2 aUVPosition;

out vec3 vFragColor;
out vec2 vFragPosition;

void main() {

  vFragPosition = aVertexPosition;

  gl_Position = vec(aVertexPosition, 0, 1);
};
