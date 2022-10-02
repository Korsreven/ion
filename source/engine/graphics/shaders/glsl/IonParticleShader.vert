/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/shaders/glsl
File:	IonParticleShader.vert
-------------------------------------------
*/

#version 330 core

struct Matrix
{
    mat4 model_view;
    mat4 model_view_projection;
    mat3 normal;
};

struct Camera
{
    vec3 position;
    float rotation;
};


layout (location = 0) in vec3 vertex_position;
layout (location = 1) in float vertex_rotation;
layout (location = 2) in float vertex_point_size;
layout (location = 3) in vec4 vertex_color;

out vec3 vert_position;
out vec4 vert_color;
out mat3 normal_matrix;
out mat2 rotation_matrix;

uniform Matrix matrix;
uniform Camera camera;


mat2 angle_to_rotation_mat(float angle)
{
    float sin_of_angle = sin(angle);
	float cos_of_angle = cos(angle);
    return mat2( cos_of_angle, sin_of_angle,
                -sin_of_angle, cos_of_angle);
}


void main()
{
    vert_position = (matrix.model_view * vec4(vertex_position, 1.0)).xyz;
    vert_color = vertex_color;
    normal_matrix = matrix.normal;
    rotation_matrix = angle_to_rotation_mat(camera.rotation) * angle_to_rotation_mat(vertex_rotation);
    
    gl_Position = matrix.model_view_projection * vec4(vertex_position, 1.0);
    gl_PointSize = vertex_point_size;
}