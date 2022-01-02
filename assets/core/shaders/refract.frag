#version 100
precision highp float;

varying vec4 v_position;	//WPOS
varying vec2 v_uv;			//TEXCOORD0
varying vec4 v_color;		//COLOR

uniform sampler2D screenMap;	//TEXUNIT0
uniform sampler2D refractMap;	//TEXUNIT1
uniform float scale;

void main()
{
	vec3 offset = ((texture2D(refractMap, v_uv).xyz * 2.0) - 1.0) * v_color.a;
	gl_FragColor = texture2D(screenMap, v_position.xy - (offset.xy * scale) / v_position.z);
}

