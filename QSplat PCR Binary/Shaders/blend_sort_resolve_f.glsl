#version 420

uniform ivec4 vImgSize;

uniform layout(binding=0, r32ui)   coherent uimage2DRect  colorImage;
uniform layout(binding=1, r32ui)    coherent uimage2DRect  depthImage;

#define maxFrag 2
//change to 32 or less when we have coverage culling, when 

ivec2 indexImage(int index) {
    ivec2 uv;
    uv.s = index % vImgSize[2];
    uv.t = index / vImgSize[2];
	return uv;
}

void main() {

    vec4 col = vec4(0.0, 0.0, 0.0, 0.0);
	uint depth;
	vec4 color;
	
	ivec2 coors = ivec2(gl_FragCoord);
	int baseIndex = coors.x + coors.y*vImgSize[0];
	baseIndex *= maxFrag;

	//for (int i = maxFrag - 1; i >= 0; i--) {
		ivec2 iindex = indexImage(baseIndex + 1);
		
		color = unpackUnorm4x8(imageAtomicExchange(colorImage, iindex, 0));
		depth = imageAtomicExchange(depthImage, iindex, 0xFFFFFFFF);
		
		col.rgb  = (1.0-color.a)*col.rgb;
		col.rgb += color.a*color.rgb;
		col.a    = color.a+((1-color.a)*col.a);
	//}
	    //col.rgb  = (1.0-avCol[i].a)*col.rgb;
        //col.rgb += avCol[i].a*avCol[i].rgb;
        //col.a    = avCol[i].a+((1-avCol[i].a)*col.a);


    gl_FragColor = unpackUnorm4x8(depth);
	//gl_FragDepth = depth / 100.0f;
    
	
}