#pragma once

#include <Common/Types.h>

#include <Vulkan/Forward.h>

namespace vulkan
{
	using namespace common;

	class BufferVariance
	{
	public:

		static Buffer* CreateVertex(void* Data, U64 Size);
		static Buffer* CreateIndex(void* Data, U64 Size);
		static Buffer* CreateUniform(void* Data, U64 Size);
		static Buffer* CreateStorage(void* Data, U64 Size);

		static Buffer* CreateVertexCoherent(U64 Size);
		static Buffer* CreateIndexCoherent(U64 Size);
		static Buffer* CreateUniformCoherent(U64 Size);
		static Buffer* CreateStorageCoherent(U64 Size);
	};
}