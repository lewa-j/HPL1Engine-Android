#version 100
precision highp float;

attribute vec4 a_position;//POSITION
attribute vec3 a_normal;//NORMAL
attribute vec2 a_uv;//TEXCOORD0
attribute vec4 a_tangent;//TEXCOORD1

varying vec4 v_color;//COLOR0
varying vec2 v_uv;//TEXCOORD0
varying vec3 v_lightDir;//TEXCOORD1
varying vec4 v_spotlightUv;//TEXCOORD2

uniform mat4 worldViewProj;
uniform mat4 spotViewProj;
uniform vec3 LightPos;
uniform vec4 LightColor;
uniform vec3 LightDirMul;
			
void main()
{
	gl_Position = worldViewProj * a_position;
	v_uv = a_uv;
	v_spotlightUv = spotViewProj * a_position;

	vec3 lightDir = LightPos - a_position.xyz;
	v_lightDir = lightDir * LightDirMul;

	//Calculate rotation for light to get it to tangent space.
	vec3 binormal = cross(a_normal,a_tangent.xyz)*a_tangent.w;
	mat3 rotation = mat3(a_tangent.xyz, binormal, a_normal);
		
	//Transform the lightdir
	v_lightDir = rotation * v_lightDir;
	
	v_color = LightColor;
}

