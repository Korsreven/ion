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


//Private

/*
	Updating
*/

void MovableObject::DeriveWorldAxisAlignedBoundingBox() const noexcept
{
	world_aabb_ = aabb_;

	if (parent_node_)
		world_aabb_.Transform(parent_node_->FullTransformation());
}

void MovableObject::DeriveWorldOrientedBoundingBox() const noexcept
{
	world_obb_ = obb_;

	if (parent_node_)
		world_obb_.Transform(parent_node_->FullTransformation());
}

void MovableObject::DeriveWorldBoundingSphere() const noexcept
{
	world_sphere_ = sphere_;

	if (parent_node_)
		world_sphere_.Transform(Matrix3::Transformation(0.0_r, parent_node_->DerivedScaling(), parent_node_->DerivedPosition()));
			//Ignore derived rotation
}


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
	{
		//If a shader program is active, enable fixed-function pipeline
		auto active_program = shaders::shader_program_manager::detail::get_active_shader_program();
		if (active_program > 0)
			shaders::shader_program_manager::detail::use_shader_program(0);


		if (parent_node_)
		{
			graph::scene_graph::detail::pop_gl_matrix(); //Pop world model matrix
			graph::scene_graph::detail::push_gl_matrix(); //Restore view matrix
			graph::scene_graph::detail::mult_gl_model_view_matrix(
				Matrix4::Translation({0.0_r, 0.0_r, parent_node_->DerivedPosition().Z()})); //view * z translation matrix
		}

		//Draw bounding sphere
		if (sphere_.Radius() > 0.0_r && sphere_color_ != color::Transparent)
			WorldBoundingSphere().Draw(sphere_color_);

		//Draw oriented bounding box
		if (!obb_.Empty() && obb_color_ != color::Transparent)
			WorldOrientedBoundingBox().Draw(obb_color_);

		//Draw axis-aligned bounding box
		if (!aabb_.Empty() && aabb_color_ != color::Transparent)
			WorldAxisAlignedBoundingBox().Draw(aabb_color_);

		if (parent_node_)
		{
			graph::scene_graph::detail::pop_gl_matrix(); //Pop z translation matrix
			graph::scene_graph::detail::push_gl_matrix(); //Restore view matrix
			graph::scene_graph::detail::mult_gl_model_view_matrix(parent_node_->FullTransformation()); //view * model
		}


		//Re-enable active shader program (if any)
		if (active_program > 0)
			shaders::shader_program_manager::detail::use_shader_program(active_program);
	}
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