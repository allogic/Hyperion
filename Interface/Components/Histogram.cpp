#include <Common/ValueDatabase.h>

#include <Engine/Math/Math.h>

#include <Interface/Components/Histogram.h>

#include <Vulkan/Renderer.h>
#include <Vulkan/Window.h>

namespace interface
{
	Histogram::Histogram(Node* Parent) : Node(Parent)
	{

	}

	Histogram::~Histogram()
	{

	}

	void Histogram::Update()
	{
		R32 time = Window::GetTime() * 1000.0F;

		if ((time - mPrevTime) >= mInterval)
		{
			mMinValue = std::numeric_limits<R32>::max();
			mMaxValue = std::numeric_limits<R32>::min();
			mAvgValue = 0.0F;

			for (U32 i = 1; i < mTimeSteps; ++i)
			{
				mTimeLine[i - 1] = mTimeLine[i];

				if (mTimeLine[i] > mMaxValue)
				{
					mMaxValue = mTimeLine[i];
				}

				if (mTimeLine[i] < mMinValue)
				{
					mMinValue = mTimeLine[i];
				}

				mAvgValue += mTimeLine[i];
			}

			mTimeLine[mTimeSteps - 1] = ValueDatabase::GetValue(mValueName);

			if (mTimeLine[mTimeSteps - 1] > mMaxValue)
			{
				mMaxValue = mTimeLine[mTimeSteps - 1];
			}

			if (mTimeLine[mTimeSteps - 1] < mMinValue)
			{
				mMinValue = mTimeLine[mTimeSteps - 1];
			}

			mAvgValue += mTimeLine[mTimeSteps - 1];
			mAvgValue /= mTimeSteps;

			ValueDatabase::SetValue(GetId() + ":min", mMinValue * mScale);
			ValueDatabase::SetValue(GetId() + ":max", mMaxValue * mScale);
			ValueDatabase::SetValue(GetId() + ":avg", mAvgValue * mScale);

			mPrevTime = time;
		}

		Node::Update();
	}

	void Histogram::Render()
	{
		R32 stepWidth = GetWidth() / mTimeSteps;
		R32 stepHeight = GetHeight() / mMaxValue;

		for (U32 i = 0; i < mTimeSteps; ++i)
		{
			R32V3 timePosition = { GetPositionX() + i * stepWidth, GetPositionY(), GetPositionZ()};
			R32V2 timeSize = { stepWidth, stepHeight * mTimeLine[i] };

			R32 time = mTimeLine[i] / mMaxValue;
			U32 red = (U32)Math::Lerp(0.0F, 255.0F, time);
			U32 blue = (U32)Math::Lerp(255.0F, 0.0F, time);

			U32 color = (red << 24) | (blue << 8) | 255;

			Renderer::DrawInterfaceRect(timePosition, timeSize, color);
		}

		Node::Render();
	}
}