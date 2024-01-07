#version 450 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in uint vertexColorChannel0;

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

layout(location = 0) out vec3 outputPosition;
layout(location = 1) flat out uint outputColorChannel0;

void main()
{
	vec4 position = projectionInfo.projection * projectionInfo.view * vec4(vertexPosition, 1.0);

	outputPosition = vec3(position);
	outputColorChannel0 = vertexColorChannel0;

	gl_Position = position;
}