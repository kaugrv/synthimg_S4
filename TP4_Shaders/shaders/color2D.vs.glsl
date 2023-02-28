#version 330 core

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec3 aVertexColor;

out vec3 vFragColor;
out vec2 vFragPosition;

mat3 translate(float tx, float ty) {
  return mat3(vec3(1, 0, tx), vec3(0, 1, ty), vec3(0, 0, 1));
}

mat3 scale(float sx, float sy) {
  return mat3(vec3(sx, 0, 0), vec3(0, sy, 0), vec3(0, 0, 1));
}

mat3 rotate(float a) {
  float c = cos(radians(a));
  float s = sin(radians(a));

  return mat3(vec3(c, -s, 0), vec3(s, c, 0), vec3(0, 0, 1));
}

void main() {

  vFragColor = aVertexColor;
  vFragPosition = aVertexPosition;

  gl_Position =
      vec4((rotate(0.) * scale(2, 2) * vec3(aVertexPosition, 1)).xy, 0, 1);
};
