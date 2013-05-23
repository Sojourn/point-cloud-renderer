#version 420

in vec3 fColor;
in vec3 fNormal;
in vec3 fLight;
in vec3 fEye;
in vec2 ftexCoord;
in float fDepth;

out vec4  Color;
layout (depth_less) out float gl_FragDepth;

vec3 saturate(vec3 color) {
	vec3 tore;
	tore.x = clamp(color.x, 0.0, 1.0);
	tore.y = clamp(color.y, 0.0, 1.0);
	tore.z = clamp(color.z, 0.0, 1.0);
	return tore;
}

void main() {
	if (length(ftexCoord) > 1) discard; 
	gl_FragDepth = fDepth;

	vec3 h = normalize(fLight + fEye);

	vec3 ia = vec3(.4f,.4f,.4f);
	ia = ia * fColor.xyz;
		
	float diffuseFactor = max(dot(fLight, fNormal), 0.0);
	vec3 id = diffuseFactor * fColor.xyz*.9f;

	vec3 is = fColor.xyz * pow(max(dot(fNormal, h), 0.0), 100);
	
	if (dot(fLight, fNormal) < 0) 
		is = vec3(0.0, 0.0, 0.0);

	Color = vec4(saturate(ia + id + is), 1);
} 

