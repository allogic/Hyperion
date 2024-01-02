#include <Engine/Common/Macros.h>

#include <Engine/Interface/Node.h>

#include <Engine/Platform/Window.h>

#include <Engine/Renderer/Renderer.h>

namespace hyperion
{
	R32 Node::GetPositionX() const
	{
		R32 position = 0.0F;

		switch (mHorizontalAlignment)
		{
			case NodeAlignmentMode::None:
			{
				position = mPositionX + mHorizontalOffset;

				break;
			}
			case NodeAlignmentMode::Center:
			{
				if (mParent)
				{
					position = (mParent->GetWidth() / 2.0F) - (GetWidth() / 2.0F);
				}
				else
				{
					position = mPositionX + mHorizontalOffset;
				}

				break;
			}
			case NodeAlignmentMode::Left:
			{
				position = mPositionX + mHorizontalOffset;

				break;
			}
			case NodeAlignmentMode::Right:
			{
				if (mParent)
				{
					position = mParent->GetWidth() - GetWidth() - mHorizontalOffset;
				}
				else
				{
					position = mPositionX + mHorizontalOffset;
				}

				break;
			}
		}

		return position;
	}

	R32 Node::GetPositionY() const
	{
		R32 position = 0.0F;

		switch (mVerticalAlignment)
		{
			case NodeAlignmentMode::None:
			{
				position = mPositionY + mVerticalOffset;

				break;
			}
			case NodeAlignmentMode::Center:
			{
				if (mParent)
				{
					position = (mParent->GetHeight() / 2.0F) - (GetHeight() / 2.0F);
				}
				else
				{
					position = mPositionY + mVerticalOffset;
				}

				break;
			}
			case NodeAlignmentMode::Top:
			{
				if (mParent)
				{
					position = mParent->GetHeight() - GetHeight() - mVerticalOffset;
				}
				else
				{
					position = mPositionY + mVerticalOffset;
				}

				break;
			}
			case NodeAlignmentMode::Bottom:
			{
				position = mPositionY + mVerticalOffset;

				break;
			}
		}

		return position;
	}

	R32 Node::GetPositionZ() const
	{
		return mPositionZ;
	}

	R32 Node::GetWidth() const
	{
		if (mWidth == 0.0F)
		{
			if (mParent)
			{
				return mParent->GetWidth();
			}
			else
			{
				return (R32)gWindow->GetWindowWidth();
			}
		}
		else
		{
			return mWidth;
		}
	}

	R32 Node::GetHeight() const
	{
		if (mHeight == 0.0F)
		{
			if (mParent)
			{
				return mParent->GetHeight();
			}
			else
			{
				return (R32)gWindow->GetWindowHeight();
			}
		}
		else
		{
			return mHeight;
		}
	}

	Node::Node(Node* Parent)
	{
		mParent = Parent;
	}

	Node::~Node()
	{
		for (auto const& child : mChildren)
		{
			delete child;
		}
	}

	void Node::Update()
	{
		for (auto const& child : mChildren)
		{
			child->Update();
		}
	}

	void Node::Render()
	{
		for (auto const& child : mChildren)
		{
			child->Render();
		}
	}

	void Node::AddChild(Node* Node)
	{
		mChildren.emplace_back(Node);
	}

	void Node::RemoveChild(Node* Node)
	{
		mChildren.erase(std::find(mChildren.begin(), mChildren.end(), Node));
	}

	void Node::PrintHierarchy(U32 Offset, U32 Indent, U32 Increment)
	{
		if (mParent)
		{
			for (U32 i = 0; i < Offset; ++i)
			{
				LOG(" ");
			}

			for (U32 i = 0; i < Indent; ++i)
			{
				LOG("%c", ((i % Increment == 0) && (i >= Increment)) ? '|' : ' ');
			}
		}

		LOG("%s\n", mName.data());

		for (auto const& child : mChildren)
		{
			child->PrintHierarchy(Offset, Indent + Increment, Increment);
		}

		if (!mParent)
		{
			LOG("\n");
		}
	}
}