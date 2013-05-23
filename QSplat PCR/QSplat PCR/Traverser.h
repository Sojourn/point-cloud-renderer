#ifndef TRAVERSER_H_
#define TRAVERSER_H_

#include <deque>
#include <memory>
#include <functional>
#include <algorithm>
#include <chrono>
#include <tuple>
#include <boost/heap/priority_queue.hpp>

#include "IBuffer.h"
#include "QSplatModel.h"
#include "Camera.h"
#include "OcclusionFilter.h"

typedef glm::vec4 Frustum_t[6];

enum VisibilityTest_t : uint32_t
{
	FrustumTest   = (1 << 0),
	BackfaceTest  = (1 << 1),
	OcclusionTest = (1 << 2),
	CutoffTest    = (1 << 3)
};

enum FrustumClip_t : uint32_t
{
	OutsideFrustum  = 0,
	ClippingFrustum = 1,
	InsideFrustum   = 2
};

struct TraversalNodeGroup_t
{
	DecodedNodeGroup_t nodes;
	ProjectedEllipse_t ellipse;
	VisibilityTest_t tests;
	uint32_t depth;

	inline bool operator>(const TraversalNodeGroup_t &rhs) const
	{
		float maxRadius = glm::max(ellipse.radius.x, ellipse.radius.y);
		float rhs_maxRadius = glm::max(rhs.ellipse.radius.x, rhs.ellipse.radius.y);
		return maxRadius < rhs_maxRadius;
	}
};

class Traverser
{
public:
	Traverser(const std::unique_ptr<QSplatModel> model);

	// Traverse until either the search space is exhausted or the duration is exceeded,
	// and returns true or false respectively.
	bool traverseFor(
		std::chrono::milliseconds duration,
		const CameraState_t &camera,
		bool resume,
		std::vector<QSplat> *result);
	
	uint32_t depth() const;

private:
	void reinitialize(const CameraState_t &camera, bool resume, std::vector<QSplat> *result);
	void traverseGroup(const TraversalNodeGroup_t &group);
	void writeSplat(const DecodedNode_t &node);
	void storeCamera(const CameraState_t &camera);
	void computeFrustum(const glm::mat4 &view, const glm::mat4 &proj, Frustum_t &frustum) const;
	bool frontfacing(const glm::vec3 &normal) const;
	
	// Return the distance + radius to the near plane, and the frustum test result.
	std::tuple<float, FrustumClip_t> frustumTest(
		const Sphere_t &sphere,
		const Frustum_t &frustum) const;

	// Return the distance + radius to the near plane.
	float nearDistance(const Sphere_t &sphere, const Frustum_t &frustum) const;

	const std::unique_ptr<QSplatModel> _model;
	boost::heap::priority_queue<
		TraversalNodeGroup_t,
		boost::heap::compare<std::greater<TraversalNodeGroup_t> > > _workingSet;
	OcclusionFilter _filter;

	std::vector<QSplat> _leaves;
	std::vector<QSplat> *_result;

	glm::vec3 _cameraPosition;
	Frustum_t _frustum;
	uint32_t _depth;
};

#endif // TRAVERSER_H_