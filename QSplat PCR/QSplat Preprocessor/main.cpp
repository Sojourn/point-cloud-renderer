#include <stdio.h>
#include "qsplat_make_from_pts.h"
#include "qsplat_make_qtree_v11.h"

using namespace std;



int main(int argc, char *argv[])
{
	const char *infilename = argv[1];
	const char *outfilename = argv[2];

	int numleaves;
	bool havecolor = true;
	std::string comments;
	QTree_Node *leaves;

	/*char buffer[1024];
	GetCurrentDirectoryA(
	  1024,
	  buffer
	);*/

	//Read file, get leaves
	if(!read_pts(infilename, numleaves, leaves, havecolor, comments)){
		fprintf(stderr, "Couldn'tread input file %s\n", infilename);
		exit(1);
	}

	// Initialize tree
	QTree qt(numleaves, leaves, havecolor);

	//Build tree
	qt.BuildTree();

	//Write Tree
	qt.Write(outfilename, comments);

	return 0;
	
}
