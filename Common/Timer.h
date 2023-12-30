#pragma once

#include <Common/Types.h>

namespace common
{
	class Timer
	{
	public:

		template<typename T> inline T GetElapsedNanoSeconds() const { return (T)mElapsedNanoSeconds; }
		template<typename T> inline T GetElapsedMicroSeconds() const { return (T)mElapsedMicroSeconds; }
		template<typename T> inline T GetElapsedMilliSeconds() const { return (T)mElapsedMilliSeconds; }

	public:

		Timer();
		virtual ~Timer();

	public:

		void BeginMeasure();
		void EndMeasure();

	private:

		R64 mFrequency = 0.0;

		R64 mStartMicroSeconds = 0.0;
		R64 mEndMicroSeconds = 0.0;

		R64 mElapsedNanoSeconds = 0.0;
		R64 mElapsedMicroSeconds = 0.0;
		R64 mElapsedMilliSeconds = 0.0;
	};
}