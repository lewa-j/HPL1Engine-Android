#version 100
precision highp float;

varying vec4 v_color;//COLOR0
varying vec2 v_uv;//TEXCOORD0
varying vec3 v_lightDir;//TEXCOORD1
varying vec4 v_spotlightUv;//TEXCOORD2

uniform sampler2D diffuseMap;//TEXUNIT0
uniform samplerCube normalCubeMap;//TEXUNIT2,
uniform sampler2D falloffMap;//TEXUNIT3,
uniform sampler2D spotlightMap;//TEXUNIT4,
uniform sampler2D spotNegRejectMap;//TEXUNIT5,

void main()
{
	vec3 diffuse = texture2D(diffuseMap, v_uv).rgb;
	vec3 lightVec = (2.0 * textureCube(normalCubeMap, v_lightDir).xyz - 1.0);
	float attenuation =	texture2D(falloffMap, vec2(dot(v_lightDir, v_lightDir), 0.0)).r;
	vec3 spotColor = texture2DProj(spotlightMap, (v_spotlightUv.xyz)).rgb;
	float rejectNeg = texture2D(spotNegRejectMap, vec2((v_spotlightUv.z) + 0.5, 0.5)).r;

	gl_FragColor.rgb = diffuse * dot(lightVec, vec3(0.0,0.0,1.0)) * attenuation *
					v_color.rgb * spotColor * rejectNeg;
}

