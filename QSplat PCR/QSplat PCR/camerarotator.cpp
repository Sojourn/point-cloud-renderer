#include "camerarotator.h"

CameraRotator::CameraRotator() :
	_frameCount(0)
{
}

CameraRotator::~CameraRotator()
{
}

void CameraRotator::update(Camera *camera)
{
	glm::mat4 matrix;
	matrix = glm::rotate(camera->getView(), 100.0f * _frameCount / 16.0f, glm::vec3(0, 1, 0));
	camera->setView(matrix);
	_frameCount++;
}
