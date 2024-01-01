#pragma once

#include <Engine/Interface/Node.h>

namespace hyperion
{
	class Button : public Node
	{
	public:

		Button(Node* Parent);
		virtual ~Button();

	public:

		virtual void Update() override;
		virtual void Render() override;

	public:

		virtual void OnClick();
	};
}