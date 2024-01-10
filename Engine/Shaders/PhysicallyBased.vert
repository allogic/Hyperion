#version 450 core

#define MAX_BONE_INFLUENCE 4

struct FixedSizeBlock
{
	uint allocated;
	uint reserved0;
	uint accessorHigh;
	uint accessorLow;
};

struct Bone
{
	FixedSizeBlock fixedSizeBlock;
	uint channelViewIndex;
	uint allocated;
	int parentIndex;
	uint reserved0;
	mat4 parentTransform;
	mat4 offsetTransform;
	mat4 localTransform;
	mat4 worldTransform;
};

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec3 vertexTangent;
layout(location = 3) in vec3 vertexBiTangent;
layout(location = 4) in vec4 vertexColorChannel0;
layout(location = 5) in vec4 vertexColorChannel1;
layout(location = 6) in vec4 vertexColorChannel2;
layout(location = 7) in vec4 vertexColorChannel3;
layout(location = 8) in vec4 vertexColorChannel4;
layout(location = 9) in vec4 vertexColorChannel5;
layout(location = 10) in vec4 vertexColorChannel6;
layout(location = 11) in vec4 vertexColorChannel7;
layout(location = 12) in vec3 vertexTexCoordChannel0;
layout(location = 13) in vec3 vertexTexCoordChannel1;
layout(location = 14) in vec3 vertexTexCoordChannel2;
layout(location = 15) in vec3 vertexTexCoordChannel3;
layout(location = 16) in vec3 vertexTexCoordChannel4;
layout(location = 17) in vec3 vertexTexCoordChannel5;
layout(location = 18) in vec3 vertexTexCoordChannel6;
layout(location = 19) in vec3 vertexTexCoordChannel7;
layout(location = 20) in ivec4 vertexBoneIndices;
layout(location = 21) in vec4 vertexBoneWeights;

layout(binding = 0) uniform TimeInfo
{
	float time;
	float deltaTime;
} timeInfo;

layout(binding = 1) uniform ProjectionInfo
{
	mat4 view;
	mat4 projection;
} projectionInfo;

layout(binding = 2) buffer BoneBuffer
{
	Bone bones[];
} boneBuffer;

layout(location = 0) out vec3 outputPosition;
layout(location = 1) out vec3 outputNormal;
layout(location = 2) out vec3 outputTangent;
layout(location = 3) out vec3 outputBiTangent;
layout(location = 4) out vec4 outputColorChannel0;
layout(location = 5) out vec4 outputColorChannel1;
layout(location = 6) out vec4 outputColorChannel2;
layout(location = 7) out vec4 outputColorChannel3;
layout(location = 8) out vec4 outputColorChannel4;
layout(location = 9) out vec4 outputColorChannel5;
layout(location = 10) out vec4 outputColorChannel6;
layout(location = 11) out vec4 outputColorChannel7;
layout(location = 12) out vec3 outputTexCoordChannel0;
layout(location = 13) out vec3 outputTexCoordChannel1;
layout(location = 14) out vec3 outputTexCoordChannel2;
layout(location = 15) out vec3 outputTexCoordChannel3;
layout(location = 16) out vec3 outputTexCoordChannel4;
layout(location = 17) out vec3 outputTexCoordChannel5;
layout(location = 18) out vec3 outputTexCoordChannel6;
layout(location = 19) out vec3 outputTexCoordChannel7;

void main()
{
	vec4 worldPosition = vec4(vertexPosition, 1.0);

	for (uint i = 0; i < MAX_BONE_INFLUENCE; ++i)
	{
		if (vertexBoneIndices[i] >= 0)
		{
			Bone bone = boneBuffer.bones[vertexBoneIndices[i]];

			if (bone.allocated == 1)
			{
				vec4 localPosition = bone.parentTransform * vec4(vertexPosition, 1.0);
	
				worldPosition += localPosition * vertexBoneWeights[i];

				//vec3 localNormal = mat3(boneInfo.boneTransforms[vertexBoneIds[i]]) * vertexNormal; // TODO
			}	
		}
	}

	worldPosition = projectionInfo.projection * projectionInfo.view * worldPosition;

	outputPosition = vec3(worldPosition);
	outputNormal = vertexNormal;
	outputTangent = vertexTangent;
	outputBiTangent = vertexBiTangent;
	outputColorChannel0 = vertexColorChannel0;
	outputColorChannel1 = vertexColorChannel1;
	outputColorChannel2 = vertexColorChannel2;
	outputColorChannel3 = vertexColorChannel3;
	outputColorChannel4 = vertexColorChannel4;
	outputColorChannel5 = vertexColorChannel5;
	outputColorChannel6 = vertexColorChannel6;
	outputColorChannel7 = vertexColorChannel7;
	outputTexCoordChannel0 = vertexTexCoordChannel0;
	outputTexCoordChannel1 = vertexTexCoordChannel1;
	outputTexCoordChannel2 = vertexTexCoordChannel2;
	outputTexCoordChannel3 = vertexTexCoordChannel3;
	outputTexCoordChannel4 = vertexTexCoordChannel4;
	outputTexCoordChannel5 = vertexTexCoordChannel5;
	outputTexCoordChannel6 = vertexTexCoordChannel6;
	outputTexCoordChannel7 = vertexTexCoordChannel7;

	gl_Position = worldPosition;
}