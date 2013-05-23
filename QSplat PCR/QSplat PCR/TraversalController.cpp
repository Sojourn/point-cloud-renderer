#include "TraversalController.h"

#include <cassert>

class UpdateListener : public CameraListener
{
public:
	UpdateListener(TraversalController &controller) :
		_controller(controller)
	{
	}

	virtual void update(Camera *camera) override
	{
		_controller.update(camera->windowSize(), camera->getView(), camera->getProjection());
	}

private:
	TraversalController &_controller;
};

TraversalController::TraversalController(
	std::unique_ptr<QSplatModel> model,
	std::shared_ptr<IBuffer> buffer,
	std::shared_ptr<Profiler> profiler) :
	_traverser(std::move(model)),
	_buffer(buffer),
	_profiler(profiler),
	_exitFlag(false),
	_firstRefine(std::chrono::milliseconds(50)),
	_subsequentRefine(std::chrono::milliseconds(50))
{
}

TraversalController::~TraversalController(void)
{
}

void TraversalController::start()
{
	assert(!_thread);
	
	_thread = std::unique_ptr<std::thread>(new std::thread(&TraversalController::run, this));
	_listener = std::make_shared<UpdateListener>(*this);
	Camera::inst().addListener(_listener);
}

void TraversalController::end()
{
	assert(_thread);
	
	{
		std::lock_guard<std::mutex> lock(_mutex);
		Camera::inst().removeListener(_listener);
		_listener.reset();
		
		_exitFlag.store(true);
		_cond.notify_all();
	}
	_thread->join();
}

void TraversalController::update(glm::ivec2 windowSize, glm::mat4 view, glm::mat4 proj)
{
	std::lock_guard<std::mutex> lock(_mutex);
	
	_cameraState.windowSize = windowSize;
	_cameraState.view = view;
	_cameraState.projection = proj;

	_updateFlag.store(true);
	_cond.notify_all();
}

void TraversalController::run()
{
	CameraState_t cameraState;
	bool done = true;
	bool resume = false;

	while(!_exitFlag.load())
	{
		if(done)
		{
			std::unique_lock<std::mutex> lock(_mutex);
			{
				while(!_updateFlag.load() && !_exitFlag.load())
				{
					_cond.wait(lock);
				}
			}
		}

		if(_exitFlag.load())
		{
			break;
		}
		else if(_updateFlag.load())
		{
			_updateFlag.store(false);
			cameraState = _cameraState;
			done = false;
			resume = false;
		}
		else
		{
			done = refine(resume, cameraState);
			resume = true;
		}
	}
}

bool TraversalController::refine(bool resume, const CameraState_t &camera)
{
	bool done;
	_buffer->Lock(IBuffer::Traverser);
	{
		std::chrono::milliseconds duration = resume ? _firstRefine : _subsequentRefine;
		std::vector<QSplat> *output = &_buffer->Array(IBuffer::Traverser);
		done = _traverser.traverseFor(duration, camera, resume, output);
	}
	_buffer->Unlock(IBuffer::Traverser);

	_profiler->SetCurrentDepth(_traverser.depth());
	_buffer->Swap();
	return done;
}

void TraversalController::setRefinementDurations(std::chrono::milliseconds first, std::chrono::milliseconds subsequent)
{
	_firstRefine = first;
	_subsequentRefine = subsequent;
}