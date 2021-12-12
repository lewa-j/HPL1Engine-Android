#version 100
precision highp float;

varying vec4 v_color;//COLOR0
varying vec2 v_uv;//TEXCOORD0
varying vec3 v_lightDir;//TEXCOORD1
varying vec3 v_halfVec;//TEXCOORD2

uniform sampler2D diffuseMap;//TEXUNIT0
uniform sampler2D normalMap;//TEXUNIT1
uniform samplerCube normalCubeMap;//TEXUNIT2
uniform sampler2D falloffMap;//TEXUNIT3

uniform sampler2D specularMap;//TEXUNIT6

void main()
{
	vec3 diffuse = texture2D(diffuseMap, v_uv).rgb;
	float attenuation =	texture2D(falloffMap, vec2(dot(v_lightDir, v_lightDir), 0.5)).r;
	
	vec3 bumpVec = (2.0 * texture2D(normalMap, v_uv).xyz - 1.0);
	vec3 lightVec = normalize(v_lightDir);
	vec3 halfVec = normalize(v_halfVec);

	float specular = clamp(dot(halfVec, bumpVec), 0.0, 1.0);
	specular = pow(specular, 16.0) * v_color.w;

	gl_FragColor.rgb = diffuse * v_color.rgb * dot(lightVec, bumpVec)
					+ (specular * texture2D(specularMap, v_uv).rgb);
	gl_FragColor.rgb *= attenuation;
}

