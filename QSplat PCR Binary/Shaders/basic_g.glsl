#version 330

//12 = Good, 24 = Great, 64 = max (NVidia 570)

layout (points) in;
layout (triangle_strip, max_vertices = 60) out; //max_vertices = 3 * num_triangles

const int num_triangles = 6;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 lightVector;

in vec3 gNormal[1];
in vec4 gColor[1];
in float gRadius[1];

out vec4 fColor;
out vec3 fNormal;
out vec3 fLight;
out vec3 fEye;


vec3 getIvector(vec3 normal) {
	if (dot(normal, vec3(1,0,0)) == 0.0f) {
		return normalize(cross(normal, normal + vec3(1,0,0)));
	} else {
		return normalize(cross(normal, normal + vec3(0,1,0)));
	}
}

void SetVertex(vec4 position, vec3 normal, vec4 color, float z, float alpha) {
		gl_Position = projection * view * model * position;
		gl_Position = gl_Position / gl_Position.w;
		gl_Position.z = z;
		fColor = vec4(color.xyz, alpha);
		fNormal = normalize((view * model * vec4(normal, 0.0)).xyz);

		vec4 temp = lightVector - model * position;
		temp.w = 0;

		fLight = normalize((view * temp).xyz);
		fEye = normalize(-(view * model * position).xyz);
		EmitVertex();
}

void main() {
	
	float angle = 0;
	float delta = 2 * 3.14159265358979323846 / num_triangles;
	
	vec3 normal = normalize(gNormal[0].xyz);
	vec3 i = getIvector(normal);
	vec3 j = cross(normal, i);

	float z = (projection *view * model * gl_in[0].gl_Position).z / (projection *view * model * gl_in[0].gl_Position).w;


	for (int m = 0; m < num_triangles; m++) {
		SetVertex(gl_in[0].gl_Position, normal, gColor[0], z,0);
		
		vec4 rVector = vec4(gRadius[0] * (cos(angle) * i + sin(angle) * j), 0);
		SetVertex(gl_in[0].gl_Position + rVector, normal, gColor[0], z, 1);
		
		angle += delta;
		
		rVector = vec4(gRadius[0] * (cos(angle) * i + sin(angle) * j), 0);
		SetVertex(gl_in[0].gl_Position + rVector, normal, gColor[0], z,1);
		
		EndPrimitive();
	
	}

}

