#pragma once

#include <vector>

#include <Engine/Forward.h>

#include <Engine/Renderer/Vertex.h>

#include <Engine/Vulkan/Buffer.h>
#include <Engine/Vulkan/BufferVariance.h>

namespace hyperion
{
	class Mesh
	{
	public:

		inline auto GetMaterial() const { return mMaterial; }
		inline auto GetVertexBuffer() const { return mVertexBuffer; }
		inline auto GetIndexBuffer() const { return mIndexBuffer; }

	public:

		inline void SetMaterial(Material* Value) { mMaterial = Value; }
		inline void SetVertexBuffer(Buffer* Value) { mVertexBuffer = Value; }
		inline void SetIndexBuffer(Buffer* Value) { mIndexBuffer = Value; }

	public:

		Mesh();
		virtual ~Mesh();

	private:

		Material* mMaterial = 0;

		Buffer* mVertexBuffer = 0;
		Buffer* mIndexBuffer = 0;
	};
}