#pragma once

#include <Engine/Forward.h>

#include <Engine/Common/Types.h>

namespace hyperion
{
	struct ComponentArguments
	{
		void* Dummy;
	};

	class Component
	{
	public:

		Component(ComponentArguments const& Arguments);
		virtual ~Component();
	};
}