#pragma once

#include <Engine/Forward.h>

#include <Engine/Ecs/Component.h>

namespace hyperion
{
	class Renderable : public Component
	{
	public:

		inline auto GetSharedMaterial() const { return mSharedMaterial; }
		inline auto GetSharedVertexBuffer() const { return mSharedVertexBuffer; }
		inline auto GetSharedIndexBuffer() const { return mSharedIndexBuffer; }

	public:

		inline void SetSharedMaterial(Material* Value) { mSharedMaterial = Value; }
		inline void SetSharedVertexBuffer(Buffer* Value) { mSharedVertexBuffer = Value; }
		inline void SetSharedIndexBuffer(Buffer* Value) { mSharedIndexBuffer = Value; }

	public:

		Renderable();
		virtual ~Renderable();

	private:

		Material* mSharedMaterial = 0;

		Buffer* mSharedVertexBuffer = 0;
		Buffer* mSharedIndexBuffer = 0;
	};
}