/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/
File:	IonMovableAnimation.h
-------------------------------------------
*/

#ifndef ION_MOVABLE_ANIMATION_H
#define ION_MOVABLE_ANIMATION_H

#include <array>
#include <optional>

#include "IonMovableObject.h"
#include "graphics/textures/IonAnimation.h"
#include "graphics/render/vertex/IonVertexBatch.h"
#include "graphics/render/vertex/IonVertexBufferObject.h"
#include "graphics/render/vertex/IonVertexDeclaration.h"
#include "graphics/shaders/IonShaderLayout.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"
#include "graphics/utilities/IonVector3.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

//Forward declarations
namespace ion::graphics
{
	namespace fonts
	{
		class Font;
	}

	namespace shaders
	{
		class ShaderProgram;
	}
}

namespace ion::graphics::scene
{
	using namespace types::type_literals;
	using utilities::Color;
	using utilities::Vector2;
	using utilities::Vector3;


	namespace movable_animation::detail
	{
		constexpr auto position_components = 3; //x,y,z
		constexpr auto color_components = 4; //r,g,b,a
		constexpr auto tex_coord_components = 2; //s,t

		constexpr auto position_offset = 0;
		constexpr auto color_offset = position_offset + position_components;
		constexpr auto tex_coord_offset = color_offset + color_components;

		constexpr auto vertex_components =
			position_components + color_components + tex_coord_components;

		using vertex_container = std::array<real, vertex_components * 6>;


		struct animation_vertex_stream
		{
			vertex_container vertex_data;
			render::vertex::VertexBatch vertex_batch;

			animation_vertex_stream();
		};


		inline auto get_vertex_declaration() noexcept
		{
			return
				render::vertex::VertexDeclaration
				{
					{
						{shaders::shader_layout::AttributeName::Vertex_Position,
							render::vertex::vertex_declaration::VertexElementType::Float3,
							position_offset * sizeof(real), vertex_components * sizeof(real)},

						{shaders::shader_layout::AttributeName::Vertex_Color,
							render::vertex::vertex_declaration::VertexElementType::Float4,
							color_offset * sizeof(real), vertex_components * sizeof(real)},

						{shaders::shader_layout::AttributeName::Vertex_TexCoord,
							render::vertex::vertex_declaration::VertexElementType::Float2,
							tex_coord_offset * sizeof(real), vertex_components * sizeof(real)}
					}
				};
		}

		vertex_container get_animation_vertex_data(textures::Animation &animation,
			const Vector3 &position, real rotation, const Vector2 &size, const Color &color);
	} //movable_animation::detail


	//A movable animation that can be attached to a scene node
	class MovableAnimation final : public MovableObject
	{
		private:
		
			Vector3 position_;
			real rotation_ = 0.0_r;
			Vector2 size_;
			Color color_;

			std::optional<textures::Animation> animation_;
			NonOwningPtr<textures::Animation> initial_animation_;

			movable_animation::detail::animation_vertex_stream vertex_stream_;
			std::optional<render::vertex::VertexBufferObject> vbo_;

			bool reload_vertex_stream_ = false;
			bool reload_vertex_buffer_ = false;


			void PrepareVertexStream();

		public:
		
			//Construct a new movable animation with the given size, animation and visibility
			MovableAnimation(const Vector2 &size, NonOwningPtr<textures::Animation> animation, bool visible = true);

			//Construct a new movable animation with the given position, size, animation and visibility
			MovableAnimation(const Vector3 &position, const Vector2 &size, NonOwningPtr<textures::Animation> animation, bool visible = true);

			//Construct a new movable animation with the given position, rotation, size, animation and visibility
			MovableAnimation(const Vector3 &position, real rotation, const Vector2 &size, NonOwningPtr<textures::Animation> animation, bool visible = true);


			//Construct a new movable animation with the given size, animation, color and visibility
			MovableAnimation(const Vector2 &size, NonOwningPtr<textures::Animation> animation, const Color &color, bool visible = true);

			//Construct a new movable animation with the given position, size, animation, color and visibility
			MovableAnimation(const Vector3 &position, const Vector2 &size, NonOwningPtr<textures::Animation> animation, const Color &color, bool visible = true);

			//Construct a new movable animation with the given position, rotation, size, animation, color and visibility
			MovableAnimation(const Vector3 &position, real rotation, const Vector2 &size, NonOwningPtr<textures::Animation> animation, const Color &color, bool visible = true);


			/*
				Modifiers
			*/

			//Sets the position of this animation to the given position
			inline void Position(const Vector3 &position) noexcept
			{
				if (position_ != position)
				{
					position_ = position;
					reload_vertex_stream_ = true;
				}
			}

			//Sets the rotation of this animation to the given angle (in radians)
			inline void Rotation(real angle) noexcept
			{
				if (rotation_ != angle)
				{
					rotation_ = angle;
					reload_vertex_stream_ = true;
				}
			}

			//Sets the size of this animation to the given size
			inline void Size(const Vector2 &size) noexcept
			{
				if (size_ != size)
				{
					size_ = size;
					reload_vertex_stream_ = true;
				}
			}

			//Sets the tint of this animation to the given color
			inline void Tint(const Color &color) noexcept
			{
				if (color_ != color)
				{
					color_ = color;
					reload_vertex_stream_ = true;
				}
			}


			//Revert to the initial animation
			void Revert();


			/*
				Observers
			*/

			//Returns the position of this animation
			[[nodiscard]] inline auto& Position() const noexcept
			{
				return position_;
			}

			//Returns the angle of rotation (in radians) for this animation
			[[nodiscard]] inline auto Rotation() const noexcept
			{
				return rotation_;
			}

			//Returns the size of this animation
			[[nodiscard]] inline auto& Size() const noexcept
			{
				return size_;
			}

			//Returns the tint of this animation
			[[nodiscard]] inline auto& Tint() const noexcept
			{
				return color_;
			}


			//Returns a mutable reference to the animation
			[[nodiscard]] auto& Get() noexcept
			{
				return animation_;
			}

			//Returns an immutable reference to the animation
			[[nodiscard]] auto& Get() const noexcept
			{
				return animation_;
			}


			/*
				Preparing / drawing
			*/

			//Prepare this animation such that it is ready to be drawn
			//This is called once regardless of passes
			void Prepare() noexcept override;

			//Draw this animation with the given shader program (optional)
			//This can be called multiple times if more than one pass
			void Draw(shaders::ShaderProgram *shader_program = nullptr) noexcept override;


			/*
				Elapse time
			*/

			//Elapse the total time for this animation by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept override;
	};
} //ion::graphics::scene

#endif