/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/shaders/glsl
File:	IonFlatTextShader.frag
-------------------------------------------
*/

#version 330 core

struct Scene
{
	float gamma;
};

struct Primitive
{
	sampler2DArray texture;
	bool has_texture;
};


in vec4 vert_color;
in vec2 vert_tex_coord;
flat in int vert_tex_layer;

out vec4 frag_color;

uniform Scene scene;
uniform Primitive primitive;


void main()
{
	//Color
	vec4 color = vert_color;

	if (primitive.has_texture)
		color *= texture(primitive.texture, vec3(vert_tex_coord, vert_tex_layer));
	
	
	//Gamma correction
    color = pow(color, vec4(1.0 / scene.gamma));

	//Fragment color
	frag_color = color;
}