#version 100
precision highp float;

attribute vec4 a_position;
attribute vec4 a_color;
attribute vec2 a_uv;

varying vec4 v_color;
varying vec2 v_uv;

uniform mat4 worldViewProj;
uniform vec4 color;

void main()
{
	gl_Position = worldViewProj * a_position;
	v_color = a_color * color;
	v_uv = a_uv;
}

