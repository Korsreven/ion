/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/shapes
File:	IonEllipse.h
-------------------------------------------
*/

#ifndef ION_ELLIPSE_H
#define ION_ELLIPSE_H

#include "IonShape.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"
#include "graphics/utilities/IonVector3.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene::shapes
{
	using namespace types::type_literals;
	using namespace utilities;

	namespace ellipse::detail
	{
		constexpr auto default_ellipse_sides = 36;
		constexpr auto min_ellipse_sides = 3;


		inline auto ellipse_sides(int sides) noexcept
		{
			return sides < min_ellipse_sides ? min_ellipse_sides : sides;
		}

		mesh::Vertices ellipse_vertices(const Vector3 &position, const Vector2 &size, const Color &color, int sides);
	} //ellipse::detail


	class Ellipse : public Shape
	{
		protected:

			Vector3 position_;
			Vector2 size_;
			int sides_ = ellipse::detail::default_ellipse_sides;

		public:
		
			//Construct a new ellipse with the given size, color and visibility
			Ellipse(const Vector2 &size, const Color &color, bool visible = true);

			//Construct a new ellipse with the given position, size, color and visibility
			Ellipse(const Vector3 &position, const Vector2 &size, const Color &color, bool visible = true);


			//Construct a new ellipse with the given size, color, sides and visibility
			Ellipse(const Vector2 &size, const Color &color, int sides, bool visible = true);

			//Construct a new ellipse with the given position, size, color, sides and visibility
			Ellipse(const Vector3 &position, const Vector2 &size, const Color &color, int sides, bool visible = true);


			/*
				Modifiers
			*/

			//Sets the position of this ellipse to the given position
			inline void Position(const Vector3 &position) noexcept
			{
				if (position_ != position)
				{
					position_ = position;
					Mesh::VertexData(ellipse::detail::ellipse_vertices(position_, size_, color_, sides_));
				}
			}

			//Sets the size of this ellipse to the given size
			inline void Size(const Vector2 &size) noexcept
			{
				if (size_ != size)
				{
					size_ = size;
					Mesh::VertexData(ellipse::detail::ellipse_vertices(position_, size_, color_, sides_));
				}
			}


			//Sets the diameter of this ellipse to the given value
			inline void Diameter(real diameter) noexcept
			{
				Size({diameter});
			}

			//Sets the radius of this ellipse to the given value
			inline void Radius(real radius) noexcept
			{
				Size({radius * 2.0_r});
			}


			/*
				Observers
			*/

			//Returns the size of this ellipse
			[[nodiscard]] inline auto& Position() const noexcept
			{
				return position_;
			}

			//Returns the size of this ellipse
			[[nodiscard]] inline auto& Size() const noexcept
			{
				return size_;
			}

			//Returns the number of sides used to render this ellipse
			[[nodiscard]] inline auto Sides() const noexcept
			{
				return sides_;
			}
	};
} //ion::graphics::scene::shapes

#endif