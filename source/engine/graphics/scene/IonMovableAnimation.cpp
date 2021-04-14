/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonMovableAnimation.cpp
-------------------------------------------
*/

#include "IonMovableAnimation.h"

namespace ion::graphics::scene
{

using namespace movable_animation;
using namespace utilities;

namespace movable_animation::detail
{

animation_vertex_stream::animation_vertex_stream() :

	vertex_batch
	{
		render::vertex::vertex_batch::VertexDrawMode::Triangles,
		get_vertex_declaration()
	}
{
	//Empty
}

vertex_container get_animation_vertex_data(textures::Animation &animation,
	const Vector3 &position, real rotation, const Vector2 &size, const Color &color)
{
	auto [half_width, half_height] = (size * 0.5_r).XY();
	auto [r, g, b, a] = color.RGBA();

	auto v1 = (position + Vector2{-half_width, half_height}).RotateCopy(rotation, position);
	auto v2 = (position + Vector2{-half_width, -half_height}).RotateCopy(rotation, position);
	auto v3 = (position + Vector2{half_width, -half_height}).RotateCopy(rotation, position);
	auto v4 = (position + Vector2{half_width, half_height}).RotateCopy(rotation, position);

	auto first_frame =
		animation.UnderlyingFrameSequence()->FirstFrame();
	auto tex_coords = first_frame ?
		first_frame->TexCoords().value_or(std::pair{vector2::Zero, vector2::UnitScale}) :
		std::pair{vector2::Zero, vector2::UnitScale};

	auto [ll_s, ll_t] = tex_coords.first.XY();
	auto [ur_s, ur_t] = tex_coords.second.XY();

	//Vertex format:
	//x, y, z
	//r, g, b, a
	//s, t

	return
		{
			//Vertex #1
			v1.X(), v1.Y(), v1.Z(),
			r, g, b, a,
			ll_s, ur_t,

			//Vertex #2
			v2.X(), v2.Y(), v2.Z(),
			r, g, b, a,
			ll_s, ll_t,

			//Vertex #3
			v3.X(), v3.Y(), v3.Z(),
			r, g, b, a,
			ur_s, ll_t,

			//Vertex #4
			v3.X(), v3.Y(), v3.Z(),
			r, g, b, a,
			ur_s, ll_t,

			//Vertex #5
			v4.X(), v4.Y(), v4.Z(),
			r, g, b, a,
			ur_s, ur_t,

			//Vertex #6
			v1.X(), v1.Y(), v1.Z(),
			r, g, b, a,
			ll_s, ur_t
		};
}

} //movable_animation::detail


//Private

void MovableAnimation::PrepareVertexStream()
{
	if (!vbo_)
		reload_vertex_buffer_ = true;

	vertex_stream_.vertex_data = detail::get_animation_vertex_data(*animation_, position_, rotation_, size_, color_);
	vertex_stream_.vertex_batch.VertexData(vertex_stream_.vertex_data);
}

//Public

MovableAnimation::MovableAnimation(const Vector2 &size, NonOwningPtr<textures::Animation> animation, bool visible) :
	MovableAnimation{vector3::Zero, size, animation, visible}
{
	//Empty
}

MovableAnimation::MovableAnimation(const Vector3 &position, const Vector2 &size, NonOwningPtr<textures::Animation> animation, bool visible) :
	MovableAnimation{position, 0.0_r, size, animation, visible}
{
	//Empty
}

MovableAnimation::MovableAnimation(const Vector3 &position, real rotation, const Vector2 &size, NonOwningPtr<textures::Animation> animation, bool visible) :
	MovableAnimation{position, rotation, size, animation, color::White, visible}
{
	//Empty
}


MovableAnimation::MovableAnimation(const Vector2 &size, NonOwningPtr<textures::Animation> animation, const Color &color, bool visible) :
	MovableAnimation{vector3::Zero, size, animation, color, visible}
{
	//Empty
}

MovableAnimation::MovableAnimation(const Vector3 &position, const Vector2 &size, NonOwningPtr<textures::Animation> animation, const Color &color, bool visible) :
	MovableAnimation{position, 0.0_r, size, animation, color, visible}
{
	//Empty
}

MovableAnimation::MovableAnimation(const Vector3 &position, real rotation, const Vector2 &size, NonOwningPtr<textures::Animation> animation, const Color &color, bool visible) :
	
	MovableObject{visible},

	position_{position},
	rotation_{rotation},
	size_{size},
	color_{color},

	animation_{animation ? std::make_optional(*animation) : std::nullopt},
	initial_animation_{animation},

	reload_vertex_stream_{!!animation}
{
	//Empty
}


/*
	Modifiers
*/

void MovableAnimation::Revert()
{
	if (initial_animation_)
		animation_ = *initial_animation_;
}


/*
	Preparing / drawing
*/

void MovableAnimation::Prepare() noexcept
{
	if (!animation_)
		return;

	if (reload_vertex_stream_)
	{
		PrepareVertexStream();
		reload_vertex_stream_ = false;
	}

	if (reload_vertex_buffer_)
	{
		if (!vbo_)
			vbo_.emplace(render::vertex::vertex_buffer_object::VertexBufferUsage::Static);

		if (vbo_ && *vbo_)
		{
			auto size = detail::vertex_components * 6;
			vbo_->Reserve(size * sizeof(real));
			vertex_stream_.vertex_batch.VertexBuffer(vbo_);
		}

		reload_vertex_buffer_ = false;
	}

	vertex_stream_.vertex_batch.Prepare();
}

void MovableAnimation::Draw(shaders::ShaderProgram *shader_program) noexcept
{
	if (visible_ && animation_)
		vertex_stream_.vertex_batch.Draw(shader_program);
}


/*
	Elapse time
*/

void MovableAnimation::Elapse(duration time) noexcept
{
	if (animation_)
	{
		animation_->Elapse(time);
		vertex_stream_.vertex_batch.BatchTexture(animation_->CurrentFrame());
		vertex_stream_.vertex_batch.Elapse(time);
	}
}

} //ion::graphics::scene