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
#include "graphics/materials/IonMaterial.h"
#include "graphics/utilities/IonColor.h"
#include "query/IonSceneQuery.h"

#undef RGB

namespace ion::graphics::scene
{

using namespace model;
using namespace graphics::utilities;

namespace model::detail
{

void add_emissive_mesh(shapes::Mesh &mesh, emissive_meshes &meshes)
{
	meshes.emplace_back(&mesh, get_emissive_light(mesh));
}

bool remove_emissive_mesh(const shapes::Mesh &mesh, emissive_meshes &meshes) noexcept
{
	if (auto iter = std::find_if(std::begin(meshes), std::end(meshes),
		[&](auto &x) noexcept
		{
			return x.first == &mesh;
		}); iter != std::end(meshes))
	{
		meshes.erase(iter);
		return true;
	}
	else
		return false;
}

bool is_mesh_emissive(const shapes::Mesh &mesh) noexcept
{
	if (auto material = mesh.RenderMaterial(); material)
		return material->EmissiveColor().RGB() != color::Black.RGB() &&
			   material->EmissiveLightRadius().value_or(1.0_r) > 0.0_r;
	else
		return false;
}


Light get_emissive_light(const shapes::Mesh &mesh) noexcept
{
	if (auto material = mesh.RenderMaterial(); material)
	{
		auto &aabb = mesh.AxisAlignedBoundingBox();
		auto sphere_radius = aabb.ToHalfSize().Max();
		auto light_radius = material->EmissiveLightRadius().value_or(sphere_radius * 2.0_r);
			//Increase default light radius by two times the sphere radius

		return Light::Point({}, aabb.Center(), light_radius, material->EmissiveColor());
	}
	else
		return Light{};
}

} //model::detail


//Protected

/*
	Events
*/

void Model::Created(shapes::Mesh &mesh) noexcept
{
	update_bounding_volumes_ |= mesh.VertexCount() > 0;

	if (detail::is_mesh_emissive(mesh))
	{
		detail::add_emissive_mesh(mesh, emissive_meshes_);
		update_emissive_lights_ = true;
	}
	
	AddPrimitive(mesh);
}

void Model::Removed(shapes::Mesh &mesh) noexcept
{
	update_bounding_volumes_ |= mesh.IncludeBoundingVolumes();
	update_emissive_lights_ |= detail::remove_emissive_mesh(mesh, emissive_meshes_);

	RemovePrimitive(mesh);
}


//Public

Model::Model(std::optional<std::string> name, bool visible) noexcept :
	DrawableObject{std::move(name), visible}
{
	query_type_flags_ |= query::scene_query::QueryType::Model;
}

Model::~Model() noexcept
{
	for (auto &[mesh, light] : emissive_meshes_)
	{
		if (light.ParentNode())
			light.ParentNode(nullptr);
	}
}


/*
	Observers
*/

std::span<Light*> Model::EmissiveLights([[maybe_unused]] bool derive) const
{
	return emissive_lights_;
}


/*
	Notifying
*/

void Model::NotifyVertexDataChanged(shapes::Mesh &mesh) noexcept
{
	if (mesh.Owner() == this)
		update_bounding_volumes_ = true;
}

void Model::NotifyMaterialChanged(shapes::Mesh &mesh) noexcept
{
	if (mesh.Owner() == this)
	{
		update_emissive_lights_ |= detail::remove_emissive_mesh(mesh, emissive_meshes_);

		if (detail::is_mesh_emissive(mesh))
		{
			detail::add_emissive_mesh(mesh, emissive_meshes_);
			update_emissive_lights_ = true;
		}
	}
}


/*
	Preparing / drawing
*/

void Model::Prepare()
{
	//Prepare all meshes
	for (auto &mesh : Meshes())
		mesh.Prepare();

	if (update_bounding_volumes_)
	{
		aabb_ = {};

		//Merge all bounding boxes
		for (auto &mesh : Meshes())
		{
			if (mesh.IncludeBoundingVolumes())
				aabb_.Merge(mesh.AxisAlignedBoundingBox());
		}

		obb_ = aabb_;
		sphere_ = {aabb_.ToHalfSize().Max(), aabb_.Center()};

		update_bounding_volumes_ = false;
	}

	if (update_emissive_lights_)
	{
		emissive_lights_.clear();

		//Update all emissive lights
		for (auto &[mesh, light] : emissive_meshes_)
		{
			light = detail::get_emissive_light(*mesh);
			light.Prepare();

			auto &mutable_light = const_cast<Light&>(light);
			mutable_light.ParentNode(ParentNode());
			emissive_lights_.push_back(&mutable_light);
		}

		update_emissive_lights_ = false;
	}

	DrawableObject::Prepare();
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


NonOwningPtr<shapes::Mesh> Model::CreateMesh(render::render_primitive::VertexContainer vertex_data, bool visible)
{
	return Create(std::move(vertex_data), visible);
}

NonOwningPtr<shapes::Mesh> Model::CreateMesh(render::render_primitive::VertexContainer vertex_data, NonOwningPtr<materials::Material> material,
	shapes::mesh::MeshTexCoordMode tex_coord_mode, bool visible)
{
	return Create(std::move(vertex_data), material, tex_coord_mode, visible);
}

NonOwningPtr<shapes::Mesh> Model::CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, render::render_primitive::VertexContainer vertex_data, bool visible)
{
	return Create(draw_mode, std::move(vertex_data), visible);
}

NonOwningPtr<shapes::Mesh> Model::CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, render::render_primitive::VertexContainer vertex_data, NonOwningPtr<materials::Material> material,
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
	render_primitives_.clear();
	render_primitives_.shrink_to_fit();
	emissive_lights_.clear();
	emissive_lights_.shrink_to_fit();

	emissive_meshes_.clear();
	emissive_meshes_.shrink_to_fit();

	Clear();
}

bool Model::RemoveMesh(shapes::Mesh &mesh) noexcept
{
	return Remove(mesh);
}

} //ion::graphics::scene