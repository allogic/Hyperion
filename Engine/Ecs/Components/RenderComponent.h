#pragma once

#include <Engine/Ecs/Component.h>

namespace hyperion
{
	class RenderComponent : public Component
	{
	public:

		inline auto GetSharedMaterial() const { return mSharedMaterial; }
		inline auto GetSharedBoneBuffer() const { return mSharedBoneBuffer; }
		inline auto GetSharedVertexBuffer() const { return mSharedVertexBuffer; }
		inline auto GetSharedIndexBuffer() const { return mSharedIndexBuffer; }

	public:

		inline void SetSharedMaterial(Material* Value) { mSharedMaterial = Value; }
		inline void SetSharedBoneBuffer(Buffer* Value) { mSharedBoneBuffer = Value; }
		inline void SetSharedVertexBuffer(Buffer* Value) { mSharedVertexBuffer = Value; }
		inline void SetSharedIndexBuffer(Buffer* Value) { mSharedIndexBuffer = Value; }

	public:

		RenderComponent(ComponentArguments const& Arguments);
		virtual ~RenderComponent();

	private:

		Material* mSharedMaterial = 0;

		Buffer* mSharedBoneBuffer = 0;
		Buffer* mSharedVertexBuffer = 0;
		Buffer* mSharedIndexBuffer = 0;
	};
}