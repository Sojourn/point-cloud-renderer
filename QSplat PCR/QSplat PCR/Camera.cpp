#include "Camera.h"

Camera &Camera::inst()
{
	static Camera camera;
	return camera;
}

Camera::Camera()
{
	_state.windowSize = glm::ivec2(-1, -1);
	_state.projection = glm::mat4(1.0f);
	_state.view = glm::mat4(1.0f);
}

Camera::~Camera()
{
}

const CameraState_t &Camera::getState() const
{
	return _state;
}

void Camera::resize(int width, int height)
{
	_state.windowSize = glm::ivec2(width, height);
}

int Camera::windowWidth() const
{
	return _state.windowSize.x;
}

int Camera::windowHeight() const
{
	return _state.windowSize.y;
}

const glm::ivec2 &Camera::windowSize() const
{
	return _state.windowSize;
}

void Camera::update()
{
	std::for_each(_listeners.begin(), _listeners.end(), [this](std::shared_ptr<CameraListener> &l) {
		l->update(this);
	});
}

void Camera::addListener(std::shared_ptr<CameraListener> listener)
{
	_listeners.push_back(listener);
}

void Camera::removeListener(std::shared_ptr<CameraListener> listener)
{
	_listeners.remove(listener);
}

const glm::mat4 &Camera::getProjection() const
{
	return _state.projection;
}

void Camera::setProjection(const glm::mat4 &p)
{
	if(p != _state.projection)
	{
		_state.projection = p;
		update();
	}
}

const glm::mat4 &Camera::getView() const
{
	return _state.view;
}

void Camera::setView(const glm::mat4 &v)
{
	if(v != _state.view)
	{
		_state.view = v;
		update();
	}
}

void Camera::transform(const glm::mat4 &matrix)
{
	if(matrix != glm::mat4(1.0f))
	{
		_state.view = _state.view * matrix;
		update();
	}
}