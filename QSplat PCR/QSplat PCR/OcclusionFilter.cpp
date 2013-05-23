#include "OcclusionFilter.h"
#include "QSplatModel.h"
#include <cassert>
#include <iostream>

OcclusionFilter::OcclusionFilter(size_t scale) :
	_scale(scale)
{
	assert(scale > 0);
}

OcclusionFilter::~OcclusionFilter()
{
}

void OcclusionFilter::reposition(Sphere_t worldSphere, const CameraState_t &camera)
{
	glm::mat4 model = glm::scale(glm::mat4(), glm::vec3(1 / worldSphere.radius));
	model = glm::translate(model, -worldSphere.center);
	_mvp = camera.projection * camera.view * model;;

	if(_viewport != camera.windowSize)
	{
		_viewport = camera.windowSize;
		assert(_viewport.x > 0);
		assert(_viewport.y > 0);

		size_t width = _viewport[0];
		size_t height = _viewport[1];
		width /= _scale;
		height /= _scale;
		
		_depthBuffer.resize(width, height);
	}

	clear();
}

void OcclusionFilter::clear()
{
	for(size_t y = 0; y < _depthBuffer.height(); y++)
	{
		for(size_t x = 0; x < _depthBuffer.width(); x++)
		{
			_depthBuffer[y][x] = 3.3e33f;
		}
	}
}

bool OcclusionFilter::insert(const ProjectedEllipse_t &ellipse)
{
	glm::ivec2 start;
	start.x = ellipse.position.x - (ellipse.radius.x / 2.0f);
	start.y = ellipse.position.y - (ellipse.radius.y / 2.0f);
	start /= _scale;

	glm::ivec2 end;
	end.x = ellipse.position.x + (ellipse.radius.x / 2.0f);
	end.y = ellipse.position.y + (ellipse.radius.y / 2.0f);
	end /= _scale;

	bool result = false;
	for(int32_t y = start.y; y < end.y; y++)
	{
		for(int32_t x = start.x; x < end.x; x++)
		{
			if(x >= _depthBuffer.width() || x < 0) continue;
			if(y >= _depthBuffer.height() || y < 0) continue;

			if(_depthBuffer[y][x] >= ellipse.distance)
			{
				_depthBuffer[y][x] = ellipse.distance;
				result = true;
			}
		}
	}

	return result;
}

ProjectedEllipse_t OcclusionFilter::project(const Sphere_t &sphere, float distance) const
{
	ProjectedEllipse_t ellipse;
	ellipse.position = projectedPosition(sphere);
	ellipse.radius = projectedRadius(sphere, distance);
	ellipse.distance = distance;
	return ellipse;
}

glm::vec2 OcclusionFilter::projectedRadius(const Sphere_t &sphere, float distance) const
{
	glm::vec2 radius;
	radius.xy = glm::atan(sphere.radius / distance);
	radius.x = (radius.x * _viewport.x) / glm::radians(30.0f);
	radius.y = (radius.y * _viewport.y) / glm::radians(30.0f);
	return radius;
}

glm::vec2 OcclusionFilter::projectedPosition(const Sphere_t &sphere) const
{
	glm::vec4 modelPos;
	modelPos.xyz = sphere.center;
	modelPos.w = 1.0f;

	glm::vec4 clipPos = _mvp * modelPos;
	clipPos.x /= clipPos.w;
	clipPos.y /= clipPos.w;

	glm::vec2 screenPos = clipPos.xy;
	screenPos.x += 1.0f;
	screenPos.y += 1.0f;
	screenPos *= (_viewport / 2);

	return screenPos;
}