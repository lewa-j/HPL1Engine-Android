#version 100
precision highp float;

varying vec4 v_color;
varying vec2 v_uv;

uniform sampler2D diffuseMap;

void main()
{
	vec4 tex = texture2D(diffuseMap, v_uv);
	gl_FragColor = tex * v_color;
}

