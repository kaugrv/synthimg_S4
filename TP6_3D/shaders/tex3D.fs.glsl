#version 330

in vec3 vVertexPosition;
in vec3 vNormalPosition;
in vec2 vTexPosition;

uniform sampler2D uTexture;

out vec3 fFragColor;

void main() {
  fFragColor =
      vec3(texture(uTexture, vTexPosition).x, texture(uTexture, vTexPosition).y,
           texture(uTexture, vTexPosition).z);
}
