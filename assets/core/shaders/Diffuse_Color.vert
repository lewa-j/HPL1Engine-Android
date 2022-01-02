#version 100
precision highp float;

attribute vec4 a_position;	//POSITION
attribute vec4 a_color;		//COLOR
attribute vec2 a_uv;		//TEXCOORD0

varying vec4 v_color;	//COLOR
varying vec2 v_uv;		//TEXCOORD0

uniform mat4 worldViewProj;

void main()
{
	gl_Position = worldViewProj * a_position;
	v_color = a_color;
	v_uv = a_uv;
}

