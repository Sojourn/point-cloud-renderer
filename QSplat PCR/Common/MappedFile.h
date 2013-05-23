#ifndef MAPPEDFILE_H_
#define MAPPEDFILE_H_

#include <memory>
#include <string>
#include <cstddef>
#include <cstdint>

class MappedFile
{
public:
	MappedFile();
	~MappedFile();

	bool open(const std::string &path);
	void close();

	std::uint8_t* data();
	const std::uint8_t *data() const;

	std::uint8_t &operator[](size_t index);
	const std::uint8_t &operator[](size_t index) const;

	bool is_open() const;
	std::uint64_t length() const;

private:
	class MappedFileImpl;
	std::unique_ptr<MappedFileImpl> _impl;
};

#endif // MAPPEDFILE_H_

