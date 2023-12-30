#include <Interface/Components/StackPanel.h>

#include <Vulkan/Renderer.h>

namespace interface
{
	StackPanel::StackPanel(Node* Parent) : Node(Parent)
	{

	}

	StackPanel::~StackPanel()
	{

	}

	void StackPanel::Update()
	{
		auto const& children = GetChildren();

		switch (mOrientation)
		{
			case StackOrientationMode::Horizontal:
			{
				U32 childCount = GetChildCount();
				R32 stepWidth = GetWidth() / childCount;

				for (U32 i = 0; i < childCount; ++i)
				{
					R32 childWidth = stepWidth;
					R32 childHeight = GetHeight();

					R32 childX = GetPositionX() + i * stepWidth;
					R32 childY = GetPositionY();

					children[i]->SetPositionXY(R32V2{ childX, childY });
					children[i]->SetSize(R32V2{ childWidth, childHeight });
				}

				break;
			}
			case StackOrientationMode::Vertical:
			{
				U32 childCount = GetChildCount();
				R32 stepHeight = GetHeight() / childCount;

				for (U32 i = 0; i < childCount; ++i)
				{
					R32 childWidth = GetWidth();
					R32 childHeight = stepHeight;

					R32 childX = GetPositionX();
					R32 childY = GetPositionY() + i * stepHeight;

					children[i]->SetPositionXY(R32V2{ childX, childY });
					children[i]->SetSize(R32V2{ childWidth, childHeight });
				}

				break;
			}
		}

		Node::Update();
	}

	void StackPanel::Render()
	{
		R32V3 stackPosition = { GetPositionX(), GetPositionY(), GetPositionZ() };
		R32V2 stackSize = { GetWidth(), GetHeight() };

		Renderer::DrawInterfaceRect(stackPosition, stackSize, GetBackgroundColor());

		Node::Render();
	}
}