#include <Engine/Memory/FixedSizeAccessor.h>

namespace hyperion
{
	FixedSizeAccessor::FixedSizeAccessor(void* Pointer, U32 Index)
	{
		mPointer = Pointer;
		mIndex = Index;
	}

	FixedSizeAccessor::~FixedSizeAccessor()
	{

	}
}