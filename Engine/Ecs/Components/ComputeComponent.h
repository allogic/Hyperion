#pragma once

#include <Engine/Ecs/Component.h>

namespace hyperion
{
	class ComputeComponent : public Component
	{
	public:

		ComputeComponent(ComponentArguments const& Arguments);
		virtual ~ComputeComponent();
	};
}