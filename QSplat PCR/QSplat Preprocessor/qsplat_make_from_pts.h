#ifndef QSPLAT_MAKE_FROM_PTS_H
#define QSPLAT_MAKE_FROM_PTS_H
/*
Szymon Rusinkiewicz

qsplat_make_from_mesh.h
Code for reading in a triangle mesh from a ply file and initializing the
QSplat data structure.

Copyright (c) 1999-2000 The Board of Trustees of the
Leland Stanford Junior University.  All Rights Reserved.
*/

/*
Modified by Christopher Jeffers in qsplat_make_from_pts.h
*/

#include "qsplat_make_qtree_v11.h"

extern bool read_pts(const char *plyfile,
		     int &numleaves, QTree_Node * &leaves,
		     bool &havecolor,
		     std::string &comments);
/*extern void merge_nodes(int &numleaves, QTree_Node *leaves,
			int &numfaces, face *faces,
			bool havecolor, float thresh);
extern void find_splat_sizes(int numleaves, QTree_Node *leaves,
		             int numfaces, const face *faces);*/

#endif
