#version 330 core

in vec3 vFragColor;
in vec2 vFragPosition;

out vec3 fFragColor;
out vec2 fFragPosition;

uniform vec3 uColor;

void main() { fFragColor = uColor; }