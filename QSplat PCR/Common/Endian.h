#ifndef ENDIAN_H
#define ENDIAN_H

#include <cstdint>
#include <cassert>

class EndianFixer
{
public:
	enum EndianType_e
	{
		BigEndian = 0,
		LittleEndian
	};

	explicit EndianFixer(EndianType_e src, EndianType_e dst) :
	_src(src), _dst(dst)
	{}

	template<class ValueType>
	ValueType fix(ValueType value) const
	{ return (_src == _dst) ? value : swap<ValueType>(value); }

	static EndianType_e systemEndianness()
	{
		union
		{
			char result[2];
			uint16_t test;
		} magic;

		magic.test = 0x0102;
		return (magic.result[1] == 2) ? BigEndian : LittleEndian;
	}

private:
	const EndianType_e _src;
	const EndianType_e _dst;

	template<class SrcType, class DstType>
	DstType bitwise_cast(SrcType value) const
	{
		return *reinterpret_cast<DstType *>(&value);
	}

	template<class ValueType>
	ValueType swap(ValueType value) const
	{
		switch(sizeof(ValueType))
		{
		case 1: // 8-bit
			return value;

		case 2: // 16-bit
			{
				uint16_t temp = bitwise_cast<ValueType, uint16_t>(value);
				temp = ((((temp & 0xff) << 8) |
					(temp) >> 8));
				return bitwise_cast<uint16_t, ValueType>(temp);
			}

		case 4: // 32-bit
			{
				uint32_t temp = bitwise_cast<ValueType, uint32_t>(value);
				temp = ((temp << 24) |
					((temp << 8) & 0x00ff0000) |
					((temp >> 8) & 0x0000ff00) |
					(temp >> 24));
				return bitwise_cast<uint32_t, ValueType>(temp);
			}

		default:

			// TODO: Implement swapping for this case
			assert(false);
			return value;
		}
	}
};

#endif // ENDIAN_H