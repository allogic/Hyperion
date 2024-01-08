#pragma once

#include <string>
#include <set>
#include <cassert>

#include <Engine/Forward.h>

#include <Engine/Common/Types.h>

namespace hyperion
{
	class FixedSizeAllocator
	{
	public:

		FixedSizeAllocator(U8* Memory, U32 ElementCount, U64 ElementSize);
		virtual ~FixedSizeAllocator();

	public:

		FixedSizeAccessor* Allocate();
		void Free(FixedSizeAccessor* Accessor);

	private:

		U32 Defragment();

	private:

		void Expand();
		void Contract();

	public:

		void Dump();

	private:

		U8* mMemory = 0;

		U32 mElementCount = 0;
		U64 mElementSize = 0;

		U32 mFreeElementIndex = 0;
	};
}