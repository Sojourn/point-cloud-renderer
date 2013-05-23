#version 420

uniform ivec4 vImgSize;
//uniform layout(binding=0, rgba32f) coherent image2DRect  image0;
uniform layout(binding=0, r32ui)   coherent uimage2DRect  image0;
uniform layout(binding=1, r32f)    coherent image2DRect  image1;
uniform layout(binding=2, r32i)    coherent iimage2DRect image2;
uniform layout(binding=3, r32i)    coherent iimage2DRect image3;

#define maxFrag 60
//change to 32 or less when we have coverage culling, when 

void main() {

    //int idx  = imageLoad( image3, ivec2(gl_FragCoord) ).x;
    int idx  = imageAtomicExchange( image3, ivec2(gl_FragCoord), -1 );
    
	if ( idx == -1 ) discard;
	
    ivec2 uv;        // Image coordinates
    int   nFrag = 0; // Number of fragments
    vec4  avCol[maxFrag]; // Color cache
    float aDep[maxFrag];  // Depth cache
    int   aIdx[maxFrag];  // Index cache
    int   iTmpIDx;   // Temp index
    
    // Cache the depth and color
    while( idx != -1 && nFrag < maxFrag ) {
        // Acquire the image coordinate of the index
        uv.s = idx%vImgSize[2];
        uv.t = idx/vImgSize[2];
        
        // Cache the fragment information
        //avCol[nFrag] = imageLoad( image0, uv );
        avCol[nFrag] = unpackUnorm4x8(imageLoad( image0, uv ).x);
        aDep[nFrag]  = imageLoad( image1, uv ).x;
        aIdx[nFrag]  = nFrag;
        nFrag++;
        
        // Acquire the next fragment index
        idx = imageLoad( image2, uv ).x;
    }

    // Execute sort
    int nSortFrag = min(nFrag,maxFrag);
    bool bSwap = true;
    while (bSwap) {
        bSwap = false;
        for ( int n=1 ; n<nSortFrag ; n++ ) {
            int i = aIdx[n-1];
            int j = aIdx[n];
            if ( aDep[j] < aDep[i] ) {
                aIdx[n-1] = j;
                aIdx[n]   = i;
                bSwap     = true;
            }
        }
    }
    
    // Render the pixels
    vec4 col = vec4(0.0, 0.0, 0.0, 0.0);
    int nBlendFrag = min(nFrag-1,maxFrag-1);
    for ( int n=nBlendFrag ; n>=0 ; n-- ) {
        int i = aIdx[n];
        col.rgb  = (1.0-avCol[i].a)*col.rgb;
        col.rgb += avCol[i].a*avCol[i].rgb;
        col.a    = avCol[i].a+((1-avCol[i].a)*col.a);
    }

    gl_FragColor = col;
    gl_FragDepth = aDep[0];
	
}