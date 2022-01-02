#version 100
precision highp float;

varying vec4 v_color;		//COLOR0
varying vec2 v_uv;			//TEXCOORD0
varying vec3 v_lightDir;	//TEXCOORD1

uniform sampler2D diffuseMap;//TEXUNIT0

void main()
{
	gl_FragColor.rgb = dot(normalize(v_lightDir), vec3(0.0, 0.0, 1.0)) * texture2D(diffuseMap, v_uv).rgb * v_color.rgb; 
	gl_FragColor.a = 0.0;
}