#version 330

layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aNormalPosition;
layout(location = 2) in vec2 aTexPosition;

uniform mat4 uMVPMatrix;    // ModelViewProjection Matrix
uniform mat4 uMVMatrix;     // ModelView Matrix
uniform mat4 uNormalMatrix; // Normal Matrix

out vec3 vVertexPosition;
out vec3 vNormalPosition;
out vec2 vTexPosition;

void main() {
  vVertexPosition = vec3((uMVMatrix * vec4(aVertexPosition, 1)).xyz);
  vNormalPosition = vec3((uNormalMatrix * vec4(aNormalPosition, 1)).xyz);
  vTexPosition = aTexPosition;
  gl_Position = vec4((uMVPMatrix * vec4(aVertexPosition, 1)).xy, 0, 1);
}
