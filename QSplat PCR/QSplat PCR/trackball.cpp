//#include "trackball.h"
//
//static const float radius = 1.0f;
//
//Trackball::Trackball() :
//	_dragging(false)
//{
//}
//
//Trackball::~Trackball()
//{
//}
//
//void Trackball::update(Camera *camera)
//{
//	if(_dragging)
//	{
//		glm::vec3 cross = glm::cross(_dragBegin, _dragCurrent);
//
//		if(glm::length(cross) > glm::epsilon<float>())
//		{
//			// Non-zero rotation
//			glm::detail::tquat<float> quat;
//			quat.x = cross.x;
//			quat.y = cross.y;
//			quat.z = cross.z;
//			quat.w = glm::dot(_dragBegin, _dragCurrent);
//			glm::mat4 rot = glm::toMat4(quat);
//
//			 camera->transform(glm::inverse(rot));
//			//camera->transform(rot);
//
//			_dragBegin = _dragCurrent;
//		}
//		else
//		{
//			// Hasn't rotated at all
//		}
//	}
//}
//
//bool Trackball::eventFilter(QObject *object, QEvent *event)
//{
//	auto mappedPosition = [this](QEvent *event) -> glm::vec3
//	{
//		QMouseEvent *mouseEvent = reinterpret_cast<QMouseEvent *>(event);
//		glm::vec2 mousePosition(mouseEvent->pos().x(), mouseEvent->pos().y());
//		return mapOnSphere(mousePosition, camera);
//	};
//
//	switch(event->type())
//	{
//	case QEvent::MouseButtonPress:
//	{
//		// TODO: Check which mouse button
//		_dragBegin = mappedPosition(event);
//		_dragging = true;
//		return true;
//	}
//
//	case QEvent::MouseButtonRelease:
//	{
//		// TODO: Check which mouse button
//		_dragging = false;
//		return true;
//	}
//
//	case QEvent::MouseMove:
//	{
//		if(_dragging)
//		{
//			_dragCurrent = mappedPosition(event);
//		}
//		return true;
//	}
//
//	default:
//		return QObject::eventFilter(object, event);
//	}
//}
//
//glm::vec3 Trackball::mapOnSphere(const glm::vec2 &point, Camera *camera) const
//{
//	// Map the point from [0..width][0..height] to [-1..1][-1..1]
//	glm::vec3 loc;
//	loc.x = ((point.x * 2.0f) / camera->width()) - 1.0f;
//	loc.y = 1.0f - ((point.y * 2.0f) / camera->height());
//	loc.z = 0.0f;
//
//	float distance = glm::distance(loc, glm::vec3(0.0f));
//	if(distance > radius)
//	{
//		// Outside the sphere
//		float norm = 1.0f / distance;
//		return glm::vec3(loc.x * norm, loc.y * norm, 0.0f);
//	}
//	else
//	{
//		// Inside the sphere
//		float z = glm::sqrt((radius * radius) - (distance * distance));
//		return glm::vec3(loc.x, loc.y, z);
//	}
//}