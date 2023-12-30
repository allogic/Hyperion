#include <Interface/Components/GridPanel.h>

#include <Vulkan/Renderer.h>

namespace interface
{
	GridPanel::GridPanel(Node* Parent) : Node(Parent)
	{
		
	}

	GridPanel::~GridPanel()
	{

	}

	void GridPanel::Update()
	{
		for (auto const& child : GetChildren())
		{
			R32 childWidth = GetWidth() / mColumns;
			R32 childHeight = GetHeight() / mRows;

			R32 childX = GetPositionX() + childWidth * child->GetColumn();
			R32 childY = GetPositionY() + childHeight * child->GetRow();

			child->SetPositionXY(R32V2{ childX, childY });
			child->SetSize(R32V2{ childWidth, childHeight });
		}

		Node::Update();
	}

	void GridPanel::Render()
	{
		R32V3 gridPosition = { GetPositionX(), GetPositionY(), GetPositionZ() };
		R32V2 gridSize = { GetWidth(), GetHeight() };

		Renderer::DrawInterfaceRect(gridPosition, gridSize, GetBackgroundColor());

		Node::Render();
	}
}