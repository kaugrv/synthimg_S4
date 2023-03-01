#version 330 core

in vec3 vFragColor;
in vec2 vFragPosition;

out vec3 fFragColor;
out vec2 fFragPosition;

vec2 complexSqr(vec2 z) {
  return vec2(z.x * z.x - z.y * z.y, z.x * z.y + z.x * z.y);
}

void main() {

  vec2 c = vFragPosition.xy;
  vec2 z = c;

  int N = 500;
  int i = 0;
  for (; i < N; i++) {

    z = complexSqr(z) + c;

    if (length(z) > 2.) {
      fFragColor = vec3(0, 0, 0);
      return;
    }
  }

  fFragColor = vec3(float(i) / N);
}
