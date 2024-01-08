#pragma once

#include <Engine/Common/Types.h>

namespace hyperion
{
	class FixedSizeAccessor
	{
	public:

		template<typename T>
		inline auto GetData() const { return (T*)mPointer; }

		inline auto GetPointer() const { return mPointer; }
		inline auto GetIndex() const { return mIndex; }

	public:

		inline void SetPointer(void* Value) { mPointer = Value; }
		inline void SetIndex(U32 Value) { mIndex = Value; }

	public:

		FixedSizeAccessor(void* Pointer, U32 Index);
		virtual ~FixedSizeAccessor();

	private:

		void* mPointer = 0;

		U32 mIndex = 0;
	};
}