#version 100
precision highp float;

attribute vec4 a_position;// POSITION

uniform mat4 worldViewProj;
uniform vec3 lightPosition;

void main()
{
	vec4 position = a_position;
	position.xyz -= lightPosition * (1.0 - position.w);
	
	gl_Position = worldViewProj * position;
}

