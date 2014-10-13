#pragma once
#include <time.h>
#include <Windows.h>

class StopWatch
{
	LARGE_INTEGER start, finish, freq;

public:
	StopWatch();
	~StopWatch();
	double GetDuration();
	void Start();
	void Stop();
};
