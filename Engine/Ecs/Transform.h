#pragma once

#include <Engine/Forward.h>

#include <Engine/Common/Types.h>

namespace hyperion
{
	constexpr R32V3 WorldRight = { 1.0F, 0.0F, 0.0F };
	constexpr R32V3 WorldLeft = { -1.0F, 0.0F, 0.0F };
	constexpr R32V3 WorldUp = { 0.0F, 1.0F, 0.0F };
	constexpr R32V3 WorldDown = { 0.0F, -1.0F, 0.0F };
	constexpr R32V3 WorldFront = { 0.0F, 0.0F, 1.0F };
	constexpr R32V3 WorldBack = { 0.0F, 0.0F, -1.0F };

	struct Transform
	{
		R32V3 LocalPosition;
		U32 Reserved0;
		R32V3 LocalEulerAngles;
		U32 Reserved1;
		R32V3 LocalScale;
		U32 Reserved2;
		R32V3 LocalRight;
		U32 Reserved3;
		R32V3 LocalUp;
		U32 Reserved4;
		R32V3 LocalFront;
		U32 Reserved5;
		R32V3 WorldPosition;
		U32 Reserved6;
		R32Q LocalRotation;
		R32Q WorldRotation;
		U32 Allocated;
		U32 WorldSpace;
		I32 ParentIndex;
		U32 Reserved7;
	};
}