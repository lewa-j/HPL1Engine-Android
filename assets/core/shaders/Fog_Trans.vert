#version 100
precision highp float;

attribute vec4 a_position;	//POSITION
attribute vec4 a_color;		//COLOR
attribute vec3 a_uv;		//TEXCOORD0

varying vec4 v_color;	//COLOR
varying vec3 v_uv;		//TEXCOORD0
varying float v_fogUv;	//TEXCOORD1

uniform mat4 worldViewProj;
uniform float fogStart;
uniform float fogEnd;

void main()
{
	gl_Position = worldViewProj * a_position;
	v_uv = a_uv;
	v_fogUv = (fogEnd - gl_Position.z) / (fogEnd - fogStart);
	v_color = a_color;
}

