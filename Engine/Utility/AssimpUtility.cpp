#include <Engine/Assimp/scene.h>

#include <Engine/Utility/AssimpUtility.h>

namespace hyperion
{
	R32M4 AssimpUtility::ToGlmMatrix4(aiMatrix4x4 const& Matrix)
	{
		R32M4 result = {};

		result[0][0] = Matrix.a1; result[1][0] = Matrix.a2; result[2][0] = Matrix.a3; result[3][0] = Matrix.a4;
		result[0][1] = Matrix.b1; result[1][1] = Matrix.b2; result[2][1] = Matrix.b3; result[3][1] = Matrix.b4;
		result[0][2] = Matrix.c1; result[1][2] = Matrix.c2; result[2][2] = Matrix.c3; result[3][2] = Matrix.c4;
		result[0][3] = Matrix.d1; result[1][3] = Matrix.d2; result[2][3] = Matrix.d3; result[3][3] = Matrix.d4;

		return result;
	}

	R32V3 AssimpUtility::ToGlmVector3(aiVector3D const& Vector)
	{
		return R32V3{ Vector.x, Vector.y, Vector.z };
	}

	R32V4 AssimpUtility::ToGlmVector4(aiColor4D const& Color)
	{
		return R32V4{ Color.r, Color.g, Color.b, Color.a };
	}

	R32Q AssimpUtility::ToGlmQuaternion(aiQuaternion const& Orientation)
	{
		return R32Q{ Orientation.w, Orientation.x, Orientation.y, Orientation.z };
	}

	aiNode const* AssimpUtility::FindNodeByNameRecursive(aiNode const* Node, std::string const& Name)
	{
		if (std::strcmp(Name.data(), Node->mName.C_Str()) == 0)
		{
			return Node;
		}

		for (U32 childIndex = 0; childIndex < Node->mNumChildren; ++childIndex)
		{
			aiNode const* node = FindNodeByNameRecursive(Node->mChildren[childIndex], Name);

			if (node)
			{
				return node;
			}
		}

		return 0;
	}
}