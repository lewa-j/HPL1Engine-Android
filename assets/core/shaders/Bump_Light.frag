#version 100
precision highp float;

varying vec4 v_color;		//COLOR0
varying vec2 v_uv;			//TEXCOORD0
varying vec3 v_lightDir;	//TEXCOORD1
varying vec3 v_halfVec;		//TEXCOORD2

uniform sampler2D diffuseMap;	//TEXUNIT0
uniform sampler2D normalMap;	//TEXUNIT1
uniform sampler2D falloffMap;	//TEXUNIT3

void main()
{
	vec3 diffuse = texture2D(diffuseMap, v_uv).rgb;
	float attenuation =	texture2D(falloffMap, vec2(dot(v_lightDir, v_lightDir), 0.5)).r;

	vec3 bumpVec = (2.0 * texture2D(normalMap, v_uv) - 1.0).xyz;
	vec3 lightVec = normalize(v_lightDir);
	vec3 halfVec = normalize(v_halfVec);

	gl_FragColor.rgb = diffuse * v_color.rgb * attenuation * dot(lightVec, bumpVec);
	gl_FragColor.a = 0.0;
}

