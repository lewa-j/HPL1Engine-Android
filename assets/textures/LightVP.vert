#version 100
precision highp float;

attribute vec4 a_position;	//POSITION
attribute vec3 a_normal;	//NORMAL
attribute vec2 a_uv;		//TEXCOORD0
attribute vec4 a_tangent;	//TEXCOORD1

varying vec4 v_color;		//COLOR0
varying vec2 v_uv;			//TEXCOORD0
varying vec3 v_lightDir;	//TEXCOORD1

uniform mat4 worldViewProj;	//ViewProjection matrix
uniform vec3 LightPos;		//The lightpostion (local to model)
uniform vec4 LightColor;	//Color of the light

void main()
{
	gl_Position = worldViewProj * a_position;
	v_uv = a_uv;

	//The direction of the light
	v_lightDir = LightPos - a_position.xyz;
	
	//Calculate rotation for light to get it to tangent space.
	vec3 binormal = cross(a_normal, a_tangent.xyz) * a_tangent.w;
	mat3 rotation = mat3(a_tangent.xyz, binormal, a_normal);
		
	//Transform the lightdir to tangent space
	v_lightDir = rotation * v_lightDir;
	
	//Set the light color
    v_color = LightColor;
}
