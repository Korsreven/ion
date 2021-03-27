/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/shapes
File:	IonRectangle.h
-------------------------------------------
*/

#ifndef ION_RECTANGLE_H
#define ION_RECTANGLE_H

#include "IonShape.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"
#include "graphics/utilities/IonVector3.h"

namespace ion::graphics::scene::shapes
{
	using namespace utilities;

	namespace rectangle::detail
	{
		mesh::Vertices rectangle_to_vertices(const Vector3 &position, const Vector2 &size, const Color &color);
	} //rectangle::detail


	class Rectangle : public Shape
	{
		private:

			Vector3 position_;
			Vector2 size_;

		public:
		
			//Construct a new rectangle with the given size and visibility
			Rectangle(const Vector2 &size, const Color &color, bool visible = true);

			//Construct a new rectangle with the given position, size and visibility
			Rectangle(const Vector3 &position, const Vector2 &size, const Color &color, bool visible = true);


			/*
				Modifiers
			*/

			//Sets the position of this rectangle to the given position
			inline void Position(const Vector3 &position) noexcept
			{
				if (position_ != position)
				{
					position_ = position;
					Mesh::VertexData(rectangle::detail::rectangle_to_vertices(position_, size_, color_));
				}
			}

			//Sets the size of this rectangle to the given size
			inline void Size(const Vector2 &size) noexcept
			{
				if (size_ != size)
				{
					size_ = size;
					Mesh::VertexData(rectangle::detail::rectangle_to_vertices(position_, size_, color_));
				}
			}


			/*
				Observers
			*/

			//Returns the size of this rectangle
			[[nodiscard]] inline auto& Position() const noexcept
			{
				return position_;
			}

			//Returns the size of this rectangle
			[[nodiscard]] inline auto& Size() const noexcept
			{
				return size_;
			}
	};
} //ion::graphics::scene::shapes

#endif