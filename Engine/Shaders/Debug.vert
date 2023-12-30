#version 450 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in uint vertexColor;

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
layout(location = 1) flat out uint outputColor;

void main()
{
	vec4 position = viewProjection.projection * viewProjection.view * vec4(vertexPosition, 1.0);

	outputPosition = vec3(position);
	outputColor = vertexColor;

	gl_Position = position;
}