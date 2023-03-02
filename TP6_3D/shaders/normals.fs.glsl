#version 330

in vec3 vVertexPosition;
in vec3 vNormalPosition;
in vec2 vTexPosition;

out vec3 fFragColor;

void main() { fFragColor = vNormalPosition; }
