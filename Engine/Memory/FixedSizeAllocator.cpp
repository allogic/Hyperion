#include <Engine/Common/Macros.h>

#include <Engine/Memory/FixedSizeBlock.h>
#include <Engine/Memory/FixedSizeAccessor.h>
#include <Engine/Memory/FixedSizeAllocator.h>

namespace hyperion
{
	FixedSizeAllocator::FixedSizeAllocator(U8* Memory, U32 ElementCount, U64 ElementSize)
	{
		mMemory = Memory;
		mElementCount = ElementCount;
		mElementSize = ElementSize;
	}

	FixedSizeAllocator::~FixedSizeAllocator()
	{
		for (U32 i = 0; i < mElementCount; ++i)
		{
			FixedSizeBlock* blockInfo = (FixedSizeBlock*)(mMemory + (sizeof(FixedSizeBlock) + mElementSize) * i);

			if (blockInfo->Allocated)
			{
				delete blockInfo->Accessor;
			}
		}
	}

	FixedSizeAccessor* FixedSizeAllocator::Allocate()
	{
		if (mFreeElementIndex >= mElementCount)
		{
			mFreeElementIndex = Defragment();
		}

		if (mFreeElementIndex >= mElementCount)
		{
			Expand();
		}

		FixedSizeBlock* blockInfo = (FixedSizeBlock*)(mMemory + (sizeof(FixedSizeBlock) + mElementSize) * mFreeElementIndex);

		blockInfo->Allocated = 1;
		blockInfo->Accessor = new FixedSizeAccessor{ blockInfo + 1, mFreeElementIndex };

		mFreeElementIndex++;

		return blockInfo->Accessor;
	}

	void FixedSizeAllocator::Free(FixedSizeAccessor* Accessor)
	{
		if (Accessor)
		{
			FixedSizeBlock* blockInfo = ((FixedSizeBlock*)Accessor->GetPointer()) - 1;

			std::memset(blockInfo, 0, sizeof(FixedSizeBlock));

			delete Accessor;
		}
	}

	U32 FixedSizeAllocator::Defragment()
	{
		std::set<U32> freeIndices = {};

		for (U32 i = 0; i < mElementCount; ++i)
		{
			FixedSizeBlock* blockInfo = (FixedSizeBlock*)(mMemory + (sizeof(FixedSizeBlock) + mElementSize) * i);

			if (blockInfo->Allocated)
			{
				if (i > 0)
				{
					if (freeIndices.empty())
					{
						break;
					}
					else
					{
						U32 freeIndex = *freeIndices.begin();

						FixedSizeBlock* lastFreeBlockInfo = (FixedSizeBlock*)(mMemory + (sizeof(FixedSizeBlock) + mElementSize) * freeIndex);

						blockInfo->Accessor->SetPointer(lastFreeBlockInfo + 1);
						blockInfo->Accessor->SetIndex(freeIndex);

						std::memcpy(lastFreeBlockInfo, blockInfo, sizeof(FixedSizeBlock) + mElementSize);
						std::memset(blockInfo, 0, sizeof(FixedSizeBlock) + mElementSize);

						freeIndices.erase(freeIndices.find(freeIndex));
						freeIndices.emplace(i);
					}
				}
			}
			else
			{
				freeIndices.emplace(i);
			}
		}

		return freeIndices.empty() ? mElementCount : *freeIndices.begin();
	}

	void FixedSizeAllocator::Expand()
	{
		// TODO
	}

	void FixedSizeAllocator::Contract()
	{
		// TODO

		U32 lastFreeIndex = 0;

		for (U32 i = 0; i < mElementCount; ++i)
		{
			
		}
	}

	void FixedSizeAllocator::Dump()
	{
		for (U32 i = 0; i < mElementCount; ++i)
		{
			FixedSizeBlock* blockInfo = (FixedSizeBlock*)(mMemory + (sizeof(FixedSizeBlock) + mElementSize) * i);

			if (blockInfo->Allocated)
			{
				LOG("Index:%u Allocated\n", i);
			}
			else
			{
				LOG("Index:%u Free\n", i);
			}
		}

		LOG("\n");
	}
}