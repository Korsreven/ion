/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonModel.cpp
-------------------------------------------
*/

#include "IonModel.h"

#include "graphics/IonGraphicsAPI.h"

namespace ion::graphics::scene
{

using namespace model;

namespace model::detail
{

int model_buffer_usage_to_gl_buffer_usage(ModelBufferUsage buffer_usage) noexcept
{
	switch (buffer_usage)
	{
		case ModelBufferUsage::Dynamic:
		return GL_DYNAMIC_DRAW;

		case ModelBufferUsage::Stream:
		return GL_STREAM_DRAW;

		case ModelBufferUsage::Static:
		default:
		return GL_STATIC_DRAW;
	}
}


std::optional<int> create_vertex_buffer_object() noexcept
{
	auto handle = 0;

	switch (gl::VertexBufferObject_Support())
	{
		case gl::Extension::Core:
		glGenBuffers(1, reinterpret_cast<unsigned int*>(&handle));
		break;

		case gl::Extension::ARB:
		glGenBuffersARB(1, reinterpret_cast<unsigned int*>(&handle));
		break;
	}

	if (handle > 0)
		return handle;
	else
		return {};
}

void delete_vertex_buffer_object(int vbo_handle) noexcept
{
	switch (gl::VertexBufferObject_Support())
	{
		case gl::Extension::Core:
		glDeleteBuffers(1, reinterpret_cast<unsigned int*>(&vbo_handle));
		break;

		case gl::Extension::ARB:
		glDeleteBuffersARB(1, reinterpret_cast<unsigned int*>(&vbo_handle));
		break;
	}
}


void set_vertex_buffer_data(int vbo_handle, ModelBufferUsage buffer_usage,
	const render::mesh::detail::vertex_storage_type &vertex_buffer) noexcept
{
	render::mesh::detail::bind_vertex_buffer_object(vbo_handle);

	switch (gl::VertexBufferObject_Support())
	{
		case gl::Extension::Core:
		glBufferData(GL_ARRAY_BUFFER, std::size(vertex_buffer) * sizeof(real),
			std::data(vertex_buffer), model_buffer_usage_to_gl_buffer_usage(buffer_usage));
		break;

		case gl::Extension::ARB:
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, std::size(vertex_buffer) * sizeof(real),
			std::data(vertex_buffer), model_buffer_usage_to_gl_buffer_usage(buffer_usage));
		break;
	}

	render::mesh::detail::bind_vertex_buffer_object(0);
}

int get_vertex_buffer_size(int vbo_handle) noexcept
{
	auto size = 0;
	render::mesh::detail::bind_vertex_buffer_object(vbo_handle);

	switch (gl::VertexBufferObject_Support())
	{
		case gl::Extension::Core:
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		break;

		case gl::Extension::ARB:
		glGetBufferParameterivARB(GL_ARRAY_BUFFER_ARB, GL_BUFFER_SIZE_ARB, &size);
		break;
	}

	render::mesh::detail::bind_vertex_buffer_object(0);
	return size / sizeof(real);
}

} //model::detail


//Private

void Model::FetchVertexData()
{
	auto size = 0;
	for (auto &mesh : Meshes())
		size += render::mesh::detail::vertex_data_size(mesh.VertexCount());

	vertex_buffer_.reserve(size);

	//Copy and append vertex data from each mesh
	for (auto &mesh : Meshes())
		vertex_buffer_.insert(std::end(vertex_buffer_), std::begin(mesh.VertexData()), std::end(mesh.VertexData()));
}


//Public

Model::Model() :
	vbo_handle_{detail::create_vertex_buffer_object()}
{
	//Empty
}

Model::Model(model::ModelBufferUsage buffer_usage, bool visible) :

	buffer_usage_{buffer_usage},
	visible_{visible},

	vbo_handle_{detail::create_vertex_buffer_object()}
{
	//Empty
}

Model::~Model() noexcept
{
	if (vbo_handle_)
		detail::delete_vertex_buffer_object(*vbo_handle_);
}
			

/*
	Modifiers
*/




/*
	Observers
*/




/*
	Drawing
*/

void Model::Prepare() noexcept
{
	if (reload_vertex_buffer_)
	{
		if (vbo_handle_)
		{
			if (std::empty(vertex_buffer_))
				FetchVertexData();

			if (!std::empty(vertex_buffer_))
			{
				//Buffer size is too small, replace with new buffer
				if (detail::get_vertex_buffer_size(*vbo_handle_) < std::ssize(vertex_buffer_))
					detail::set_vertex_buffer_data(*vbo_handle_, buffer_usage_, vertex_buffer_); //Send to VRAM
				//Use same buffer
				else
					render::mesh::detail::set_vertex_buffer_sub_data(*vbo_handle_, 0, vertex_buffer_); //Send to VRAM

				//Vertex data has been loaded to VRAM, clear RAM
				vertex_buffer_.clear();
				vertex_buffer_.shrink_to_fit();

				for (auto offset = 0; auto &mesh : Meshes())
				{
					mesh.VboHandle(vbo_handle_, offset);
					offset += std::ssize(mesh.VertexData());
				}
			}
		}

		reload_vertex_buffer_ = false;
	}

	for (auto &mesh : Meshes())
		mesh.Prepare();
}

void Model::Draw(shaders::ShaderProgram *shader_program) noexcept
{
	for (auto &mesh : Meshes())
		mesh.Draw(shader_program);
}


/*
	Meshes
	Creating
*/

render::Mesh& Model::CreateMesh(const render::mesh::Vertices &vertices)
{
	reload_vertex_buffer_ = !std::empty(vertices);
	return Create(vbo_handle_, std::ssize(vertex_buffer_), vertices);
}

render::Mesh& Model::CreateMesh(render::mesh::detail::vertex_storage_type vertex_data)
{
	reload_vertex_buffer_ = !std::empty(vertex_data);
	return Create(vbo_handle_, std::ssize(vertex_buffer_), std::move(vertex_data));
}


render::Mesh& Model::CreateMesh(const render::Mesh &mesh)
{
	reload_vertex_buffer_ = mesh.VertexCount() > 0;
	return Create(mesh);
}

render::Mesh& Model::CreateMesh(render::Mesh &&mesh)
{
	reload_vertex_buffer_ = mesh.VertexCount() > 0;
	return Create(std::move(mesh));
}


/*
	Meshes
	Removing
*/

void Model::ClearMeshes() noexcept
{
	Clear();
}

bool Model::RemoveMesh(render::Mesh &mesh) noexcept
{
	return Remove(mesh);
}

} //ion::graphics::scene