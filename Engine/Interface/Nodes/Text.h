#pragma once

#include <string>

#include <Engine/Interface/Node.h>

namespace hyperion
{
	namespace TextAlignmentMode
	{
		enum Type
		{
			None = 0,
			Left,
			Center,
			Right,
		};
	}

	class Text : public Node
	{
	public:

		inline auto const& GetFont() const { return mFont; }
		inline auto const& GetText() const { return mText; }
		inline auto const& GetValue() const { return mText; }
		inline auto GetFontSize() const { return mFontSize; }
		inline auto GetAlignment() const { return mAlignment; }

	public:

		inline void SetFont(std::string const& Value) { mFont = Value; }
		inline void SetText(std::string const& Value) { mText = Value; }
		inline void SetValue(std::string const& Value) { mValue = Value; }
		inline void SetFontSize(R32 Value) { mFontSize = Value; }
		inline void SetAlignment(TextAlignmentMode::Type Value) { mAlignment = Value; }

	public:

		Text(Node* Parent);
		virtual ~Text();

	public:

		virtual void Update() override;
		virtual void Render() override;

	private:

		std::string mFont = "";
		std::string mText = "";
		std::string mValue = "";

		R32 mFontSize = 1.0F;

		TextAlignmentMode::Type mAlignment = TextAlignmentMode::Left;
	};
}