#version 330

uniform mat4 view;
uniform mat4 projection;

in  vec4 vPosition;
in vec4 vColor;

out  vec4 fColor;

void main() {
  gl_Position = normalize(projection*view * vPosition);
  fColor = vColor;
} 

