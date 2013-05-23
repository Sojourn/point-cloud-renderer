#version 420

#define maxFrag 2

in vec4 fColor;
in vec3 fNormal;
in vec3 fLight;
in vec2 ftexCoord;
in vec3 fEye;

out vec4  Color;


uniform ivec4 vImgSize;          // Size [width, height, storage width, storage height]

uniform layout(binding=0, r32ui)   coherent uimage2DRect  colorImage;
uniform layout(binding=1, r32ui)    coherent uimage2DRect  depthImage;

float transparencyFunction(float x) {
	const float x5 = -12.587f;
	const float x4 = 31.685f;
	const float x3 = -24.432f;
	const float x2 = 4.8005f;
	const float x1 = -.4662;
	const float x0 = 1;

	return clamp(x5*x*x*x*x*x + x4*x*x*x*x + x3*x*x*x + x2*x*x + x1*x + x0, 0.0, 1.0);

}

vec3 saturate(vec3 color) {
	vec3 tore;
	tore.x = clamp(color.x, 0.0, 1.0);
	tore.y = clamp(color.y, 0.0, 1.0);
	tore.z = clamp(color.z, 0.0, 1.0);
	return tore;
}

ivec2 indexImage(int index) {
    ivec2 uv;
    uv.s = index % vImgSize[2];
    uv.t = index / vImgSize[2];
	return uv;
}

void main() {
	float alpha = length(ftexCoord);

	vec3 h = normalize(fLight + fEye);
	
	vec3 ia = vec3(.4f,.4f,.4f);
	ia = ia * fColor.xyz;
	
	float diffuseFactor = max(dot(fLight, fNormal), 0.0);
	vec3 id = diffuseFactor * fColor.xyz;

	vec3 is = fColor.xyz * pow(max(dot(fNormal, h), 0.0), 100);
	
	if (dot(fLight, fNormal) < 0) 
		is = vec3(0.0, 0.0, 0.0);

	Color = vec4(saturate(ia + id + is), transparencyFunction(alpha));
	

	if ((alpha <= 1)) {
		ivec2 coors = ivec2(gl_FragCoord);
		uint depth = uint(gl_FragCoord.z * 0xFFFFFFF); //far plane
		uint color = packUnorm4x8(Color);

		int baseIndex = int(coors.x + coors.y*vImgSize[0]);
		baseIndex *= maxFrag;

		for (int i = 0; i < maxFrag; i++) {
			ivec2 iindex = indexImage(baseIndex+1);

			uint storedDepth = imageAtomicMin(depthImage, iindex, depth);

			if (storedDepth > depth)
				color = imageAtomicExchange(colorImage, iindex, color);

			memoryBarrier();

			depth = storedDepth;
		}

	}
} 

