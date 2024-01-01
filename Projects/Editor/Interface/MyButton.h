#pragma once

#include <Interface/Components/Button.h>

namespace editor
{
	using namespace interface;

	class MyButton : public Button
	{
		DECLARE_INTERFACE_NODE(MyButton, Button)

	public:

		virtual void OnClick() override;
	};
}