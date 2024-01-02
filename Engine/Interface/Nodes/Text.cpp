#include <Engine/Common/ValueDatabase.h>

#include <Engine/Interface/Nodes/Text.h>

#include <Engine/Renderer/Renderer.h>

namespace hyperion
{
	Text::Text(Node* Parent) : Node(Parent)
	{

	}

	Text::~Text()
	{

	}

	void Text::Update()
	{
		Node::Update();
	}

	void Text::Render()
	{
		R32V3 textPosition = { GetPositionX(), GetPositionY(), GetPositionZ() };
		R32V2 textSize = { GetWidth(), GetHeight() };

		R32 textWidth = 0.0F;

		if (mValue.empty())
		{
			textWidth = Renderer::GetTextWidth(mFont, mText, mFontSize);
		}
		else
		{
			R32 value = ValueDatabase::GetValue(mValue);

			textWidth = Renderer::GetTextWidth(mFont, mText, mFontSize, &value);
		}

		switch (mAlignment)
		{
			case TextAlignmentMode::Center:
			{
				textPosition.x += textSize.x / 2.0F - textWidth / 2.0F;

				break;
			}
			case TextAlignmentMode::Right:
			{
				textPosition.x += textSize.x - textWidth;

				break;
			}
		}

		if (mValue.empty())
		{
			Renderer::DrawText(textPosition, mFont, mText, mFontSize, GetColor());
		}
		else
		{
			R32 value = ValueDatabase::GetValue(mValue);

			Renderer::DrawText(textPosition, mFont, mText, mFontSize, GetColor(), &value);
		}

		Node::Render();
	}
}