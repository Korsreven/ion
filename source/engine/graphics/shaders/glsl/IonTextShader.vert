/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/shaders/glsl
File:	IonTextShader.vert
-------------------------------------------
*/

#version 330 core

struct Matrix
{
    mat4 model_view;
    mat4 projection;
    mat4 model_view_projection;
};


layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec4 vertex_color;
layout (location = 2) in vec2 vertex_tex_coord;

out vec3 vert_position;
out vec4 vert_color;
out vec2 vert_tex_coord;

uniform Matrix matrix;


void main()
{
    vert_position = (matrix.model_view * vec4(vertex_position, 1.0)).xyz;
    vert_color = vertex_color;
    vert_tex_coord = vertex_tex_coord;

    gl_Position = matrix.model_view_projection * vec4(vertex_position, 1.0);
}