/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonDrawableParticleSystem.cpp
-------------------------------------------
*/

#include "IonDrawableParticleSystem.h"

#include "query/IonSceneQuery.h"

namespace ion::graphics::scene
{

using namespace drawable_particle_system;
using namespace types::type_literals;

namespace drawable_particle_system::detail
{

//Protected

/*
	Events
*/

void particle_emitter_primitive::RenderPassesChanged() noexcept
{
	if (owner)
		owner->NotifyRenderPassesChanged(*this);
}


//Public

particle_emitter_primitive::particle_emitter_primitive(NonOwningPtr<materials::Material> particle_material) :
	render::RenderPrimitive{render::vertex::vertex_batch::VertexDrawMode::Points, get_vertex_declaration()}
{
	RenderMaterial(particle_material);
	PointSprite(true);
}


vertex_metrics get_vertex_metrics(const render::vertex::VertexDeclaration &vertex_declaration) noexcept
{
	auto metrics = vertex_metrics{};

	for (auto &element : vertex_declaration.Elements())
	{
		if (element.Name == shaders::shader_layout::AttributeName::Vertex_Rotation)
			metrics.rotation_offset = element.Offset / static_cast<int>(sizeof(real));
		else if (element.Name == shaders::shader_layout::AttributeName::Vertex_PointSize)
			metrics.point_size_offset = element.Offset / static_cast<int>(sizeof(real));
	}

	return metrics;
}

/*
	Rendering
*/

void apply_node_rotation(const vertex_metrics &metrics, real node_rotation, render::render_primitive::VertexContainer &data) noexcept
{
	auto size = std::ssize(data);
	auto stride = sizeof(particles::Particle);

	for (auto off = metrics.rotation_offset; off < size; off += stride)
		data[off] += node_rotation;
}

void apply_node_scaling(const vertex_metrics &metrics, const Vector2 &node_scaling, render::render_primitive::VertexContainer &data) noexcept
{
	auto size = std::ssize(data);
	auto stride = sizeof(particles::Particle);

	for (auto off = metrics.point_size_offset; off < size; off += stride)
		data[off] *= (node_scaling.X() + node_scaling.Y()) * 0.5_r; //Average
}


void get_emitter_primitives(const particles::ParticleSystem &particle_system,  const vertex_metrics &metrics,
	real node_rotation, const Vector2 &node_scaling, particle_emitter_primitives &emitter_primitives)
{
	for (auto off = 0; auto &emitter : particle_system.Emitters())
	{
		if (!emitter.HasActiveParticles())
			continue;

		auto &primitive =
			[&]() noexcept -> particle_emitter_primitive&
			{
				if (off < std::ssize(emitter_primitives))
				{
					emitter_primitives[off]->RenderMaterial(emitter.ParticleMaterial());
					return *emitter_primitives[off];
				}
				else
					return *emitter_primitives.emplace_back(make_owning<particle_emitter_primitive>(emitter.ParticleMaterial()));
			}();

		auto size = std::size(emitter.Particles()) * sizeof(particles::Particle);
		primitive.vertex_data.resize(size / sizeof(real), 0.0_r);
		std::memcpy(std::data(primitive.vertex_data), std::data(emitter.Particles()), size);

		if (node_rotation != 0.0_r)
			apply_node_rotation(metrics, node_rotation, primitive.vertex_data);

		if (node_scaling != vector2::UnitScale)
			apply_node_scaling(metrics, node_scaling, primitive.vertex_data);

		++off;
	}
}

} //drawable_particle_system::detail


//Private

void DrawableParticleSystem::ReloadPrimitives()
{
	render_primitives_.clear();

	if (particle_system_)
	{
		auto parent_node = ParentNode();
		auto node_rotation = parent_node ? parent_node->Rotation() : 0.0_r;
		auto node_scaling = parent_node ? parent_node->Scaling() : vector2::UnitScale;

		detail::get_emitter_primitives(*particle_system_, vertex_metrics_,
			node_rotation, node_scaling, emitter_primitives_);
	}

	std::erase_if(emitter_primitives_,
		[&](auto &primitive) noexcept
		{
			if (!std::empty(primitive->vertex_data))
			{
				primitive->owner = this;
				primitive->VertexData(std::move(primitive->vertex_data));
				AddPrimitive(*primitive);		
				return false; //Keep
			}
			else
				return true;
		});
}

//Public

DrawableParticleSystem::DrawableParticleSystem(std::optional<std::string> name,
	NonOwningPtr<particles::ParticleSystem> particle_system, bool visible) :
	
	DrawableObject{std::move(name), visible},
	particle_system_{particle_system ? std::make_optional(particle_system->Clone()) : std::nullopt},
	initial_particle_system_{particle_system},

	vertex_metrics_{detail::get_vertex_metrics(detail::get_vertex_declaration())},
	reload_primitives_{!!particle_system_}
{
	query_type_flags_ |= query::scene_query::QueryType::ParticleSystem;
}


/*
	Modifiers
*/

void DrawableParticleSystem::Revert()
{
	if (initial_particle_system_)
	{
		particle_system_ = initial_particle_system_->Clone();
		reload_primitives_ = true;
	}
}


/*
	Preparing/drawing
*/

void DrawableParticleSystem::Prepare()
{
	if (reload_primitives_)
	{
		ReloadPrimitives();
		reload_primitives_ = false;
		update_bounding_volumes_ = true;
	}

	//Prepare primitives
	for (auto &primitive : emitter_primitives_)
		primitive->Prepare();

	if (update_bounding_volumes_)
	{
		aabb_ = {};

		//Merge all bounding boxes
		for (auto &primitive : emitter_primitives_)
			aabb_.Merge(primitive->AxisAlignedBoundingBox());

		obb_ = aabb_;
		sphere_ = {aabb_.ToHalfSize().Max(), aabb_.Center()};

		update_bounding_volumes_ = false;
	}

	DrawableObject::Prepare();
}


/*
	Elapse time
*/

void DrawableParticleSystem::Elapse(duration time) noexcept
{
	if (particle_system_)
	{
		particle_system_->Elapse(time);
		reload_primitives_ = true;
	}
}

} //ion::graphics::scene