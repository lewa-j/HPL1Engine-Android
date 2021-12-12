#version 100
precision highp float;

varying vec4 v_color;//COLOR0
varying vec2 v_uv;//TEXCOORD0

uniform sampler2D diffuseMap;//TEXUNIT0
uniform vec3 ambientColor;

void main()
{
	gl_FragColor = texture2D(diffuseMap, v_uv) * v_color;
	gl_FragColor.rgb *= ambientColor;
}

