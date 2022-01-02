#version 100
precision highp float;

varying vec4 v_position;//WPOS
varying vec2 v_uv;		//TEXCOORD0

uniform sampler2D diffuseMap;//TEXUNIT0
uniform float timeCount;

void main()
{
	vec2 uv = v_uv;
	uv.y += sin(timeCount * 6.0 + v_position.x * 16.0) * 0.01;
	gl_FragColor = texture2D(diffuseMap, uv);
}