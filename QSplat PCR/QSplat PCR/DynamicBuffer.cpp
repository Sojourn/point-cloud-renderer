#include "DynamicBuffer.h"

DynamicBuffer::DynamicBuffer(int maxBufferSize) : 
	maxBufferSize(maxBufferSize)
{
	_traverserBuffer.reserve(maxBufferSize);
	_rendererBuffer.reserve(maxBufferSize);

	_buffers[Traverser] = &_traverserBuffer;
	_buffers[Renderer] = &_rendererBuffer;
	_version.store(0);
}

DynamicBuffer::~DynamicBuffer()
{
}

std::vector<QSplat> &DynamicBuffer::Array(Accessor_t accessor)
{
	return *_buffers[accessor];
}

const std::vector<QSplat> &DynamicBuffer::Array(Accessor_t accessor) const
{
	return *_buffers[accessor];
}

void DynamicBuffer::Swap()
{
	Lock(Traverser);
	Lock(Renderer);

	std::vector<QSplat> *temp = _buffers[Traverser];
	_buffers[Traverser] = _buffers[Renderer];
	_buffers[Renderer] = temp;
	_version++;

	Unlock(Renderer);
	Unlock(Traverser);
}

void DynamicBuffer::Lock(Accessor_t accessor)
{
	_mutexes[accessor].lock();
}

void DynamicBuffer::Unlock(Accessor_t accessor)
{
	_mutexes[accessor].unlock();
}

size_t DynamicBuffer::GetVersion() const
{
	return _version.load();
}
