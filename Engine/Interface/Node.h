#pragma once

#include <string>
#include <vector>

#include <Engine/Common/Types.h>

#include <Engine/Interface/LayoutParser.h>

namespace hyperion
{
	namespace NodeAlignmentMode
	{
		enum Type
		{
			None = 0,
			Center,
			Top,
			Bottom,
			Left,
			Right,
		};
	}

	class Node
	{
	public:

		inline auto GetParent() const { return mParent; }
		inline auto const& GetId() const { return mId; }
		inline auto const& GetName() const { return mName; }
		inline auto const& GetChildren() const { return mChildren; }
		inline auto GetChildCount() const { return (U32)mChildren.size(); }
		inline auto GetHorizontalAlignment() const { return mHorizontalAlignment; }
		inline auto GetVerticalAlignment() const { return mVerticalAlignment; }
		inline auto GetHorizontalOffset() const { return mHorizontalOffset; }
		inline auto GetVerticalOffset() const { return mVerticalOffset; }
		inline auto GetColor() const { return mColor; }
		inline auto GetBackgroundColor() const { return mBackgroundColor; }
		inline auto GetRow() const { return mRow; }
		inline auto GetColumn() const { return mColumn; }

	public:

		inline void SetId(std::string const& Value) { mId = Value; }
		inline void SetName(std::string const& Value) { mName = Value; }
		inline void SetPositionX(R32 Value) { mPositionX = Value; }
		inline void SetPositionY(R32 Value) { mPositionY = Value; }
		inline void SetPositionZ(R32 Value) { mPositionZ = Value; }
		inline void SetPositionXY(R32V2 const& Value) { mPositionX = Value.x; mPositionY = Value.y; }
		inline void SetPositionXYZ(R32V3 const& Value) { mPositionX = Value.x; mPositionY = Value.y; mPositionZ = Value.z; }
		inline void SetHorizontalAlignment(NodeAlignmentMode::Type Value) { mHorizontalAlignment = Value; }
		inline void SetVerticalAlignment(NodeAlignmentMode::Type Value) { mVerticalAlignment = Value; }
		inline void SetHorizontalOffset(R32 Value) { mHorizontalOffset = Value; }
		inline void SetVerticalOffset(R32 Value) { mVerticalOffset = Value; }
		inline void SetWidth(R32 Value) { mWidth = Value; }
		inline void SetHeight(R32 Value) { mHeight = Value; }
		inline void SetSize(R32V2 const& Value) { mWidth = Value.x; mHeight = Value.y; }
		inline void SetColor(U32 Value) { mColor = Value; }
		inline void SetBackgroundColor(U32 Value) { mBackgroundColor = Value; }
		inline void SetRow(U32 Value) { mRow = Value; }
		inline void SetColumn(U32 Value) { mColumn = Value; }

	public:

		R32 GetPositionX() const;
		R32 GetPositionY() const;
		R32 GetPositionZ() const;

		R32 GetWidth() const;
		R32 GetHeight() const;

	public:

		Node(Node* Parent);
		virtual ~Node();

	public:

		virtual void Update();
		virtual void Render();

	public:

		void AddChild(Node* Node);
		void RemoveChild(Node* Node);

	public:

		void PrintHierarchy(U32 Offset = 0, U32 Indent = 0, U32 Increment = 2);

	private:

		Node* mParent = 0;

		std::string mId = "";
		std::string mName = "";

		std::vector<Node*> mChildren = {};

		R32 mPositionX = 0.0F;
		R32 mPositionY = 0.0F;
		R32 mPositionZ = 0.0F;

		R32 mWidth = 0.0F;
		R32 mHeight = 0.0F;

		U32 mColor = 0xFFFFFFFF;
		U32 mBackgroundColor = 0xFFFFFF00;

		NodeAlignmentMode::Type mHorizontalAlignment = NodeAlignmentMode::None;
		NodeAlignmentMode::Type mVerticalAlignment = NodeAlignmentMode::None;

		R32 mHorizontalOffset = 0.0F;
		R32 mVerticalOffset = 0.0F;

		U32 mRow = 0;
		U32 mColumn = 0;
	};
}