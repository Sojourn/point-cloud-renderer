#ifndef BASE_CAMERA_H_
#define BASE_CAMERA_H_

#include <list>
#include <memory>
#include <algorithm>

#include "glmconfig.h"

class Camera;

/// The complete state of a camera object.
struct CameraState_t
{
	// Size of the window (width, height).
	glm::ivec2 windowSize;

	// View matrix.
	glm::mat4 view;

	// Projection matrix.
	glm::mat4 projection;
};

/// An object which modifies the camera orientation.
class CameraListener
{
public:

	// Called once per frame per camera.
	virtual void update(Camera *camera) = 0;
};

/// An object which manages projection and view matricies.
class Camera
{
public:
	static Camera &inst();

	const CameraState_t &getState() const;

	void resize(int  width, int height);
	int windowWidth() const;
	int windowHeight() const;
	const glm::ivec2 &windowSize() const;

	void addListener(std::shared_ptr<CameraListener> listener);
	void removeListener(std::shared_ptr<CameraListener> listener);

	const glm::mat4 &getProjection() const;
	void setProjection(const glm::mat4 &p);
	
	const glm::mat4 &getView() const;
	void setView(const glm::mat4 &v);

	void transform(const glm::mat4 &matrix);

private:
	Camera();
	~Camera();

	void update();

	std::list<std::shared_ptr<CameraListener> > _listeners;

	CameraState_t _state;
};

#endif /* BASE_CAMERA_H_ */