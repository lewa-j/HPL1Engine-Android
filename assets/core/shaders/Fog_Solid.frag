#version 100
precision highp float;

varying vec4 v_color;	//COLOR0
varying float v_uv;		//TEXCOORD0

uniform sampler2D diffuseMap;

void main()
{
	gl_FragColor = texture2D(diffuseMap, vec2(v_uv, 0.5)) * v_color;
}

