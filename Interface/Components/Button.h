#pragma once

#include <Interface/Node.h>

namespace interface
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