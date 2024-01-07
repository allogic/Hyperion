#pragma once

#include <string>

#include <Engine/Assimp/quaternion.h>
#include <Engine/Assimp/vector3.h>
#include <Engine/Assimp/color4.h>
#include <Engine/Assimp/matrix4x4.h>

#include <Engine/Common/Types.h>

struct aiNode;

namespace hyperion
{
	class AssimpUtility
	{
	public:

		static R32M4 ToGlmMatrix4(aiMatrix4x4 const& Matrix);

		static R32V3 ToGlmVector3(aiVector3D const& Vector);
		static R32V4 ToGlmVector4(aiColor4D const& Color);

		static R32Q ToGlmQuaternion(aiQuaternion const& Orientation);

		static aiNode const* FindNodeByNameRecursive(aiNode const* Node, std::string const& Name);
	};
}