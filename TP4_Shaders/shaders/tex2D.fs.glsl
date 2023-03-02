#version 330 core

in vec3 vFragColor;
in vec2 vFragPosition;
in vec2 vTexturePosition;

out vec3 fFragColor;
out vec2 fFragPosition;

uniform vec3 uColor;
uniform sampler2D uTexture;

void main() {
  fFragColor = vec3(texture(uTexture, vTexturePosition).x,
                    texture(uTexture, vTexturePosition).y,
                    texture(uTexture, vTexturePosition).z);
}