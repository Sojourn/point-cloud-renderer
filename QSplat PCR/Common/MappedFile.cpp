#include "MappedFile.h"
#include <sstream>
#include <cassert>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#error TODO: Posix implementation here
#endif

class MappedFile::MappedFileImpl
{
public:
	MappedFileImpl();
	~MappedFileImpl();

	bool open(const std::string &path);
	void close();

	std::uint8_t* data();
	const std::uint8_t *data() const;

	bool is_open() const;
	std::uint64_t length() const;

private:
	std::uint8_t* _data;
	std::uint64_t _length;

#ifdef _WIN32
	HANDLE _file;
	HANDLE _mapping;
#else
#error TODO: Posix implementation here
#endif
};

MappedFile::MappedFileImpl::MappedFileImpl()
{
	_data = nullptr;
	_length = 0;

#ifdef _WIN32
	_file = nullptr;
	_mapping = nullptr;
#else
#error TODO: Posix implementation here
#endif
}

MappedFile::MappedFileImpl::~MappedFileImpl()
{
#ifdef _WIN32
	if(is_open())
	{
		close();
	}
#else
#error TODO: Posix implementation here
#endif
}

bool MappedFile::MappedFileImpl::open(const std::string &path)
{
#ifdef _WIN32
	// Step 1: Open the file
	_file = CreateFileA(
		path.c_str(),
		(GENERIC_READ | GENERIC_WRITE),
		FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		NULL,
		NULL);
	if(_file == INVALID_HANDLE_VALUE)
	{
		goto bail1;
	}

	// Step 2: Get the file size
	BY_HANDLE_FILE_INFORMATION info;
	BOOL ok = GetFileInformationByHandle(_file, &info);
	if(!ok)
	{
		goto bail2;
	}

	_length |= (info.nFileSizeLow & 0xFFFFFFFF);
	_length |= (static_cast<std::uint64_t>(info.nFileSizeHigh) << 32);

	// Step 3: Memory map the file
	_mapping = CreateFileMappingA(
		_file,
		NULL,
		PAGE_READWRITE,
		0,
		0,
		NULL);
	if(_mapping == nullptr)
	{
		goto bail3;
	}

	// Step 4: Open a view of the memory mapping
	_data = reinterpret_cast<std::uint8_t*>(MapViewOfFile(
		_mapping,
		FILE_MAP_WRITE,
		0,
		0,
		0));
	if(_data == nullptr)
	{
		goto bail4;
	}

	return true;

	// Recovery block
bail4:
	CloseHandle(_mapping);
	_data = nullptr;
bail3:
	_mapping = nullptr;
bail2:
	CloseHandle(_file);
	_length = 0;
bail1:
	_file = nullptr;
	return false;

#else
#error TODO: Posix implementation here
#endif
}

void MappedFile::MappedFileImpl::close()
{
	assert(is_open());

#ifdef _WIN32
	UnmapViewOfFile(reinterpret_cast<LPCVOID>(_data));
	_data = nullptr;

	CloseHandle(_mapping);
	_mapping = nullptr;

	CloseHandle(_file);
	_file = nullptr;
	_length = 0;

#else
#error TODO: Posix implementation here
#endif
}

bool MappedFile::MappedFileImpl::is_open() const
{
#ifdef _WIN32
	return (_file != nullptr) && (_mapping != nullptr) && (_data != nullptr);
#else
#error TODO: Posix implementation here
#endif
}

std::uint8_t* MappedFile::MappedFileImpl::data()
{
	assert(is_open());
	return _data;
}

const std::uint8_t *MappedFile::MappedFileImpl::data() const
{
	assert(is_open());
	return _data;
}

std::uint64_t MappedFile::MappedFileImpl::length() const
{
	assert(is_open());
	return _length;
}




MappedFile::MappedFile() :
	_impl(new MappedFileImpl())
{
}

MappedFile::~MappedFile()
{
}

bool MappedFile::open(const std::string &path)
{
	return _impl->open(path);
}

void MappedFile::close()
{
	_impl->close();
}

std::uint8_t* MappedFile::data()
{
	return _impl->data();
}

const std::uint8_t *MappedFile::data() const
{
	return _impl->data();
}

std::uint8_t &MappedFile::operator[](size_t index)
{
	assert(index < length());
	return *(_impl->data() + index);
}

const std::uint8_t &MappedFile::operator[](size_t index) const
{
	assert(index < length());
	return *(_impl->data() + index);
}

bool MappedFile::is_open() const
{
	return _impl->is_open();
}

uint64_t MappedFile::length() const
{
	return _impl->length();
}