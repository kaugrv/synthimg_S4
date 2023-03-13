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
  // Passage en coordonnées homogènes
  vec4 vertexPosition = vec4(aVertexPosition, 1);
  vec4 vertexNormal = vec4(aNormalPosition, 0);

  // Calcul des valeurs de sortie
  vVertexPosition = vec3(uMVMatrix * vertexPosition);
  vNormalPosition = vec3(uNormalMatrix * vertexNormal);
  vTexPosition = aTexPosition;

  // Calcul de la position projetée
  gl_Position = uMVPMatrix * vertexPosition;
}
