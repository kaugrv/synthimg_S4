#version 330 core

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec2 aUVPosition;

out vec3 vFragColor;
out vec2 vFragPosition;

uniform mat3 uModelMatrix;

void main() {

  vFragPosition = aVertexPosition;

  gl_Position = vec4((uModelMatrix * vec3(aVertexPosition * 0.5, 1)).xy, 0, 1);
};
