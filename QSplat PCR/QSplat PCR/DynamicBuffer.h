#ifndef DYNAMICBUFFER_H
#define DYNAMICBUFFER_H

#include <cstdint>
#include <vector>
#include <mutex>
#include <array>
#include <atomic>

#include "IBuffer.h"

// A dynamically resized double IBuffer.
class DynamicBuffer : public IBuffer
{
public:
	DynamicBuffer(int maxBufferSize);
	virtual ~DynamicBuffer();
	virtual std::vector<QSplat> &Array(Accessor_t accessor);
	virtual const std::vector<QSplat> &Array(Accessor_t accessor) const;
	virtual void Swap();
	virtual void Lock(Accessor_t accessor);
	virtual void Unlock(Accessor_t accessor);
	virtual size_t GetVersion() const;

private:
	const size_t maxBufferSize;
	std::vector<QSplat> _traverserBuffer;
	std::vector<QSplat> _rendererBuffer;
	std::array<std::vector<QSplat>*, 2> _buffers;
	std::array<std::mutex, 2> _mutexes;
	std::atomic<size_t> _version;
};

#endif // DYNAMICBUFFER_H