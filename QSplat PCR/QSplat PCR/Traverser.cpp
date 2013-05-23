#include "Traverser.h"
#include "Timer.h"
#include "GL\glut.h"

Traverser::Traverser(std::unique_ptr<QSplatModel> model) :
	_model(std::move(model)),
	_result(nullptr),
	_depth(0)
{
}

uint32_t Traverser::depth() const
{
	return _depth;
}

bool Traverser::traverseFor(std::chrono::milliseconds duration, const CameraState_t &camera, bool resume, std::vector<QSplat> *result)
{
	auto start = std::chrono::high_resolution_clock::now();
	
	reinitialize(camera, resume, result);

	bool finished;
	for(;;)
	{
		if(_workingSet.empty())
		{
			finished = true;
			break;
		}

		auto now = std::chrono::high_resolution_clock::now();
		if((now - start) >= duration)
		{
			std::for_each(std::begin(_workingSet), std::end(_workingSet), [&](const TraversalNodeGroup_t &group)
			{
				writeSplat(group.nodes.parent);
			});
			finished = false;
			break;
		}
		else
		{
			TraversalNodeGroup_t node = _workingSet.top();
			_workingSet.pop();
			traverseGroup(node);
		}
	}

	return finished;
}

void Traverser::reinitialize(const CameraState_t &camera, bool resume, std::vector<QSplat> *result)
{
	_result = result;
	_result->clear();
	if(!resume)
	{
		storeCamera(camera);
		computeFrustum(camera.view, camera.projection, _frustum);
		_filter.reposition(_model->worldSphere(), camera);

		_depth = 0;
		_leaves.clear();
		_workingSet.clear();
		std::for_each(std::begin(_model->roots()), std::end(_model->roots()), [&](const DecodedNodeGroup_t &group)
		{
			TraversalNodeGroup_t traversalGroup;
			traversalGroup.tests = (VisibilityTest_t) (FrustumTest | BackfaceTest | CutoffTest);
			// traversalGroup.tests = (VisibilityTest_t) (OcclusionTest);
			traversalGroup.nodes = group;
			traversalGroup.depth = 0;
			traversalGroup.ellipse.distance = 3.3e33f;
			traversalGroup.ellipse.radius = glm::vec2(3.3e33f);

			_workingSet.push(traversalGroup);
		});
	}
	else
	{
		std::copy(std::begin(_leaves), std::end(_leaves), std::back_inserter(*_result));
		_filter.clear();
	}
}

void Traverser::traverseGroup(const TraversalNodeGroup_t &group)
{
	for(size_t i = 0; i < group.nodes.siblingCount; i++)
	{
		VisibilityTest_t tests = group.tests;
		const DecodedNode_t &node = group.nodes.siblings[i];
		
		float distance;
		if(tests & FrustumTest)
		{
			std::tuple<float, FrustumClip_t> result = frustumTest(node.sphere, _frustum);
			distance = std::get<0>(result);
			switch(std::get<1>(result))
			{
			case InsideFrustum: // Node + children definitely in frustum
				tests = (VisibilityTest_t) (tests & ~FrustumTest);
				break;

			case ClippingFrustum: // Children may be in frustum
				break;

			case OutsideFrustum: // Node + children not in frustum
				continue;
			}
		}
		else
		{
			distance = nearDistance(node.sphere, _frustum);
		}

		ProjectedEllipse_t ellipse = _filter.project(node.sphere, distance);
		if(tests & OcclusionTest)
		{
			if(!_filter.insert(ellipse))
			{
				continue;
			}
		}

		// Check if the node is smaller than a pixel
		bool radiusResult = (tests & CutoffTest) ?
			(ellipse.radius.x < 1.0f && ellipse.radius.y < 1.0f) : false;

		if(radiusResult || node.childCount == 0)
		{
			if(tests & BackfaceTest)
			{
				glm::vec3 normal(node.normal[0], node.normal[1], node.normal[2]);
				if(!frontfacing(normal))
				{
					continue;
				}
			}

			writeSplat(node);
			_leaves.push_back(_result->back());
		}
		else
		{
			TraversalNodeGroup_t childGroup;
			childGroup.ellipse = ellipse;
			childGroup.tests = tests;
			childGroup.depth = group.depth + 1;

			_depth = std::max(childGroup.depth, _depth);

			_model->decodeChildren(&node, &childGroup.nodes,
				(QSplatModel::DecodeFlags_t) (QSplatModel::DECODE_COLOR | QSplatModel::DECODE_NORMAL));
			_workingSet.push(childGroup);
		}
	}
}

void Traverser::writeSplat(const DecodedNode_t &node)
{
	// Check if the node is fully decoded
	const float *colorArray = node.color;
	const float *normalArray = node.normal;
	if(colorArray == nullptr || normalArray == nullptr)
	{
		return;
	}

	glm::vec3 normal(normalArray[0], normalArray[1], normalArray[2]);
	glm::vec3 color(colorArray[0], colorArray[1], colorArray[2]);

	QSplat splat;
	splat.normal = QSplat::pack(normal, 255);
	splat.color = QSplat::packColor(color, 255);
	splat.position = node.sphere.center;
	splat.radius = node.sphere.radius;
	_result->push_back(splat);
}

void Traverser::storeCamera(const CameraState_t &camera)
{
	glm::mat4 viewInverse = glm::inverse(camera.view);
	_cameraPosition.x = viewInverse[3][0];
	_cameraPosition.y = viewInverse[3][1];
	_cameraPosition.z = viewInverse[3][2];
}

void Traverser::computeFrustum(const glm::mat4 &view, const glm::mat4 &proj, Frustum_t &frustum) const
{
	Sphere_t worldSphere = _model->worldSphere();
	glm::mat4 model = glm::scale(glm::mat4(), glm::vec3(1 / worldSphere.radius));
	model = glm::translate(model, -worldSphere.center);
	
	glm::mat4 clip = proj * view * model;

	// Right plane
	frustum[0][0] = clip[0][3] - clip[0][0];
	frustum[0][1] = clip[1][3] - clip[1][0];
	frustum[0][2] = clip[2][3] - clip[2][0];
	frustum[0][3] = clip[3][3] - clip[3][0];

	// Left plane
	frustum[1][0] = clip[0][3] + clip[0][0];
	frustum[1][1] = clip[1][3] + clip[1][0];
	frustum[1][2] = clip[2][3] + clip[2][0];
	frustum[1][3] = clip[3][3] + clip[3][0];

	// Bottom plane
	frustum[2][0] = clip[0][3] + clip[0][1];
	frustum[2][1] = clip[1][3] + clip[1][1];
	frustum[2][2] = clip[2][3] + clip[2][1];
	frustum[2][3] = clip[3][3] + clip[3][1];

	// Top plane
	frustum[3][0] = clip[0][3] - clip[0][1];
	frustum[3][1] = clip[1][3] - clip[1][1];
	frustum[3][2] = clip[2][3] - clip[2][1];
	frustum[3][3] = clip[3][3] - clip[3][1];

	// Far plane
	frustum[4][0] = clip[0][3] - clip[0][2];
	frustum[4][1] = clip[1][3] - clip[1][2];
	frustum[4][2] = clip[2][3] - clip[2][2];
	frustum[4][3] = clip[3][3] - clip[3][2];

	// Near plane
	frustum[5][0] = clip[0][3] + clip[0][2];
	frustum[5][1] = clip[1][3] + clip[1][2];
	frustum[5][2] = clip[2][3] + clip[2][2];
	frustum[5][3] = clip[3][3] + clip[3][2];

	// Normalize the planes
	for(size_t plane = 0; plane < 6; plane++)
	{
		float x = frustum[plane][0];
		float y = frustum[plane][1];
		float z = frustum[plane][2];
		float t = glm::sqrt((x * x) + (y * y) + (z * z));

		frustum[plane][0] /= t;
		frustum[plane][1] /= t;
		frustum[plane][2] /= t;
		frustum[plane][3] /= t;
	}
}

std::tuple<float, FrustumClip_t> Traverser::frustumTest(
	const Sphere_t &sphere,
	const Frustum_t &frustum) const
{
	float distance = 0;
	size_t c = 0;

	for(size_t plane = 0; plane < 6; plane++)
	{
		distance =
			frustum[plane][0] * sphere.center.x +
			frustum[plane][1] * sphere.center.y +
			frustum[plane][2] * sphere.center.z +
			frustum[plane][3];

		if(distance <= -sphere.radius)
		{
			return std::make_tuple(0.0f, OutsideFrustum);
		}
		if(distance > sphere.radius)
		{
			c++;
		}
	}

	return std::make_tuple(distance + sphere.radius, (c == 6) ? InsideFrustum : ClippingFrustum);
}

float Traverser::nearDistance(const Sphere_t &sphere, const Frustum_t &frustum) const
{
	return
			frustum[5][0] * sphere.center.x +
			frustum[5][1] * sphere.center.y +
			frustum[5][2] * sphere.center.z +
			frustum[5][3] + sphere.radius;
}

bool Traverser::frontfacing(const glm::vec3 &normal) const
{
	return glm::dot(_cameraPosition, normal) > 0;
}