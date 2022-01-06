#version 100
precision highp float;

varying vec4 v_color;		//COLOR0
varying vec2 v_uv;			//TEXCOORD0
varying vec3 v_lightDir;	//TEXCOORD1
varying vec3 v_halfVec;		//TEXCOORD2
varying vec4 v_spotlightUv;	//TEXCOORD3

uniform sampler2D diffuseMap;		//TEXUNIT0
uniform sampler2D normalMap;		//TEXUNIT1
uniform sampler2D falloffMap;		//TEXUNIT3
uniform sampler2D spotlightMap;		//TEXUNIT4
uniform sampler2D spotNegRejectMap;	//TEXUNIT5

void main()
{
	vec3 diffuse = texture2D(diffuseMap, v_uv).rgb;

	vec4 bumpVec = texture2D(normalMap, v_uv);
	bumpVec.xyz = (2.0 * bumpVec.xyz) - 1.0;

	vec3 lightVec = normalize(v_lightDir);
   
	float attenuation =	texture2D(falloffMap, vec2(dot(v_lightDir, v_lightDir), 0.5)).r;
	vec4 spotColor = texture2DProj(spotlightMap, v_spotlightUv.xyz);
	float rejectNeg = texture2D(spotNegRejectMap, vec2(v_spotlightUv.z + 0.5, 0.5)).r;

	float specular = clamp(dot(normalize(v_halfVec), bumpVec.xyz), 0.0, 1.0);
	specular = pow(specular, 16.0) * v_color.a * bumpVec.w * spotColor.a;

	gl_FragColor.rgb = diffuse * dot(lightVec, bumpVec.xyz) * v_color.rgb * spotColor.rgb + specular;
	gl_FragColor.rgb *= attenuation * rejectNeg;
}
