#pragma once

#include <Engine/Interface/Nodes/Button.h>

namespace editor
{
	using namespace hyperion;

	class MyButton : public Button
	{
		DECLARE_INTERFACE_NODE(MyButton, Button)

	public:

		virtual void OnClick() override;
	};
}