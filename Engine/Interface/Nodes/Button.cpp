#include <Engine/Interface/Nodes/Button.h>

#include <Engine/Vulkan/Window.h>
#include <Engine/Vulkan/Renderer.h>

namespace hyperion
{
	Button::Button(Node* Parent) : Node(Parent)
	{

	}

	Button::~Button()
	{

	}

	void Button::Update()
	{
		R32V2 mousePosition = Window::GetMousePosition();
		R32V2 buttonPosition = { GetPositionX(), GetPositionY() };
		R32V2 buttonSize = { GetWidth(), GetHeight() };

		if ((mousePosition.x >= buttonPosition.x) && (mousePosition.x < (buttonPosition.x + buttonSize.x)))
		{
			if ((mousePosition.y >= buttonPosition.y) && (mousePosition.y < (buttonPosition.y + buttonSize.y)))
			{
				if (Window::LeftMouseDown())
				{
					OnClick();
				}
			}
		}

		Node::Update();
	}

	void Button::Render()
	{
		R32V3 buttonPosition = { GetPositionX(), GetPositionY(), GetPositionZ() };
		R32V2 buttonSize = { GetWidth(), GetHeight() };

		Renderer::DrawInterfaceRect(buttonPosition, buttonSize, GetBackgroundColor());

		Node::Render();
	}

	void Button::OnClick()
	{

	}
}