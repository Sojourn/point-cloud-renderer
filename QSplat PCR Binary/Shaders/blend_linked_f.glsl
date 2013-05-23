#version 420

in vec4 fColor;
in vec3 fNormal;
in vec3 fLight;
in vec3 fEye;

out vec4  Color;

uniform ivec4 vImgSize;          // Size [width, height, storage width, storage height]
uniform layout(binding=0, offset=0) atomic_uint counter;

//uniform layout(binding=0, rgba32f) coherent image2DRect  image0;
uniform layout(binding=0, r32ui)   coherent uimage2DRect  image0;
uniform layout(binding=1, r32f)    coherent image2DRect  image1;
uniform layout(binding=2, r32i)    coherent iimage2DRect image2;
uniform layout(binding=3, r32i)    coherent iimage2DRect image3;

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

void main() {
	vec3 h = normalize(fLight + fEye);
	
	vec3 ia = vec3(.4f,.4f,.4f);
	ia = ia * fColor.xyz;
	
	float diffuseFactor = max(dot(fLight, fNormal), 0.0);
	vec3 id = diffuseFactor * fColor.xyz;

	vec3 is = fColor.xyz * pow(max(dot(fNormal, h), 0.0), 100);
	
	if (dot(fLight, fNormal) < 0) 
		is = vec3(0.0, 0.0, 0.0);

	Color = vec4(saturate(ia + id + is), transparencyFunction(fColor.w));
	

	// Acquire the next fragment index and calculate the associated texture
    // coordinates
    int idx = int(atomicCounterIncrement(counter));

	if (idx < vImgSize[2] * vImgSize[3]) {
    ivec2 uv;
    uv.s = idx%vImgSize[2];
    uv.t = idx/vImgSize[2];
    
    // Store the color and depth values at the texture coordinates for the
    // current index in the color and depth images
    //imageStore( image0, uv, Color );                  // Color
    imageStore( image0, uv, uvec4(packUnorm4x8(Color),0,0,0) );                  // Color
    imageStore( image1, uv, vec4(gl_FragCoord.z,0,0,0) ); // Depth
    
    // Insert the current fragment index into the head of the linked list and
    // the set the next fragment at current index to the previous head index
    int previdx = imageAtomicExchange( image3, ivec2(gl_FragCoord), idx ); // Head
    imageStore( image2, uv, ivec4(previdx,0,0,0) );                        // Next
	}

} 

