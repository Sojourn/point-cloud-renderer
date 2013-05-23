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

out vec3 fColor;
out vec3 fNormal;
out vec3 fLight;
out vec3 fEye;

void main() {
	gl_Position = projection * view * model * vPosition;
	fNormal = normalize((view * model * vec4(vNormal, 0.0)).xyz);
	vec4 temp = lightVector - model * vPosition;
	temp.w = 0;

	fLight = normalize((view * temp).xyz);
	fEye = normalize(-(view * model * vPosition).xyz);
	fColor = vColor.xyz;
} 

