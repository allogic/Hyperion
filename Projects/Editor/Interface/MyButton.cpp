#include <Editor/Interface/MyButton.h>

namespace editor
{
	void MyButton::OnClick()
	{
		static bool open = true;

		if (Node* parent = GetParent())
		{
			open = !open;

			if (open)
			{
				parent->SetHeight(50);
			}
			else
			{
				parent->SetHeight(100);
			}
		}
	}
};