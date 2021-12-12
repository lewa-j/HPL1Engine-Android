#version 100
precision highp float;

varying vec4 v_position;//WPOS
varying vec2 v_uv;//TEXCOORD0
varying vec4 v_color;//COLOR

uniform sampler2D screenMap;//TEXUNIT0
uniform sampler2D refractMap;//TEXUNIT1
uniform sampler2D alphaMap;//TEXUNIT2

uniform vec2 screenSize;
uniform float t;
uniform float scale;

void main()
{
	float alpha = texture2D(alphaMap, v_uv).x;
	vec2 uv2 = (v_position.xy / screenSize);
	uv2.x += sin(t + uv2.y * 10.0) * 0.001 * scale;
	uv2.y += sin(t + uv2.x * 10.0) * 0.001 * scale;
	vec3 offset = ((texture2D(refractMap, uv2).xyz * 2.0) - 1.0) * alpha * v_color.a;
	
	gl_FragColor.rgb = texture2D(screenMap, v_position.xy - offset.xy * scale).rgb;
}

