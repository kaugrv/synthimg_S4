#version 330 core

in vec3 vFragColor;
in vec2 vFragPosition;

out vec3 fFragColor;
out vec2 fFragPosition;

void main() {
  float alpha = 3;
  float beta = 30.;

  // flou joli
  float a =
      alpha * exp(-beta * length(vFragPosition.xy) * length(vFragPosition.xy));

  // repeat
  a = length(fract(5.0 * vFragPosition));

  // repeat + rond flou
  a = length(abs(fract(5.0 * vFragPosition) * 2.0 - 1.0));

  // damier rigolo
  a = mod(floor(10.0 * vFragPosition.x) + floor(10.0 * vFragPosition.y), 2.0);

  // gruyère
  a = smoothstep(0.3, 0.32, length(fract(5.0 * vFragPosition) - 0.5));

  // briques
  a = smoothstep(0.4, 0.5,
                 max(abs(fract(8.0 * vFragPosition.x -
                               0.5 * mod(floor(8.0 * vFragPosition.y), 2.0)) -
                         0.5),
                     abs(fract(8.0 * vFragPosition.y) - 0.5)));

  fFragColor = a * vFragColor;
}