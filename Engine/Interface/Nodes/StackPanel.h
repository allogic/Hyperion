#pragma once

#include <Engine/Interface/Node.h>

namespace hyperion
{
	namespace StackOrientationMode
	{
		enum Type
		{
			None = 0,
			Horizontal,
			Vertical,
		};
	}

	class StackPanel : public Node
	{
	public:

		inline auto GetOrientation() const { return mOrientation; }

	public:

		inline void SetOrientation(StackOrientationMode::Type Value) { mOrientation = Value; }

	public:

		StackPanel(Node* Parent);
		virtual ~StackPanel();

	public:

		virtual void Update() override;
		virtual void Render() override;

	private:

		StackOrientationMode::Type mOrientation = StackOrientationMode::None;
	};
}