#pragma once

#include <string>
#include <vector>

#include <Engine/Common/Types.h>

namespace hyperion
{
	class Bone
	{
	public:

		inline auto GetId() const { return mId; }
		inline auto GetParentName() const { return mParentName; }
		inline auto GetName() const { return mName; }
		inline auto GetParent() const { return mParent; }

	public:

		inline void SetParent(Bone* Value) { mParent = Value; }

	public:

		Bone(U32 Id, std::string const& ParentName, std::string const& Name, R32M4 const& Transform, R32M4 const& Offset);
		virtual ~Bone();

	public:

		void AddChild(Bone* Bone);

	public:

		void ComputeBoneTransformRecursive(Buffer* BoneTransformBuffer, Animation* Animation, R32 Time, R32M4& ParentTransform);

	public:

		Bone* FindBone(std::string const& Name);

	public:

		void PrintHierarchy(U32 Offset = 0, U32 Indent = 0, U32 Increment = 2);
		void DrawHierarchy(R32M4& ParentTransform);

	private:

		U32 mId = 0;

		std::string mParentName = "";
		std::string mName = "";

		R32M4 mTransform = {};
		R32M4 mOffset = {};

		Bone* mParent = 0;

		std::vector<Bone*> mChildren = {};
	};
}