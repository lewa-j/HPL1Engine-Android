#version 100
precision highp float;

varying vec4 v_color;		//COLOR0
varying vec2 v_uv;			//TEXCOORD0
varying vec3 v_lightDir;	//TEXCOORD1
varying vec3 v_halfVec;		//TEXCOORD2

uniform sampler2D diffuseMap;		//TEXUNIT0
uniform sampler2D normalMap;		//TEXUNIT1
uniform sampler2D falloffMap;		//TEXUNIT3

void main()
{
	vec3 diffuse = texture2D(diffuseMap, v_uv).rgb;
	
	vec4 bumpVec = texture2D(normalMap, v_uv);
	bumpVec.xyz = (2.0 * bumpVec.xyz) - 1.0;
	vec3 lightVec = normalize(v_lightDir);
	vec3 halfVec = normalize(v_halfVec);
    
	float attenuation =	texture2D(falloffMap, vec2(dot(v_lightDir, v_lightDir), 0.5)).r;
	
	float specular = clamp(dot(halfVec, bumpVec.xyz), 0.0, 1.0);
	specular = pow(specular, 16.0) * v_color.w * bumpVec.w;
	
	gl_FragColor.rgb = diffuse * dot(lightVec, bumpVec.xyz) * v_color.rgb + specular;
	gl_FragColor.rgb *= attenuation;
}

