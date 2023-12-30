#version 450 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUv;
layout(location = 2) in uint vertexColor;

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
layout(location = 1) out vec2 outputUv;
layout(location = 2) flat out uint outputColor;

void main()
{
	float ndcX = (2.0 * vertexPosition.x) / screenInfo.size.x - 1.0;
	float ndcY = 1.0 - (2.0 * vertexPosition.y) / screenInfo.size.y;

	vec4 ndcPosition = vec4(ndcX, ndcY, vertexPosition.z, 1.0);

	outputPosition = vec3(ndcPosition);
	outputUv = vertexUv;
	outputColor = vertexColor;

	gl_Position = ndcPosition;
}