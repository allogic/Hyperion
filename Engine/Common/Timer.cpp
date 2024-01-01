#include <Engine/Common/Timer.h>

#include <windows.h>

namespace hyperion
{
	Timer::Timer()
	{
		LARGE_INTEGER frequency = {};

		QueryPerformanceFrequency(&frequency);

		mFrequency = (R64)frequency.QuadPart;
	}

	Timer::~Timer()
	{

	}

	void Timer::BeginMeasure()
	{
		LARGE_INTEGER startMicroSeconds = {};

		QueryPerformanceCounter(&startMicroSeconds);

		mStartMicroSeconds = (R64)startMicroSeconds.QuadPart;
	}

	void Timer::EndMeasure()
	{
		LARGE_INTEGER endMicroSeconds = {};

		QueryPerformanceCounter(&endMicroSeconds);

		mEndMicroSeconds = (R64)endMicroSeconds.QuadPart;

		mElapsedMicroSeconds = (mEndMicroSeconds - mStartMicroSeconds) / mFrequency;
		mElapsedNanoSeconds = mElapsedMicroSeconds / 1000.0;
		mElapsedMilliSeconds = mElapsedMicroSeconds * 1000.0;
	}
}