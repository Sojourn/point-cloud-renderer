#ifndef OCCLUSIONFILTER_H
#define OCCLUSIONFILTER_H

#include "Camera.h"
#include "Array2D.h"
#include "glmconfig.h"

struct Sphere_t;

struct ProjectedEllipse_t
{
	glm::vec2 position;
	glm::vec2 radius;
	float distance;
};

class OcclusionFilter
{
public:
	OcclusionFilter(size_t approximationFactor = 1);
	~OcclusionFilter();

	void reposition(Sphere_t worldSphere, const CameraState_t &camera);
	void clear();
	bool insert(const ProjectedEllipse_t &ellipse);

	ProjectedEllipse_t project(const Sphere_t &sphere, float distance) const;

private:
	glm::vec2 projectedRadius(const Sphere_t &sphere, float distance) const;
	glm::vec2 projectedPosition(const Sphere_t &sphere) const;

	const size_t _scale;
	Array2D<float> _depthBuffer;

	glm::mat4 _mvp;
	glm::ivec2 _viewport;
};

#endif // OCCLUSIONFILTER_H