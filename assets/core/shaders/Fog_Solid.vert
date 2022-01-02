#version 100
precision highp float;

attribute vec4 a_position;	//POSITION

varying vec4 v_color;	//COLOR
varying float v_uv;		//TEXCOORD0

uniform mat4 worldViewProj;
uniform vec3 fogColor;
uniform float fogStart;
uniform float fogEnd;

void main()
{
	gl_Position = worldViewProj * a_position;
	v_uv = (fogEnd - gl_Position.z) / (fogEnd - fogStart);
	v_color.rgb = fogColor;
	v_color.a = 1.0;
}

