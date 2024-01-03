#version 450 core

layout(location = 0) in vec3 inputPosition;
layout(location = 1) in vec3 inputNormal;
layout(location = 2) in vec3 inputTangent;
layout(location = 3) in vec3 inputBiTangent;
layout(location = 4) in vec4 inputColorChannel0;
layout(location = 5) in vec4 inputColorChannel1;
layout(location = 6) in vec4 inputColorChannel2;
layout(location = 7) in vec4 inputColorChannel3;
layout(location = 8) in vec4 inputColorChannel4;
layout(location = 9) in vec4 inputColorChannel5;
layout(location = 10) in vec4 inputColorChannel6;
layout(location = 11) in vec4 inputColorChannel7;
layout(location = 12) in vec3 inputTexCoordChannel0;
layout(location = 13) in vec3 inputTexCoordChannel1;
layout(location = 14) in vec3 inputTexCoordChannel2;
layout(location = 15) in vec3 inputTexCoordChannel3;
layout(location = 16) in vec3 inputTexCoordChannel4;
layout(location = 17) in vec3 inputTexCoordChannel5;
layout(location = 18) in vec3 inputTexCoordChannel6;
layout(location = 19) in vec3 inputTexCoordChannel7;

layout(location = 0) out vec4 outputColor;

void main()
{
	outputColor = vec4(inputNormal.xy, 0.0, 1.0);
}