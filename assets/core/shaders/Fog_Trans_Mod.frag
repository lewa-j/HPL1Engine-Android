#version 100
precision highp float;

varying vec4 v_color;//COLOR0
varying vec3 v_uv;//TEXCOORD0
varying float v_fogUv;//TEXCOORD1

uniform sampler2D diffuseMap;
uniform sampler2D fogMap;

void main()
{
	vec4 diffuse = texture2D(diffuseMap, v_uv.xy);
	float alpha = texture2D(fogMap, vec2(v_fogUv, 0.5)).a;
	gl_FragColor.rgb = diffuse.rgb * alpha + (1.0 - alpha);
	gl_FragColor.a = diffuse.a;
}

