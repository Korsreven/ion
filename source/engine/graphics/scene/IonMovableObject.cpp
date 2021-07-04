/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonMovableObject.cpp
-------------------------------------------
*/

#include "IonMovableObject.h"

#include "graph/IonSceneGraph.h"
#include "graph/IonSceneNode.h"
#include "graphics/shaders/IonShaderProgramManager.h"
#include "graphics/utilities/IonMatrix3.h"

namespace ion::graphics::scene
{

using namespace movable_object;

namespace movable_object::detail
{
} //movable_object::detail


//Protected

/*
	Bounding volumes
*/

Aabb MovableObject::DeriveWorldAxisAlignedBoundingBox(Aabb aabb) const noexcept
{
	if (parent_node_)
		aabb.Transform(parent_node_->FullTransformation());

	return aabb;
}

Obb MovableObject::DeriveWorldOrientedBoundingBox(Obb obb) const noexcept
{
	if (parent_node_)
		obb.Transform(parent_node_->FullTransformation());

	return obb;
}

Sphere MovableObject::DeriveWorldBoundingSphere(Sphere sphere) const noexcept
{
	if (parent_node_)
		sphere.Transform(Matrix3::Transformation(0.0_r, parent_node_->DerivedScaling(), parent_node_->DerivedPosition()));
			//Ignore derived rotation

	return sphere;
}


void MovableObject::DrawBoundingVolumes(const Aabb &aabb, const Obb &obb, const Sphere &sphere,
	const Color &aabb_color, const Color &obb_color, const Color &sphere_color) const noexcept
{
	if (parent_node_)
	{
		graph::scene_graph::detail::pop_gl_matrix(); //Pop world model matrix
		graph::scene_graph::detail::push_gl_matrix(); //Restore view matrix
		graph::scene_graph::detail::mult_gl_model_view_matrix(
			Matrix4::Translation({0.0_r, 0.0_r, parent_node_->DerivedPosition().Z()})); //view * z translation matrix
	}


	//Enable fixed-function pipeline
	auto active_program = shaders::shader_program_manager::detail::get_active_shader_program();
	if (active_program > 0)
		shaders::shader_program_manager::detail::use_shader_program(0);

	//Draw bounding sphere
	if (sphere.Radius() > 0.0_r && sphere_color != color::Transparent)
		sphere.Draw(sphere_color);

	//Draw oriented bounding box
	if (!obb.Empty() && obb_color != color::Transparent)
		obb.Draw(obb_color);

	//Draw axis-aligned bounding box
	if (!aabb.Empty() && aabb_color != color::Transparent)
		aabb.Draw(aabb_color);

	//Re-enable active shader program (if any)
	if (active_program > 0)
		shaders::shader_program_manager::detail::use_shader_program(active_program);


	if (parent_node_)
	{
		graph::scene_graph::detail::pop_gl_matrix(); //Pop z translation matrix
		graph::scene_graph::detail::push_gl_matrix(); //Restore view matrix
		graph::scene_graph::detail::mult_gl_model_view_matrix(parent_node_->FullTransformation()); //view * model
	}
}


//Private

/*
	Helper functions
*/

void MovableObject::Detach() noexcept
{
	if (parent_node_)
	{
		parent_node_->DetachObject(*this);
		parent_node_ = nullptr;
	}
}


//Public

MovableObject::MovableObject(bool visible) :
	visible_{visible}
{
	//Empty
}

MovableObject::MovableObject(std::string name, bool visible) :

	managed::ManagedObject<SceneManager>{std::move(name)},
	visible_{visible}
{
	//Empty
}

MovableObject::MovableObject(const MovableObject &rhs) noexcept :

	managed::ManagedObject<SceneManager>{rhs},
	parent_node_{nullptr} //A copy of a movable object has no parent node
{
	//Empty
}

MovableObject::~MovableObject() noexcept
{
	Detach();
}


/*
	Rendering
*/

void MovableObject::Render() noexcept
{
	//Optional to override

	if (show_bounding_volumes_)
		DrawBoundingVolumes(
			WorldAxisAlignedBoundingBox(), WorldOrientedBoundingBox(), WorldBoundingSphere(),
			aabb_color_, obb_color_, sphere_color_);
}

const ShaderPrograms& MovableObject::RenderPrograms([[maybe_unused]] bool derive) const
{
	//Optional to override
	return shader_programs_;
}


/*
	Elapse time
*/

void MovableObject::Elapse([[maybe_unused]] duration time) noexcept
{
	//Optional to override
}

} //ion::graphics::scene