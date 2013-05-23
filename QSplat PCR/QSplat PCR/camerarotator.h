#ifndef CAMERAROTATOR_H
#define CAMERAROTATOR_H

#include "Camera.h"
#include "glmconfig.h"

class CameraRotator : public CameraListener
{
public:
	CameraRotator();
	virtual ~CameraRotator();

	virtual void update(Camera *camera);

private:
	std::size_t _frameCount;
};

#endif // CAMERAROTATOR_H
