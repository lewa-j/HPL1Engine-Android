#version 100
precision highp float;

attribute vec4 a_position;
attribute vec2 a_uv;
attribute vec4 a_color;

varying vec2 v_uv;
varying vec4 v_color;

uniform mat4 worldViewProj;

void main()
{
	gl_Position = worldViewProj * a_position;
	v_uv = a_uv;
	v_color = a_color;
}

