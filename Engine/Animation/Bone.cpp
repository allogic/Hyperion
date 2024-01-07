#include <Engine/Animation/Animation.h>
#include <Engine/Animation/Bone.h>

#include <Engine/Common/Macros.h>

#include <Engine/Renderer/Renderer.h>

#include <Engine/Vulkan/Buffer.h>

#include <Engine/Utility/AssimpUtility.h>

namespace hyperion
{
	Bone::Bone(U32 Id, std::string const& ParentName, std::string const& Name, R32M4 const& Transform, R32M4 const& Offset)
	{
		mId = Id;
		mParentName = ParentName;
		mName = Name;
		mTransform = Transform;
		mOffset = Offset;
	}

	Bone::~Bone()
	{
		
	}

	void Bone::AddChild(Bone* Bone)
	{
		mChildren.emplace_back(Bone);
	}

	void Bone::ComputeBoneTransformRecursive(Buffer* BoneTransformBuffer, Animation* Animation, R32 Time, R32M4& ParentTransform)
	{
		R32M4 worldTransform = ParentTransform * Animation->ComputeTransform(this, Time);

		R32M4* boneTransform = BoneTransformBuffer->GetMappedData<R32M4>() + mId;

		*boneTransform = worldTransform * mOffset;

		for (auto const& child : mChildren)
		{
			child->ComputeBoneTransformRecursive(BoneTransformBuffer, Animation, Time, worldTransform);
		}
	}

	Bone* Bone::FindBone(std::string const& Name)
	{
		if (mName == Name)
		{
			return this;
		}

		for (auto const& child : mChildren)
		{
			if (Bone* bone = child->FindBone(Name))
			{
				return bone;
			}
		}

		return 0;
	}

	void Bone::PrintHierarchy(U32 Offset, U32 Indent, U32 Increment)
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

	void Bone::DrawHierarchy(R32M4& ParentTransform)
	{
		R32M4 worldTransform = ParentTransform * mTransform;

		R32V3 parentPosition = ParentTransform[3];
		R32V3 position = worldTransform[3];

		Renderer::DrawDebugLine(parentPosition, position, 0xFFFF00FF);

		for (auto const& child : mChildren)
		{
			child->DrawHierarchy(worldTransform);
		}
	}
}