#version 100
precision highp float;

// Basically, what this program does is that it tranforms a vertice,
// a vertice is a postion, normal, tex coords, etc.
// So the program below is run one time for every vertice in the vertex buffer

attribute vec4 a_position;	//POSITION	//This is the postion in vertex
attribute vec4 a_color;		//COLOR		//The color
attribute vec2 a_uv;		//TEXCOORD0	//Texture coord 0

varying vec4 v_position;	//POSITION	//The transformed postion (the postion it will in screen space)
varying vec2 v_uv;			//TEXCOORD0	//The transformed uv
varying vec4 v_color;		//COLOR		//The "transformed" color

uniform mat4 worldViewProj;				//This is the view-projection matrix which is basically a 
										//multiplication of:
										// Model matrix
										// View (camera) matrix
										// Projection matrix (3d -> 2d)

void main()
{
	v_position = worldViewProj * a_position;	// Transform the incoming vertex postion to screen space
	gl_Position = v_position;
	v_uv = a_uv;		//Simply just pass on the uv, do no transforming	
	v_color = a_color;	//Simply just pass on the color
}
