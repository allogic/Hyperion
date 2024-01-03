#version 450 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in uint vertexColorChannel0;
layout(location = 2) in vec2 vertexTexCoordChannel0;

layout(binding = 0) uniform TimeInfo
{
	float time;
	float deltaTime;
} timeInfo;

layout(binding = 1) uniform ScreenInfo
{
	vec2 size;
} screenInfo;

layout(binding = 2) uniform ViewProjection
{
	mat4 view;
	mat4 projection;
} viewProjection;

layout(location = 0) out vec3 outputPosition;
layout(location = 1) flat out uint outputColorChannel0;
layout(location = 2) out vec2 outputTexCoordChannel0;

void main()
{
	float ndcX = (2.0 * vertexPosition.x) / screenInfo.size.x - 1.0;
	float ndcY = 1.0 - (2.0 * vertexPosition.y) / screenInfo.size.y;

	vec4 ndcPosition = vec4(ndcX, ndcY, vertexPosition.z, 1.0);

	outputPosition = vec3(ndcPosition);
	outputColorChannel0 = vertexColorChannel0;
	outputTexCoordChannel0 = vertexTexCoordChannel0;

	gl_Position = ndcPosition;
}