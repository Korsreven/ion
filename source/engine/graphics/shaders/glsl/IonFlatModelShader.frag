/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/shaders/glsl
File:	IonFlatModelShader.frag
-------------------------------------------
*/

#version 330 core

struct Scene
{
	float gamma;
};

struct Primitive
{
	bool has_material;
};

struct Material
{
	vec4 diffuse;
	sampler2D diffuse_map;
	bool has_diffuse_map;
};


in vec4 vert_color;
in vec2 vert_tex_coord;

out vec4 frag_color;

uniform Scene scene;
uniform Primitive primitive;
uniform Material material;


void main()
{
	//Color
	vec4 color = vert_color;

	if (primitive.has_material)
	{
		color *= material.diffuse;

		if (material.has_diffuse_map)
			color *= texture(material.diffuse_map, vert_tex_coord);
	}
	
	
	//Gamma correction
    color = pow(color, vec4(1.0 / scene.gamma));

	//Fragment color
	frag_color = color;
}