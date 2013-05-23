#include "QSplatModel.h"
#include "Traverser.h"
#include "IBuffer.h"
#include "Util.h"

#include "Timer.h"

// These three trash std::min, std::max, and std::sqrtf with #define(s)
#include "qsplat_colorquant.h"
#include "qsplat_normquant.h"
#include "qsplat_spherequant.h"

#include <string>
#include <algorithm>

// A collection of QSplat offsets.
namespace Offset
{
	namespace Fragment
	{
		// Offset of the magic number in a fragment
		static const size_t magic_number = 0;

		// Offset of the version in a fragment
		static const size_t version = 6;

		// Offset of the fragment length in a fragment
		static const size_t fragment_length = 8;

		// Offset of comment(s) in a fragment
		static const size_t comment = 19;

		// Offset of the number of leaves in a fragment
		static const size_t leafCount = 12;
	
		// Offset of the top-level x coordinate in a fragment
		static const size_t x = 20;

		// Offset of the top-level y coordinate in a fragment
		static const size_t y = 24;

		// Offset of the top-level z coordinate in a fragment
		static const size_t z = 28;

		// Offset of the top-level radius in a fragment
		static const size_t r = 32;

		// Offset of the root node count in a fragment
		static const size_t rootNodeCount = 36;

		// Offset of the root encoded node in a fragment
		static const size_t root = 40;
	}

	namespace Node
	{
		// Offset of the normal in an encoded node
		static const size_t normal = 2;

		// Offset of the normal cone in an encoded node
		static const size_t normal_cone = 2;

		// Offset of the color in an encoded node
		static const size_t color = 4;
	}
}

// A collection of QSplat constants.
namespace Constants
{
	namespace Fragment
	{
		static const char magic_number[] = {'Q', 'S', 'p', 'l', 'a', 't'};
		static const size_t magic_number_length = 6;
	
		static const char version[] = {'1', '1'};
		static const size_t version_length = 2;
	
		static const uint8_t comment_flag = 2;
	}
}

QSplatModel::QSplatModel() :
	_fixer(EndianFixer::BigEndian, EndianFixer::systemEndianness())
{
	static bool initialized = false;
	if(!initialized)
	{
		initialized = true;
		QSplat_ColorQuant::Init();
		QSplat_NormQuant::Init();
		QSplat_SphereQuant::Init();
	}
}

QSplatModel::~QSplatModel(void)
{
	unload();
}

bool QSplatModel::load(const std::string &path)
{
	unload();

	if(!_file.open(path))
	{
		return false;
	}

	if(!readHeader())
	{
		_file.close();
		return false;
	}

	std::for_each(std::begin(_fragments), std::end(_fragments), [&](const Fragment_t &frag)
	{
		_roots.push_back(DecodedNodeGroup_t());
		decodeRoots(&frag, &_roots.back());
	});

	return true;
}

void QSplatModel::unload()
{
	if(loaded())
	{
		_file.close();
	}

	_fragments.clear();
	_roots.clear();
	_root = Sphere_t();
	_leaves = 0;
}

bool QSplatModel::loaded() const
{
	return _file.is_open();
}

Sphere_t QSplatModel::worldSphere() const
{
	return _root;
}

const std::vector<Fragment_t> &QSplatModel::fragments() const
{
	return _fragments;
}

const std::vector<DecodedNodeGroup_t> &QSplatModel::roots() const
{
	return _roots;
}

size_t QSplatModel::leaves() const
{
	return _leaves;
}

void QSplatModel::decodeChildren(const DecodedNode_t *node, DecodedNodeGroup_t *result, DecodeFlags_t flags) const
{
	assert(node);
	assert(result);

	result->parent = *node;
	result->siblingCount = node->childCount;
	
	// Get the offset of the first grandchild
	GenericEncodedNodeGroup_u grandchildren;
	if(node->grandchildren)
	{
		const uint8_t *offsetPtr = node->children.genericGroup;
		assert(offsetPtr >= _file.data() && offsetPtr < (_file.data() + _file.length()));

		result->childOffset = read<uint32_t>(offsetPtr);
		grandchildren.genericGroup = node->children.genericGroup + result->childOffset;
	}
	else
	{
		result->childOffset = 0;
		grandchildren.genericGroup = nullptr;
	}

	// Decode each of the children
	for(size_t i = 0; i < node->childCount; i++)
	{
		const EncodedNode_t *child = getChild(node, i);

		// Decode the tree structure
		DecodedNode_t *decodedChild = &result->siblings[i];
		decodedChild->childCount = childCount(child);
		decodedChild->grandchildren = hasGrandchildren(child);
		decodedChild->children = grandchildren;
		if(node->grandchildren)
		{
			grandchildren.genericGroup += (sizeof(EncodedNode_t) * decodedChild->childCount);
			grandchildren.genericGroup += (decodedChild->grandchildren) ? sizeof(uint32_t) : 0;
		}

		// Decode child's quantized fields (center, normal, color)
		decodeChild(child, node, decodedChild, flags);
	}
}

void QSplatModel::decodeRoots(const Fragment_t *fragment, DecodedNodeGroup_t *result) const
{
	assert(fragment);
	assert(result);

	// Make a fake parent node from the fragment
	DecodedNode_t rootParent;
	rootParent.children.genericGroup = fragment->ptr + Offset::Fragment::root;
	rootParent.childCount = fragment->rootNodeCount;
	rootParent.grandchildren = (fragment->rootNodeCount < fragment->leaves);
	rootParent.sphere = fragment->rootSphere;

	decodeChildren(&rootParent, result);
}

bool QSplatModel::readHeader()
{
	using namespace std;

	// Max/min global center position
	glm::vec3 maxCenter(-3.3e33f, -3.3e33f, -3.3e33f);
	glm::vec3 minCenter(3.3e33f, 3.3e33f, 3.3e33f);

	// Base pointer to the mapped file
	const uint8_t *data = _file.data();
	
	// The base address of the current or next fragment
	const uint8_t *fragment_ptr = data;

	// There can be multiple 'fragments' in a single file
	// which are concatenated, each with it's own header.
	while(fragment_ptr < (data + _file.length()))
	{
		Fragment_t fragment;
		fragment.ptr = fragment_ptr;

		// Check magic number
		if(!std::equal(
			fragment.ptr + Offset::Fragment::magic_number,
			fragment.ptr + Offset::Fragment::magic_number + Constants::Fragment::magic_number_length,
			Constants::Fragment::magic_number))
		{
			return false;
		}

		// Check file version
		if(!std::equal(
			fragment.ptr + Offset::Fragment::version,
			fragment.ptr + Offset::Fragment::version + Constants::Fragment::version_length,
			Constants::Fragment::version))
		{
			return false;
		}

		// Read fragment length
		size_t fragment_length = read<uint32_t>(fragment.ptr + Offset::Fragment::fragment_length);
		if((fragment.ptr + fragment_length) > (data + _file.length()))
		{
			return false;
		}
		else
		{
			fragment_ptr += fragment_length;
		}

		// Skip fragments containing comments
		if(read<uint8_t>(fragment.ptr + Offset::Fragment::comment) & Constants::Fragment::comment_flag)
		{
			continue;
		}

		// Add this fragment's leaves to the total count
		fragment.leaves = read<uint32_t>(fragment.ptr + Offset::Fragment::leafCount);
		_leaves += fragment.leaves;

		// Read top-level sphere position and radius
		fragment.rootSphere.center.x = read<float>(fragment.ptr + Offset::Fragment::x);
		fragment.rootSphere.center.y = read<float>(fragment.ptr + Offset::Fragment::y);
		fragment.rootSphere.center.z = read<float>(fragment.ptr + Offset::Fragment::z);
		fragment.rootSphere.radius = read<float>(fragment.ptr + Offset::Fragment::r);
		
		// Read the number of root nodes
		fragment.rootNodeCount = read<uint32_t>(fragment.ptr + Offset::Fragment::rootNodeCount);

		// Compute a new min/max world sphere position
		minCenter.x = min(minCenter.x, fragment.rootSphere.center.x - fragment.rootSphere.radius);
		minCenter.y = min(minCenter.y, fragment.rootSphere.center.y - fragment.rootSphere.radius);
		minCenter.z = min(minCenter.z, fragment.rootSphere.center.z - fragment.rootSphere.radius);
		maxCenter.x = max(maxCenter.x, fragment.rootSphere.center.x + fragment.rootSphere.radius);
		maxCenter.y = max(maxCenter.y, fragment.rootSphere.center.y + fragment.rootSphere.radius);
		maxCenter.z = max(maxCenter.z, fragment.rootSphere.center.z + fragment.rootSphere.radius);

		// Add to the 'list' of fragments
		_fragments.push_back(fragment);
	}

	// Compute world sphere
	_root.center.x = 0.5f * (minCenter.x + maxCenter.x);
	_root.center.y = 0.5f * (minCenter.y + maxCenter.y);
	_root.center.z = 0.5f * (minCenter.z + maxCenter.z);
	_root.radius = 0.5f * sqrtf(
		((maxCenter.x - minCenter.x) * (maxCenter.x - minCenter.x)) +
		((maxCenter.y - minCenter.y) * (maxCenter.y - minCenter.y)) +
		((maxCenter.z - minCenter.z) * (maxCenter.z - minCenter.z)));

	return true;
}

const EncodedNode_t *QSplatModel::getChild(const DecodedNode_t *node, size_t index) const
{
	if(node->grandchildren)
	{
		const EncodedNodeGroup_t *branchGroup = node->children.branchGroup;
		return &branchGroup->siblings[index];
	}
	else
	{
		const EncodedLeafNodeGroup_t *leafGroup = node->children.leafGroup;
		return &leafGroup->siblings[index];
	}
}

uint8_t QSplatModel::childCount(const EncodedNode_t *node) const
{
	uint8_t children = reinterpret_cast<const uint8_t*>(node)[1] & 3;
	return (children > 0) ? (children + 1) : children;
}
	
bool QSplatModel::hasGrandchildren(const EncodedNode_t *node) const
{
	bool result = (reinterpret_cast<const uint8_t*>(node)[1] & 4) > 0;
	return result;
}

void QSplatModel::decodeChild(const EncodedNode_t *child, const DecodedNode_t *parent, DecodedNode_t *result, DecodeFlags_t flags) const
{
	const uint8_t *childPtr = reinterpret_cast<const uint8_t*>(child);

	QSplat_SphereQuant::lookup(
		childPtr,
		parent->sphere.center.x,
		parent->sphere.center.y,
		parent->sphere.center.z,
		parent->sphere.radius,
		result->sphere.center.x,
		result->sphere.center.y,
		result->sphere.center.z,
		result->sphere.radius);

	if(flags & DECODE_NORMAL)
	{
		result->normal = QSplat_NormQuant::lookup(childPtr + Offset::Node::normal);
	}
	if(flags & DECODE_NORMAL_CONE)
	{
		result->normal_cone = QSplat_NormQuant::lookup_cone(childPtr + Offset::Node::normal_cone);
	}
	if(flags & DECODE_COLOR)
	{
		result->color = QSplat_ColorQuant::lookup(childPtr + Offset::Node::color);
	}
}