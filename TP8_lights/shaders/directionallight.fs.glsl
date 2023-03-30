#version 330

in vec3 vVertexPosition;
in vec3 vNormalPosition;
in vec2 vTexPosition;

uniform vec3 uKd;
uniform vec3 uKs;
uniform float uShininess;

uniform vec3 uLightDir_vs;
uniform vec3 uLightIntensity;

out vec3 fFragColor;

vec3 blinnPhong() {
  auto Li = uLightIntensity;
  auto Kd = uKd;
  auto Ks = uKs;
  auto wi = normalize(uLightIntensity);
  auto w0 = normalize(-vVertexPosition);
  auto N = vNormalPosition;
  auto halfVector = (w0 + wi) / 2;
  return Li * (Kd * dot(wi, N) + Ks * pow(dot(halfVector * N), uShininess));
}

void main() { fFragColor = blinnPhong(); }
