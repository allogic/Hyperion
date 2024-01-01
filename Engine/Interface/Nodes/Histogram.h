#pragma once

#include <climits>

#include <Engine/Interface/Node.h>

namespace hyperion
{
	class Histogram : public Node
	{
	public:

		inline auto GetValueName() const { return mValueName; }
		inline auto GetTimeSteps() const { return mTimeSteps; }
		inline auto GetInterval() const { return mInterval; }
		inline auto GetScale() const { return mScale; }

	public:

		inline void SetValueName(std::string const& Value) { mValueName = Value; }
		inline void SetTimeSteps(U32 Value) { mTimeSteps = Value; mTimeLine.resize(mTimeSteps); }
		inline void SetInterval(U32 Value) { mInterval = Value; }
		inline void SetScale(R32 Value) { mScale = Value; }

	public:

		Histogram(Node* Parent);
		virtual ~Histogram();

	public:

		virtual void Update() override;
		virtual void Render() override;

	private:

		std::string mValueName = "";

		U32 mTimeSteps = 0;
		U32 mInterval = 0;

		R32 mScale = 1.0F;

		std::vector<R32> mTimeLine = {};

	private:

		R32 mPrevTime = 0.0F;

		R32 mMinValue = 0.0F;
		R32 mMaxValue = 0.0F;
		R32 mAvgValue = 0.0F;
	};
}