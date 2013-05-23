#include <stdio.h>
#include <string.h>
#include <float.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "MappedFile.h"
#include "Endian.h"
using namespace std;
#include "qsplat_make_from_pts.h"

// Try to read a pts file, returning vertices
bool read_pts(const char *ptsfile,
	      int &numleaves, QTree_Node * &leaves,
	      bool &have_colors,
	      string &comments)
{
	MappedFile mf;
	if(!mf.open(ptsfile)){
		return false;	
	};
	uint8_t * file = mf.data();
	int other_prop_len, color_offset;
	string leafNum;
	int i, result;
	have_colors = true;  
	numleaves = 0;
	leaves = NULL;
	float xCoord, yCoord, zCoord, xNorm, yNorm, zNorm, rColor, gColor, bColor;

	cout << "Reading " << ptsfile << "...\n";
	
	// Read header (number of points)
	numleaves = *((uint32_t*) file);

	leaves = new QTree_Node[numleaves];

	cout << "Reading " << numleaves << " vertices...\n";
	
	float *section = (float*)(file + sizeof(uint32_t));
	for (i=0; i < numleaves; i++) {
		leaves[i].pos[0] = *section; section++;
		leaves[i].pos[1] = *section; section++;
		leaves[i].pos[2] = *section; section++;
		leaves[i].r = 0.1;
	}
	for (i = 0; i < numleaves; i++){
		leaves[i].norm[0] = *section; section++;
		leaves[i].norm[1] = *section; section++;
		leaves[i].norm[2] = *section; section++;
	}
	for (i = 0; i < numleaves; i++){
		rColor = *section; section++;
		gColor = *section; section++;
		bColor = *section; section++;
		leaves[i].col[0] = (unsigned char) (((rColor) * 255));
		leaves[i].col[1] = (unsigned char) (((gColor) * 255));
		leaves[i].col[2] = (unsigned char) (((bColor) * 255));
	}
	for (i = 0; i < numleaves; i++){
		leaves[i].m.refcount = 1;
		leaves[i].m.remap = i;
	}
	cout << "Done.\n";
	return true;
}