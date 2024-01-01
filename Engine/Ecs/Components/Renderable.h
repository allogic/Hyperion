#pragma once

#include <Engine/Forward.h>

#include <Engine/Ecs/Component.h>

namespace hyperion
{
	class Renderable : public Component
	{
	public:

		Renderable();
		virtual ~Renderable();

	private:

		Buffer* mVertexBuffer = 0;
		Buffer* mIndexBuffer = 0;
	};
}