#ifndef TRAVERSALCONTROLLER_H
#define TRAVERSALCONTROLLER_H

#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "QSplatModel.h"
#include "Traverser.h"
#include "IBuffer.h"
#include "Profiler.h"
#include "Camera.h"
#include "glmconfig.h"

class TraversalController
{
	friend class UpdateListener;
public:
	TraversalController(
		std::unique_ptr<QSplatModel> model,
		std::shared_ptr<IBuffer> buffer,
		std::shared_ptr<Profiler> profiler);
	
	~TraversalController(void);

	void start();
	void end();

	void setRefinementDurations(
		std::chrono::milliseconds first,
		std::chrono::milliseconds subsequent);

private:
	void run();

	void update(
		glm::ivec2 windowSize,
		glm::mat4 view,
		glm::mat4 proj);

	bool refine(
		bool resume,
		const CameraState_t &camera);

	Traverser _traverser;
	CameraState_t _cameraState;

	std::shared_ptr<IBuffer> _buffer;
	std::shared_ptr<Profiler> _profiler;
	std::shared_ptr<CameraListener> _listener;

	std::unique_ptr<std::thread> _thread;
	std::mutex _mutex;
	std::condition_variable _cond;

	std::atomic<bool> _exitFlag;
	std::atomic<bool> _updateFlag;

	std::atomic<std::chrono::milliseconds> _firstRefine;
	std::atomic<std::chrono::milliseconds> _subsequentRefine;
};

#endif // TRAVERSALCONTROLLER_H