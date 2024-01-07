#pragma once

#include <Engine/Ecs/Component.h>

namespace hyperion
{
	class AnimatorComponent : public Component
	{
	public:

		inline auto GetBoneTransformBuffer() const { return mBoneTransformBuffer; }

	public:

		inline void SetBoneTransformBuffer(Buffer* Value) { mBoneTransformBuffer = Value; }
		inline void SetSharedSkeleton(Skeleton* Value) { mSharedSkeleton = Value; }

	public:

		AnimatorComponent();
		virtual ~AnimatorComponent();

	public:

		void Play(Animation* Animation);
		void Update();

	private:

		Buffer* mBoneTransformBuffer = 0;

		R32 mTime = 0.0F;

		Animation* mAnimation = 0;
		Skeleton* mSharedSkeleton = 0;
	};
}