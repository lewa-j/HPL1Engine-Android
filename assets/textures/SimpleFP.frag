#version 100
precision highp float;

//This program is run once for every screen pixel that is drawn. 
//Normally, this is all the front facing polygons in a model (the vertex buffer)
//The values are interpolated values pased on the outputs of the vertex program
//so if vertex 0 as screenpos of (0,0) and vertex 1 has screen pos of (2,5)
//then color, uv or any other output value will be interpolated from (0,0) -> (2,5)

varying vec2 v_uv;//TEXCOORD0 //The interpolated uv coord
varying vec4 v_color;//COLOR0 //The interpolated color value

uniform sampler2D diffuseMap;//TEXUNIT0 //The texture to be used from texture unit 0
uniform float timeCount; 				//A user variable

void main()
{
	vec2 uv = v_uv;
	//Do some silly calculations using the timeCount varaible
	//so that the uv coord is warped 
	//This is "unimporanted" and simply making the cg do something visible.
	for (int i = 0; i < 2; i++)
	{
		uv.y += sin(timeCount * 2.0 + uv.x * 10.0) * 0.1;
		uv.x += sin(timeCount * 2.0 + uv.y * 10.0) * 0.1;
	}

	gl_FragColor = texture2D(diffuseMap, uv) * //Get the texel color some texture at postion uv
				v_color ;//*					//The interpolated color value
				//(sin(uv.y)+1.0);			//A silly calculation (can skip it, just showing of cg)
											//This line also seem to mess up on HD cards

}
