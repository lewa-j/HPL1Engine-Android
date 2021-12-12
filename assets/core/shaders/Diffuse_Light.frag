#version 100
precision highp float;

varying vec4 v_color;//COLOR0
varying vec2 v_uv;//TEXCOORD0
varying vec3 v_lightDir;//TEXCOORD1

uniform sampler2D diffuseMap;//TEXUNIT0
uniform samplerCube normalCubeMap;//TEXUNIT2
uniform sampler2D falloffMap;//TEXUNIT3

void main()
{
	float attenuation =	texture2D(falloffMap, vec2(dot(v_lightDir, v_lightDir), 0.0)).r;
	vec3 diffuse = texture2D(diffuseMap, v_uv).rgb;
	vec3 lightVec = (2.0 * textureCube(normalCubeMap, v_lightDir) - 1.0).xyz;
	gl_FragColor.rgb = diffuse * dot(lightVec, vec3(0.0, 0.0, 1.0)) * attenuation * v_color.rgb;
	gl_FragColor.a = 0.0;
}

