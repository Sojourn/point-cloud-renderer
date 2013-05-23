#ifndef UTIL_H_
#define UTIL_H_

#include <cstdint>

//// Endianess conversion functions.
//class Endian
//{
//public:
//
//	// Returns true if the system is little-endian
//	inline static bool isBE()
//	{
//		union
//		{
//			char result[2];
//			uint16_t test;
//		} magic_u;
//
//		magic_u.test = 0x0102;
//		return magic_u.result[1] == 2;
//	}
//
//	// Returns true if the system is big-endian
//	inline static bool isLE()
//	{
//		return !isBE();
//	}
//
//	// Swap the endianess of an 8-bit value
//	inline static uint8_t swap(uint8_t value)
//	{
//		return value;
//	}
//
//	// Swap the endianess of an 16-bit value
//	inline static uint16_t swap(uint16_t value)
//	{
//		char bytes[2];
//		bytes[0] = (value & 0x000000FF) >> 0 ;
//		bytes[1] = (value & 0x0000FF00) >> 8 ;
//		return((bytes[0] << 8) | (bytes[1] << 0));
//	}
//
//	// Swap the endianess of an 32-bit value
//	inline static uint32_t swap(uint32_t value)
//	{
//		char bytes[4];
//		bytes[0] = (value & 0x000000FF) >> 0 ;
//		bytes[1] = (value & 0x0000FF00) >> 8 ;
//		bytes[2] = (value & 0x00FF0000) >> 16;
//		bytes[3] = (value & 0xFF000000) >> 24 ;
//		return((bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | (bytes[3] << 0));
//	}
//
//	// Make a system endianess value big-endian
//	inline static uint8_t makeBE(uint8_t value)
//	{
//		return isBE() ? value : swap(value);
//	}
//
//	// Make a system endianess value big-endian
//	inline static uint16_t makeBE(uint16_t value)
//	{
//		return isBE() ? value : swap(value);
//	}
//
//	// Make a system endianess value big-endian
//	inline static uint32_t makeBE(uint32_t value)
//	{
//		return isBE() ? value : swap(value);
//	}
//
//	// Make a system endianess value little-endian
//	inline static uint8_t makeLE(uint8_t value)
//	{
//		return isLE() ? value : swap(value);
//	}
//
//	// Make a system endianess value little-endian
//	inline static uint16_t makeLE(uint16_t value)
//	{
//		return isLE() ? value : swap(value);
//	}
//
//	// Make a system endianess value little-endian
//	inline static uint32_t makeLE(uint32_t value)
//	{
//		return isLE() ? value : swap(value);
//	}
//};

#endif // UTIL_H_