#version 330

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 lightVector;

in vec3 gNormal[1];
in vec4 gColor[1];
in float gRadius[1];

out vec3 fColor;
out vec3 fNormal;
out vec3 fLight;
out vec3 fEye;
out vec2 ftexCoord;


vec3 getIvector(vec3 normal) {
	if (dot(normal, vec3(1,0,0)) == 0.0f) {
		return normalize(cross(normal, normal + vec3(1,0,0)));
	} else {
		return normalize(cross(normal, normal + vec3(0,1,0)));
	}
}

void SetVertex(vec4 position, vec3 normal, float z, vec2 texs) {
		gl_Position = projection * view * model * position;
		gl_Position = gl_Position / gl_Position.w;
		gl_Position.z = z;
		fColor = gColor[0].xyz;
		fNormal = normalize((view * model * vec4(normal, 0.0)).xyz);

		vec4 temp = lightVector - model * position;
		temp.w = 0;

		fLight = normalize((view * temp).xyz);
		fEye = normalize(-(view * model * position).xyz);

		ftexCoord = texs;
		EmitVertex();
}

void main() {
	
	vec3 normal = normalize(gNormal[0].xyz);
	vec3 i = getIvector(normal);
	vec3 j = cross(normal, i);
	
	vec4 val = projection *view * model * gl_in[0].gl_Position;
	float z = val.z / val.w;

	vec3 rVector;
	
	rVector = gRadius[0]*(-i + -j);
	SetVertex(gl_in[0].gl_Position + vec4(rVector, 0), normal, z, vec2(-1,-1));

	rVector = gRadius[0]*(i + -j);
	SetVertex(gl_in[0].gl_Position + vec4(rVector, 0), normal, z,vec2(1,-1));

	rVector = gRadius[0]*(-i + j);
	SetVertex(gl_in[0].gl_Position + vec4(rVector, 0), normal, z, vec2(-1,1));
	
	rVector = gRadius[0]*(i + j);
	SetVertex(gl_in[0].gl_Position + vec4(rVector, 0), normal, z, vec2(1,1));


	EndPrimitive();
}

