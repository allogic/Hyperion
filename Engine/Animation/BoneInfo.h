#pragma once

#include <string>
#include <vector>

#include <Engine/Forward.h>

#include <Engine/Memory/FixedSizeAccessor.h>

namespace hyperion
{
	struct BoneInfoArguments
	{
		std::string Name;
		std::string ParentName;
		FixedSizeAccessor* Accessor;
	};

	class BoneInfo
	{
	public:

		inline auto const& GetName() const { return mName; }
		inline auto const& GetParentName() const { return mParentName; }
		inline auto GetBone() const { return mAccessor->GetData<Bone>(); }
		inline auto GetBoneIndex() const { return mAccessor->GetIndex(); }
		inline auto const& GetChildren() const { return mChildren; }

	public:

		inline void SetParent(BoneInfo* Value) { mParent = Value; }

	public:

		BoneInfo(BoneInfoArguments const& Arguments);
		virtual ~BoneInfo();

	public:

		void AddChild(BoneInfo* BoneInfo);

	public:

		void UpdateParentIndices();

	public:

		void PrintHierarchy(U32 Offset = 0, U32 Indent = 0, U32 Increment = 2);
		void DrawHierarchy(R32M4 const& WorldTransform, U32 Color);

	private:

		std::string mName = "";
		std::string mParentName = "";
		FixedSizeAccessor* mAccessor = 0;

		BoneInfo* mParent = 0;

		std::vector<BoneInfo*> mChildren = {};
	};
}