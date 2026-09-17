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

Aabb particle_emitter_primitive::GetAabb() const noexcept
{
	return aabb::Zero;
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

std::tuple<Aabb, Obb, Sphere> generate_bounding_volumes(const particles::ParticleSystem &particle_system) noexcept
{
	auto aabb = aabb::Zero;

	for (auto &emitter : particle_system.Emitters())
	{
		switch (emitter.Type())
		{
			case particles::emitter::EmitterType::Point:
			aabb.Merge(emitter.Position());
			break;

			case particles::emitter::EmitterType::Box:
			case particles::emitter::EmitterType::Ring:
			aabb.Merge(Aabb::Size(emitter.Size(), emitter.Position()));
			break;
		}
	}

	return {aabb, aabb, {aabb.ToHalfSize().Max(), aabb.Center()}};
}

/*
	Rendering
*/

void apply_node_rotation(const vertex_metrics &metrics, real node_rotation, particle_emitter_primitive &primitive) noexcept
{
	primitive.TransformVertexData(
		std::tuple{size_t(metrics.rotation_offset), sizeof(particles::ParticleRenderData),
			[=](real value) { return value + node_rotation; }}
	);
}

void apply_node_scaling(const vertex_metrics &metrics, const Vector2 &node_scaling, particle_emitter_primitive &primitive) noexcept
{
	primitive.TransformVertexData(
		std::tuple{size_t(metrics.point_size_offset), sizeof(particles::ParticleRenderData),
			[=](real value) { return value * ((node_scaling.X() + node_scaling.Y()) * 0.5_r); }} //Average
	);
}

void apply_node_rotation_and_scaling(const vertex_metrics &metrics, real node_rotation, const Vector2 &node_scaling, particle_emitter_primitive &primitive) noexcept
{
	primitive.TransformVertexData(
		std::tuple{size_t(metrics.rotation_offset), sizeof(particles::ParticleRenderData),
			[=](real value) { return value + node_rotation; }},
		std::tuple{size_t(metrics.point_size_offset), sizeof(particles::ParticleRenderData),
			[=](real value) { return value * ((node_scaling.X() + node_scaling.Y()) * 0.5_r); }} //Average
	);
}


void get_emitter_primitives(const particle_emitter_batches &emitter_batches,  const vertex_metrics &metrics,
	real node_rotation, const Vector2 &node_scaling, particle_emitter_primitives &emitter_primitives)
{
	for (auto &[key, emitters] : emitter_batches)
	{
		auto total_count = std::accumulate(std::begin(emitters), std::end(emitters), 0,
			[](int count, particles::Emitter *emitter)
			{
				return count + std::min(emitter->ParticleQuota(), emitter->ParticleQuotaLimit());
			});

		auto primitive_off =
			emitter_batches.find({emitters.front()->Position().Z(), emitters.front()->ParticleMaterial().get()}) -
			std::begin(emitter_batches);
		auto &primitive = emitter_primitives[primitive_off];

		if (!primitive)
			emitter_primitives[primitive_off] = make_owning<particle_emitter_primitive>(emitters.front()->ParticleMaterial());
		else
			emitter_primitives[primitive_off]->RenderMaterial(emitters.front()->ParticleMaterial());
		
		for (auto off = 0; auto &emitter : emitters)
		{
			auto count = std::min(emitter->ParticleQuota(), emitter->ParticleQuotaLimit());
			primitive->StreamVertexData(emitter->GetParticles().RenderData(), count, total_count, off);
			off += count;
		}
	}

	//Apply both rotation and scaling from parent node
	if (node_rotation != 0.0_r && node_scaling != vector2::UnitScale)
	{
		for (auto &primitive : emitter_primitives)
			apply_node_rotation_and_scaling(metrics, node_rotation, node_scaling, *primitive);
	}

	//Apply rotation from parent node
	else if (node_rotation != 0.0_r)
	{
		for (auto &primitive : emitter_primitives)
			apply_node_rotation(metrics, node_rotation, *primitive);
	}

	//Apply scaling from parent node
	else if (node_scaling != vector2::UnitScale)
	{
		for (auto &primitive : emitter_primitives)
			apply_node_scaling(metrics, node_scaling, *primitive);
	}
}

} //drawable_particle_system::detail


//Private

void DrawableParticleSystem::ReloadPrimitives()
{
	UpdateBatches();

	if (std::size(emitter_batches_) < std::size(render_primitives_))
		render_primitives_.erase(std::begin(render_primitives_) + std::size(emitter_batches_), std::end(render_primitives_));

	emitter_primitives_.resize(std::size(emitter_batches_));

	if (particle_system_ && !std::empty(emitter_batches_))
	{
		auto parent_node = ParentNode();
		auto node_rotation = parent_node && particle_system_->InheritNodeRotation() ?
			parent_node->Rotation() : 0.0_r;
		auto node_scaling = parent_node && particle_system_->InheritNodeScaling() ?
			parent_node->Scaling() : vector2::UnitScale;

		detail::get_emitter_primitives(emitter_batches_, vertex_metrics_,
			node_rotation, node_scaling, emitter_primitives_);
	}

	for (auto &primitive : emitter_primitives_)
	{
		if (!primitive->owner)
		{
			primitive->owner = this;
			AddPrimitive(*primitive);
		}
	}
}

void DrawableParticleSystem::UpdateBatches()
{
	for (auto &batch : emitter_batches_)
		batch.second.clear(); //Keep batch capacity

	for (auto &emitter : particle_system_->Emitters())
	{
		if (emitter.HasActiveParticles())
			emitter_batches_[{emitter.Position().Z(), emitter.ParticleMaterial().get()}].push_back(&emitter);
	}

	//Remove unused batches
	emitter_batches_.erase_if(
		[](auto &batch) noexcept
		{
			return std::empty(batch.second);
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
	ReloadPrimitives(); //Always

	if (reload_primitives_)
	{
		reload_primitives_ = false;
		update_bounding_volumes_ = true;
	}

	//Prepare primitives
	for (auto &primitive : emitter_primitives_)
		primitive->Prepare();

	if (update_bounding_volumes_)
	{
		if (particle_system_)
		{
			auto [aabb, obb, sphere] =
				detail::generate_bounding_volumes(*particle_system_);
			aabb_ = aabb;
			obb_ = obb;
			sphere_ = sphere;
		}

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
		particle_system_->Elapse(time);
}

} //ion::graphics::scene