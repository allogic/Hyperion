#version 450 core

layout(location = 0) in vec3 inputPosition;
layout(location = 1) flat in uint inputColor;

layout(location = 0) out vec4 outputColor;

void main()
{
	float r = ((inputColor >> 24) & 0xFF) / 255.0;
	float g = ((inputColor >> 16) & 0xFF) / 255.0;
	float b = ((inputColor >> 8) & 0xFF) / 255.0;
	float a = ((inputColor) & 0xFF) / 255.0;

	outputColor = vec4(r, g, b, a);
}