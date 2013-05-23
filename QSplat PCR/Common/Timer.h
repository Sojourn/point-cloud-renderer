#ifndef TIMER_H
#define TIMER_H

#include <cstdint>
#include <chrono>
#include <iostream>

#define TIME_FUNCTION() Timer(__FUNCTION__) __function_timer

class Timer
{
public:
	Timer(const char *name) :
		_name(name)
	{
		_start = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		std::chrono::high_resolution_clock::time_point end = 
			std::chrono::high_resolution_clock::now();

		std::cout 
			<< _name
			<< ", "
			<< std::chrono::duration_cast<std::chrono::microseconds>(end - _start).count()
			<< " us"
			<< std::endl;
	}

private:
	const char *_name;
	std::chrono::high_resolution_clock::time_point _start;
};

#endif // TIMER_H