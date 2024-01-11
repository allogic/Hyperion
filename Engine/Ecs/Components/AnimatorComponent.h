#pragma once

#include <Engine/Ecs/Component.h>

namespace hyperion
{
	class AnimatorComponent : public Component
	{
	public:

		inline auto GetBoneBuffer() const { return mBoneBuffer; }
		inline auto GetTime() const { return mTime; }

	public:

		AnimatorComponent(ComponentArguments const& Arguments, Model* Model);
		virtual ~AnimatorComponent();

	public:

		void Play(std::string const& Name);
		void Update();

	private:

		Model* mSharedModel = 0;
		Skeleton* mSharedSkeleton = 0;

	private:

		Buffer* mBoneBuffer = 0;

	private:

		R32 mDuration = 0.0F;
		R32 mTicksPerSecond = 0.0F;

		R32 mTime = 0.0;

		bool mPlaying = false;
	};
}