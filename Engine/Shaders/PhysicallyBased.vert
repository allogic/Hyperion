#version 450 core

struct Transform
{
	vec3 localPosition;
	uint reserved0;
	vec3 localEulerAngles;
	uint reserved1;
	vec3 localScale;
	uint reserved2;
	vec3 localRight;
	uint reserved3;
	vec3 localUp;
	uint reserved4;
	vec3 localFront;
	uint reserved5;
	vec3 worldPosition;
	uint reserved6;
	vec4 localRotation;
	vec4 worldRotation;
	uint parentIndex;
	uint levelIndex;
	uint allocated;
	uint worldSpace;
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

layout(binding = 0) uniform TimeInfo
{
	float time;
	float deltaTime;
} timeInfo;

layout(binding = 1) uniform ViewProjection
{
	mat4 view;
	mat4 projection;
} viewProjection;

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
	vec4 position = viewProjection.projection * viewProjection.view * vec4(vertexPosition.xyz, 1.0);

	outputPosition = vec3(position);
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

	gl_Position = position;
}