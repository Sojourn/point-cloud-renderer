#ifndef QSPLAT_MAKE_QTREE_V11_H
#define QSPLAT_MAKE_QTREE_V11_H
/*
Szymon Rusinkiewicz

qsplat_make_qtree_v11.h
Data structure for the tree.

Copyright (c) 1999-2000 The Board of Trustees of the
Leland Stanford Junior University.  All Rights Reserved.
*/

#include "mempool.h"
#include "qsplat_util.h"
#include <string>
#include <stdint.h>


struct QTree_Node {
	point pos;
	float r;
	vec norm;
	float normcone;
	union {
		QTree_Node * child[4];
		struct {
			uint32_t refcount;
			uint32_t remap;
			short col_tmp[3];
		} m;
	};
	color col;

	static PoolAlloc memPool;
	void *operator new(size_t n) { return memPool.alloc(n); }
	void operator delete(void *p, size_t n) { memPool.free(p,n); }
};


class QTree {
private:
	QTree_Node *root, *leaves;
	uint32_t numleaves;
	QTree_Node **leafptr;
	bool havecolor;
	void InitLeaves();
	QTree_Node *BuildLeaves(uint32_t begin, uint32_t end);
	QTree_Node *CombineNodes(QTree_Node *n1, QTree_Node *n2);
	QTree_Node *CombineNodes(QTree_Node *n1, QTree_Node *n2, QTree_Node *n3);
	QTree_Node *CombineNodes(QTree_Node *n1, QTree_Node *n2, QTree_Node *n3, QTree_Node *n4);
	QTree_Node *CombineLeaves(uint32_t begin, uint32_t end);
	uint32_t Partition(uint32_t begin, uint32_t end);
	uint32_t Num_Children(QTree_Node *n);
	bool Has_Grandchildren(QTree_Node *n);
	uint32_t Nodesize(QTree_Node *n);
	uint32_t Treesize(QTree_Node *n = NULL);

public:
	QTree(uint32_t _numleaves, QTree_Node *_leaves, bool _havecolor) :
		numleaves(_numleaves), leaves(_leaves), havecolor(_havecolor)
	{
		InitLeaves();
	}
	void BuildTree();
	void Write(const char *qsfile, const std::string &comments);
};

#endif
