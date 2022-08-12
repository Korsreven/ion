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

#include "IonLight.h"
#include "graphics/IonGraphicsAPI.h"
#include "graphics/materials/IonMaterial.h"
#include "graphics/shaders/IonShaderProgram.h"
#include "graphics/shaders/IonShaderProgramManager.h"
#include "graphics/utilities/IonColor.h"
#include "query/IonSceneQuery.h"

#undef RGB

namespace ion::graphics::scene
{

using namespace model;
using namespace graphics::utilities;

namespace model::detail
{

mesh_vertex_stream::mesh_vertex_stream(shapes::mesh::VertexContainer vertex_data, render::vertex::vertex_batch::VertexDrawMode draw_mode,
	NonOwningPtr<materials::Material> material) :

	vertex_data{std::move(vertex_data)},
	vertex_batch
	{
		draw_mode,
		shapes::mesh::detail::get_vertex_declaration(),
		this->vertex_data,
		material
	}
{
	//Empty
}

bool mesh_group_comparator::operator()(const shapes::Mesh &mesh, const shapes::Mesh &mesh2) const noexcept
{
	return mesh.DrawMode() == mesh2.DrawMode() &&
		   mesh.SurfaceMaterial() == mesh2.SurfaceMaterial() &&
		   mesh.LineThickness() == mesh2.LineThickness() &&
		   mesh.ShowWireframe() == mesh2.ShowWireframe();
}


int get_mesh_vertex_count(const mesh_vertex_streams &mesh_streams) noexcept
{
	auto count = 0;
	for (auto &stream : mesh_streams)
		count += std::ssize(stream.vertex_data);
	return count;
}

mesh_vertex_stream* get_mesh_stream(const shapes::Mesh &mesh, mesh_vertex_streams &mesh_streams) noexcept
{
	for (auto &stream : mesh_streams)
	{
		if (std::find(std::begin(stream.meshes), std::end(stream.meshes), &mesh) != std::end(stream.meshes))
			return &stream;
	}

	return nullptr;
}


emissive_mesh* get_emissive_mesh(const shapes::Mesh &mesh, emissive_meshes &meshes) noexcept
{
	if (auto iter = std::find_if(std::begin(meshes), std::end(meshes),
		[&](auto &x) noexcept
		{
			return x.first == &mesh;
		}); iter != std::end(meshes))
		
		return &*iter;
	else
		return nullptr;
}

Light get_emissive_light(const shapes::Mesh &mesh) noexcept
{
	if (auto material = mesh.SurfaceMaterial(); material)
	{
		auto &sphere = mesh.BoundingSphere();
		return Light::Point({}, sphere.Center(), sphere.Radius(), material->EmissiveColor());
	}
	else
		return Light{};
}

void add_emissive_mesh(shapes::Mesh &mesh, emissive_meshes &meshes)
{
	meshes.push_back({&mesh, get_emissive_light(mesh)});
}

void remove_emissive_mesh(const shapes::Mesh &mesh, emissive_meshes &meshes) noexcept
{
	if (auto iter = std::find_if(std::begin(meshes), std::end(meshes),
		[&](auto &x) noexcept
		{
			return x.first == &mesh;
		}); iter != std::end(meshes))

		meshes.erase(iter);
}

void update_emissive_mesh(emissive_mesh &em_mesh) noexcept
{
	if (auto mesh = em_mesh.first; mesh)
		em_mesh.second = get_emissive_light(*mesh);
}

bool is_mesh_emissive(const shapes::Mesh &mesh) noexcept
{
	if (auto material = mesh.SurfaceMaterial(); material)
		return material->EmissiveColor().RGB() != color::Black.RGB();
	else
		return false;
}

} //model::detail


//Private

void Model::ReloadVertexStreams()
{
	auto vertex_count = detail::get_mesh_vertex_count(mesh_vertex_streams_);
	auto z_offset = shapes::mesh::detail::position_offset + 2;

	mesh_vertex_streams_.clear();
	emissive_meshes_.clear();

	detail::mesh_pointers meshes;
	for (auto &mesh : Meshes())
	{
		if (mesh.Visible() &&
			std::ssize(mesh.VertexData()) > z_offset)
			meshes.push_back(&mesh);
	}

	//Order all meshes by z (from first vertex)
	std::stable_sort(std::begin(meshes), std::end(meshes),
		[&](auto &mesh, auto &mesh2) noexcept
		{
			return mesh->VertexData()[z_offset] < mesh2->VertexData()[z_offset];
		});

	for (shapes::Mesh *previous_mesh = nullptr; auto &mesh : meshes)
	{
		//New vertex stream
		if (!previous_mesh ||
			!detail::mesh_group_comparator{}(*previous_mesh, *mesh))
			mesh_vertex_streams_.emplace_back(shapes::mesh::VertexContainer{}, mesh->DrawMode(), mesh->SurfaceMaterial());

		mesh_vertex_streams_.back().vertex_data.insert(std::end(mesh_vertex_streams_.back().vertex_data),
			std::begin(mesh->VertexData()), std::end(mesh->VertexData()));
		mesh_vertex_streams_.back().meshes.push_back(mesh);
		
		//Add emissive mesh
		if (detail::is_mesh_emissive(*mesh))
			detail::add_emissive_mesh(*mesh, emissive_meshes_);

		previous_mesh = mesh;
	}

	for (auto &stream : mesh_vertex_streams_)
		stream.vertex_batch.VertexData(stream.vertex_data);

	if (vertex_count < get_mesh_vertex_count(mesh_vertex_streams_))
		reload_vertex_buffer_ = true;

	update_emissive_lights_ = !std::empty(emissive_meshes_);
}

void Model::PrepareVertexStreams()
{
	for (auto &stream : mesh_vertex_streams_)
	{
		if (stream.reload_vertex_data)
		{
			stream.vertex_data.clear();
			
			for (auto &mesh : stream.meshes)
			{
				//Update vertex data
				stream.vertex_data.insert(std::end(stream.vertex_data),
					std::begin(mesh->VertexData()), std::end(mesh->VertexData()));

				//Update emissive mesh
				if (auto em_mesh = detail::get_emissive_mesh(*mesh, emissive_meshes_); em_mesh)
					detail::update_emissive_mesh(*em_mesh);
			}

			stream.vertex_batch.VertexData(stream.vertex_data);
		}
	}
}


//Protected

/*
	Events
*/

void Model::Created(shapes::Mesh &mesh) noexcept
{
	update_bounding_volumes_ |= !std::empty(mesh.VertexData());
}

void Model::Removed(shapes::Mesh &mesh) noexcept
{
	reload_vertex_streams_ = true;
	update_bounding_volumes_ = true;

	//Remove emissive mesh
	detail::remove_emissive_mesh(mesh, emissive_meshes_);
}


void Model::OpacityChanged() noexcept
{
	for (auto &mesh : Meshes())
	{
		if (auto shape = dynamic_cast<shapes::Shape*>(&mesh); shape)
			shape->Refresh();
		else
			mesh.VertexOpacity(Opacity());
	}
}


//Public

Model::Model(std::optional<std::string> name, bool visible) noexcept :
	DrawableObject{std::move(name), visible}
{
	query_type_flags_ |= query::scene_query::QueryType::Model;
}


/*
	Notifying
*/

void Model::NotifyMeshReload(const shapes::Mesh &mesh) noexcept
{
	if (mesh.Owner() == this && !reload_vertex_streams_)
	{
		if (auto stream = detail::get_mesh_stream(mesh, mesh_vertex_streams_); stream)
			stream->reload_vertex_data = true;
	}
}

void Model::NotifyMeshReloadAll(const shapes::Mesh &mesh) noexcept
{
	if (mesh.Owner() == this)
		reload_vertex_streams_ = true;
}


/*
	Rendering
*/

const movable_object::Lights& Model::EmissiveLights(bool derive) const
{
	if (derive || update_emissive_lights_)
	{
		emissive_lights_.clear();

		for (auto &[mesh, light] : emissive_meshes_)
			emissive_lights_.push_back(const_cast<Light*>(&light));

		update_emissive_lights_ = false;
	}

	return emissive_lights_;
}


/*
	Preparing / drawing
*/

void Model::Prepare() noexcept
{
	if (std::empty(Meshes()))
		return;

	//Prepare all meshes
	for (auto &mesh : Meshes())
		update_bounding_volumes_ |=
			mesh.Prepare() == shapes::mesh::MeshBoundingVolumeStatus::Changed;

	if (reload_vertex_streams_)
	{
		ReloadVertexStreams();
		reload_vertex_streams_ = false;
	}
	else
		PrepareVertexStreams();

	if (reload_vertex_buffer_)
	{
		if (!vbo_)
			vbo_.emplace(render::vertex::vertex_buffer_object::VertexBufferUsage::Static);

		if (vbo_ && *vbo_)
		{
			if (!std::empty(mesh_vertex_streams_))
			{
				auto vertex_count = detail::get_mesh_vertex_count(mesh_vertex_streams_);
				vbo_->Reserve(vertex_count * sizeof(real));

				for (auto offset = 0; auto &stream : mesh_vertex_streams_)
				{
					vertex_count = std::ssize(stream.vertex_data);
					stream.vertex_batch.VertexBuffer(vbo_->SubBuffer(offset * sizeof(real), vertex_count * sizeof(real)));
					offset += vertex_count;
				}
			}
		}

		reload_vertex_buffer_ = false;
	}

	//Prepare all mesh vertex streams
	for (auto &stream : mesh_vertex_streams_)
		stream.vertex_batch.Prepare();

	//Prepare all emissive lights
	for (auto &[mesh, light] : emissive_meshes_)
		light.Prepare();

	if (update_bounding_volumes_)
	{
		aabb_ = {};

		for (auto &mesh : Meshes())
		{
			if (mesh.IncludeBoundingVolumes())
				aabb_.Merge(mesh.AxisAlignedBoundingBox());
		}

		obb_ = aabb_;
		sphere_ = {aabb_.ToHalfSize().Max(), aabb_.Center()};

		update_bounding_volumes_ = false;
	}
}

void Model::Draw(shaders::ShaderProgram *shader_program) noexcept
{
	if (visible_ && !std::empty(mesh_vertex_streams_))
	{
		auto use_shader = shader_program && shader_program->Owner() && shader_program->Handle();
		auto shader_in_use = use_shader && shader_program->Owner()->IsShaderProgramActive(*shader_program);

		if (use_shader && !shader_in_use)
			shader_program->Owner()->ActivateShaderProgram(*shader_program);

		//Draw all meshes
		for (auto &stream : mesh_vertex_streams_)
		{
			stream.meshes.front()->DrawStarted();
			stream.vertex_batch.Draw(shader_program);
			stream.meshes.front()->DrawEnded();
		}

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

	//Elapse all mesh vertex streams
	for (auto &stream : mesh_vertex_streams_)
		stream.vertex_batch.Elapse(time);
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