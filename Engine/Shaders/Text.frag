#version 450 core

layout(location = 0) in vec3 inputPosition;
layout(location = 1) flat in uint inputColorChannel0;
layout(location = 2) in vec2 inputTexCoordChannel0;

layout(binding = 3) uniform sampler2D textureAtlasSampler;

layout(location = 0) out vec4 outputColor;

void main()
{
	float r = ((inputColorChannel0 >> 24) & 0xFF) / 255.0;
	float g = ((inputColorChannel0 >> 16) & 0xFF) / 255.0;
	float b = ((inputColorChannel0 >> 8) & 0xFF) / 255.0;
	float a = ((inputColorChannel0) & 0xFF) / 255.0;

	a *= texture(textureAtlasSampler, inputTexCoordChannel0).r;

	outputColor = vec4(r, g, b, a);
}