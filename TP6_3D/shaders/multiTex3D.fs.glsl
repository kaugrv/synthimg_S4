#version 330

in vec3 vVertexPosition;
in vec3 vNormalPosition;
in vec2 vTexPosition;

uniform sampler2D uPlanetTexture;
uniform sampler2D uCloudTexture;

out vec3 fFragColor;

void main() {
  fFragColor = vec3(texture(uPlanetTexture, vTexPosition).xyz) +
               vec3(texture(uCloudTexture, vTexPosition).xyz);
}
