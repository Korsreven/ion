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
#include "graphics/shaders/IonShaderProgram.h"
#include "graphics/shaders/IonShaderProgramManager.h"
#include "query/IonSceneQuery.h"

namespace ion::graphics::scene
{

using namespace model;

namespace model::detail
{
} //model::detail


//Private

void Model::Created(shapes::Mesh &mesh) noexcept
{
	reload_vertex_buffer_ |= mesh.VertexCount() > 0;
	update_bounding_volumes_ |= reload_vertex_buffer_;
}

void Model::Removed(shapes::Mesh&) noexcept
{
	update_bounding_volumes_ = true;
}


//Public

Model::Model(bool visible) noexcept :
	DrawableObject{visible}
{
	query_type_flags_ |= query::scene_query::QueryType::Model;
}


/*
	Modifiers
*/

void Model::Opacity(real percent) noexcept
{
	if (opacity_ != percent)
	{
		opacity_ = percent;

		for (auto &mesh : Meshes())
		{
			if (auto shape = dynamic_cast<shapes::Shape*>(&mesh); shape)
				shape->Refresh();
			else
				mesh.VertexOpacity(percent);
		}
	}
}


/*
	Preparing / drawing
*/

void Model::Prepare() noexcept
{
	if (reload_vertex_buffer_)
	{
		if (!vbo_)
			vbo_.emplace(render::vertex::vertex_buffer_object::VertexBufferUsage::Static);

		if (vbo_ && *vbo_)
		{
			auto size = 0;
			for (auto &mesh : Meshes())
				size += std::ssize(mesh.VertexData());

			vbo_->Reserve(size * sizeof(real));

			for (auto offset = 0; auto &mesh : Meshes())
			{
				size = std::ssize(mesh.VertexData());
				mesh.VertexBuffer(vbo_->SubBuffer(offset * sizeof(real), size * sizeof(real)));
				offset += size;
			}
		}

		reload_vertex_buffer_ = false;
	}


	//Prepare all meshes
	for (auto &mesh : Meshes())
		update_bounding_volumes_ |=
			mesh.Prepare() == shapes::mesh::MeshBoundingVolumeStatus::Changed;

	if (update_bounding_volumes_)
	{
		aabb_ = {};

		for (auto &mesh : Meshes())
			aabb_.Merge(mesh.AxisAlignedBoundingBox());

		obb_ = aabb_;
		sphere_ = {aabb_.ToHalfSize().Max(), aabb_.Center()};

		update_bounding_volumes_ = false;
	}
}

void Model::Draw(shaders::ShaderProgram *shader_program) noexcept
{
	if (auto meshes = Meshes(); visible_ && !std::empty(meshes))
	{
		auto use_shader = shader_program && shader_program->Owner() && shader_program->Handle();
		auto shader_in_use = use_shader && shader_program->Owner()->IsShaderProgramActive(*shader_program);

		if (use_shader && !shader_in_use)
			shader_program->Owner()->ActivateShaderProgram(*shader_program);

		//Draw all meshes
		for (auto &mesh : meshes)
			mesh.Draw(shader_program);

		if (use_shader && !shader_in_use)
			shader_program->Owner()->DeactivateShaderProgram(*shader_program);
	}
}


/*
	Elapse time
*/

void Model::Elapse(duration time) noexcept
{
	//Elapse all meshes
	for (auto &mesh : Meshes())
		mesh.Elapse(time);
}


/*
	Meshes
	Creating
*/

NonOwningPtr<shapes::Mesh> Model::CreateMesh(const shapes::mesh::Vertices &vertices, bool visible)
{
	return Create(vertices, visible);
}

NonOwningPtr<shapes::Mesh> Model::CreateMesh(const shapes::mesh::Vertices &vertices, NonOwningPtr<materials::Material> material,
	shapes::mesh::MeshTexCoordMode tex_coord_mode, bool visible)
{
	return Create(vertices, material, tex_coord_mode, visible);
}

NonOwningPtr<shapes::Mesh> Model::CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, const shapes::mesh::Vertices &vertices, bool visible)
{
	return Create(draw_mode, vertices, visible);
}

NonOwningPtr<shapes::Mesh> Model::CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, const shapes::mesh::Vertices &vertices, NonOwningPtr<materials::Material> material,
	shapes::mesh::MeshTexCoordMode tex_coord_mode, bool visible)
{
	return Create(draw_mode, vertices, material, tex_coord_mode, visible);
}


NonOwningPtr<shapes::Mesh> Model::CreateMesh(shapes::mesh::VertexContainer vertex_data, bool visible)
{
	return Create(std::move(vertex_data), visible);
}

NonOwningPtr<shapes::Mesh> Model::CreateMesh(shapes::mesh::VertexContainer vertex_data, NonOwningPtr<materials::Material> material,
	shapes::mesh::MeshTexCoordMode tex_coord_mode, bool visible)
{
	return Create(std::move(vertex_data), material, tex_coord_mode, visible);
}

NonOwningPtr<shapes::Mesh> Model::CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, shapes::mesh::VertexContainer vertex_data, bool visible)
{
	return Create(draw_mode, std::move(vertex_data), visible);
}

NonOwningPtr<shapes::Mesh> Model::CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, shapes::mesh::VertexContainer vertex_data, NonOwningPtr<materials::Material> material,
	shapes::mesh::MeshTexCoordMode tex_coord_mode, bool visible)
{
	return Create(draw_mode, std::move(vertex_data), material, tex_coord_mode, visible);
}


NonOwningPtr<shapes::Mesh> Model::CreateMesh(const shapes::Mesh &mesh)
{
	return Create(mesh);
}

NonOwningPtr<shapes::Mesh> Model::CreateMesh(shapes::Mesh &&mesh)
{
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

bool Model::RemoveMesh(shapes::Mesh &mesh) noexcept
{
	return Remove(mesh);
}

} //ion::graphics::scene