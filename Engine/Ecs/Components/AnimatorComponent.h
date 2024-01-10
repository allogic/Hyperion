#pragma once

#include <Engine/Ecs/Component.h>

namespace hyperion
{
	class AnimatorComponent : public Component
	{
	public:

		inline auto GetSharedSkeleton() const { return mSharedSkeleton; }

	public:

		inline void SetSharedSkeleton(Skeleton* Value) { mSharedSkeleton = Value; }

	public:

		AnimatorComponent(ComponentArguments const& Arguments);
		virtual ~AnimatorComponent();

	public:

		void Play(Animation* Animation);
		void Update();

	private:

		Animation* mSharedAnimation = 0;
		Skeleton* mSharedSkeleton = 0;

	private:

		R32 mTime = 0.0F;

		bool mPlaying = false;
	};
}