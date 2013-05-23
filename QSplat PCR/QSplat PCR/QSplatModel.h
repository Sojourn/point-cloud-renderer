#ifndef QSPLATMODEL_H
#define QSPLATMODEL_H

#include <string>
#include <vector>
#include <tuple>

#include "QSplat.h"
#include "MappedFile.h"
#include "Endian.h"
#include "glmconfig.h"

// A generic sphere.
struct Sphere_t
{
	// Center of the sphere.
	glm::vec3 center;

	// Radius of the sphere.
	float radius;
};

// An rgb value with 6, 5, and 5 bits for the red, green, and blue
// color channels respectively.
struct ColorRGB16_t
{
	uint16_t red : 6;
	uint16_t green : 5;
	uint16_t blue : 5;
};
static_assert(sizeof(ColorRGB16_t) == 2, "Size of a 16-bit RGB value");

// Packed representation of a single node.
struct EncodedNode_t
{
	uint16_t sphere : 13;
	uint16_t structure : 3;
	uint16_t normal : 14;
	uint16_t normal_cone : 2;
	ColorRGB16_t color;
};
static_assert(sizeof(EncodedNode_t) == (48 / 8), "Encoded node size should be 48-bits");

// Packed representation of a group of non-leaf nodes.
struct EncodedNodeGroup_t
{
	uint32_t childOffset;
	EncodedNode_t siblings[4];
};
static_assert(sizeof(EncodedNodeGroup_t) == 4 + 4 * (48 / 8), "Encoded node group should be 224-bits");

// Packed representation of a group of leaf nodes.
struct EncodedLeafNodeGroup_t
{
	EncodedNode_t siblings[4];
};
static_assert(sizeof(EncodedLeafNodeGroup_t) == 4 * (48 / 8), "Encoded node group should be 192-bits");

// A pointer to a group of leaf, or branch nodes.
union GenericEncodedNodeGroup_u
{
	const uint8_t *genericGroup;
	const EncodedNodeGroup_t *branchGroup;
	const EncodedLeafNodeGroup_t *leafGroup;
};

// A single decoded node.
struct DecodedNode_t
{
	GenericEncodedNodeGroup_u children;

	Sphere_t sphere;
	float normal_cone;
	const float *normal;
	const float *color;

	size_t childCount;
	bool grandchildren;

	DecodedNode_t()
	{
		sphere.center = glm::vec3(0.0f, 0.0f, 0.0f);
		sphere.radius = 0.0f;
		normal_cone = 0.0f;
		children.genericGroup = nullptr;
		normal = nullptr;
		color = nullptr;
		childCount = 0;
		grandchildren = false;
	}
};

// A group of decoded nodes.
struct DecodedNodeGroup_t
{
	uint32_t childOffset;
	size_t siblingCount;

	DecodedNode_t parent;
	DecodedNode_t siblings[4];

	DecodedNodeGroup_t()
	{
		childOffset = 0;
		siblingCount = 0;
	}
};

// A top-level fragment of in a QSplat file.
struct Fragment_t
{
	// Base address of the fragment
	const uint8_t* ptr;

	// Top-level sphere in the fragment
	Sphere_t rootSphere;
	
	// Top-level root node count
	size_t rootNodeCount;

	// Number of leaf nodes in the fragment
	size_t leaves;
};

// An object for managing and using a QSplat model.
class QSplatModel
{
public:
	enum DecodeFlags_t
	{
		DECODE_MINIMAL     = 0,        // Decode only required fields
		DECODE_COLOR       = (1 << 0), // Decode the color vector
		DECODE_NORMAL      = (1 << 1), // Decode the normal vector
		DECODE_NORMAL_CONE = (1 << 2), // Decode the normal cone
		DECODE_ALL         = (DECODE_COLOR | DECODE_NORMAL | DECODE_NORMAL_CONE)
	};

	// Ctor.
	QSplatModel();
	
	// Dtor.
	~QSplatModel(void);

	// Load a model.
	bool load(const std::string &path);

	// Unload a model.
	void unload();

	// Returns true if a model is loaded.
	bool loaded() const;

	// Return the world sphere, containing all of the fragments.
	Sphere_t worldSphere() const;

	// Return a 'list' of all the model's fragments.
	const std::vector<Fragment_t> &fragments() const;
	
	// Return a vector of the model's root nodes.
	const std::vector<DecodedNodeGroup_t> &roots() const;

	// Returns the number of leaf nodes in the model.
	size_t leaves() const;

	// Decode children of a node.
	void decodeChildren(
		const DecodedNode_t *node,
		DecodedNodeGroup_t *result,
		DecodeFlags_t flags = DECODE_ALL) const;

private:

	// Read and fix the endiannass of a value.
	template<class ValueType>
	ValueType read(const uint8_t *location) const
	{
		return _fixer.fix(*reinterpret_cast<const ValueType *>(location));
	}

	// Read the fragment header(s).
	bool readHeader();

	// Decode the root nodes of a fragment.
	void decodeRoots(const Fragment_t *fragment, DecodedNodeGroup_t *result) const;

	// Return the child of node at the index.
	const EncodedNode_t *getChild(const DecodedNode_t *node, size_t index) const;

	// Return the number of children an encoded node has.
	uint8_t childCount(const EncodedNode_t *node) const;
	
	// Return true if the encoded node has grandchildren.
	bool hasGrandchildren(const EncodedNode_t *node) const;

	// Decode the fields of a node (sphere, normal, color).
	void decodeChild(const EncodedNode_t *child, const DecodedNode_t *parent, DecodedNode_t *result, DecodeFlags_t flags) const;

	// Memory mapped file containing the model
	MappedFile _file;
	
	// Vector of fragments in the file
	std::vector<Fragment_t> _fragments;

	// Vector of root nodes in the file
	std::vector<DecodedNodeGroup_t> _roots;

	// Global sphere containing all of the fragments
	Sphere_t _root;

	// Number of leaves in all the fragments
	size_t _leaves;

	// Endiannass fixer
	EndianFixer _fixer;
};

#endif // QSPLATMODEL_H