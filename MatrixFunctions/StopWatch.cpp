#include "stdafx.h"
#include "StopWatch.h"


StopWatch::StopWatch()
{
}


StopWatch::~StopWatch()
{
}

double StopWatch::GetDuration()
{
	return ((finish.QuadPart - start.QuadPart) / static_cast<double>(freq.QuadPart));
}

void StopWatch::Start()
{
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);
}

void StopWatch::Stop()
{
	QueryPerformanceCounter(&finish);
}