#ifndef QTYPES_H
#define QTYPES_H

#include <cstdint>
#include <memory>

// JR: Why?
//typedef uint16_t uint16;
//typedef uint32_t uint32;
//typedef uint64_t uint64;
//typedef int64_t int64;
//typedef int32_t int32;
//typedef int16_t int16;

static_assert(sizeof(uint8_t) == 1, "");
static_assert(sizeof(int8_t) == 1, "");
static_assert(sizeof(uint16_t) == 2, "");
static_assert(sizeof(int16_t) == 2, "");
static_assert(sizeof(uint32_t) == 4, "");
static_assert(sizeof(int32_t) == 4, "");
static_assert(sizeof(uint64_t) == 8, "");
static_assert(sizeof(int64_t) == 8, "");

class IBuffer;
class ISplatter;
class Profiler;
class IRenderer;

typedef std::shared_ptr<IBuffer> pBuffer;
typedef std::shared_ptr<ISplatter> pSplatter;
typedef std::shared_ptr<Profiler> pProfiler;
typedef std::shared_ptr<IRenderer> pRenderer;

#endif // QTYPES_H