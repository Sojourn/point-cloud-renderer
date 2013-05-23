#version 330

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 lightVector;

in  vec4 vPosition;
in  vec3 vNormal;
in  vec4 vColor;
in  float vRadius;
in  int vNormalCone;


out  vec3 gNormal;
out  vec4 gColor;
out  float gRadius;

void main() {
  gl_Position = vPosition;
  gNormal = vNormal;
  gColor = vColor;
  gRadius = vRadius;
} 

