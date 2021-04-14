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
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene::shapes
{
	using namespace utilities;
	using namespace types::type_literals;

	namespace rectangle::detail
	{
		mesh::Vertices rectangle_vertices(const Vector3 &position, real rotation, const Vector2 &size, const Color &color);
	} //rectangle::detail


	class Rectangle : public Shape
	{
		protected:

			Vector3 position_;
			real rotation_ = 0.0_r;
			Vector2 size_;


			//Construct a new rectangle with the given size, color, material and visibility
			//Can only be instantiated by derived
			Rectangle(const Vector2 &size, const Color &color, NonOwningPtr<materials::Material> material, bool visible = true);

			//Construct a new rectangle with the given position, size, color, material and visibility
			//Can only be instantiated by derived
			Rectangle(const Vector3 &position, const Vector2 &size, const Color &color,
				NonOwningPtr<materials::Material> material, bool visible = true);

			//Construct a new rectangle with the given position, rotation, size, color, material and visibility
			//Can only be instantiated by derived
			Rectangle(const Vector3 &position, real rotation, const Vector2 &size, const Color &color,
				NonOwningPtr<materials::Material> material, bool visible = true);


			virtual mesh::Vertices GetVertices() const noexcept override;

		public:
		
			//Construct a new rectangle with the given size, color and visibility
			Rectangle(const Vector2 &size, const Color &color, bool visible = true);

			//Construct a new rectangle with the given position, size, color and visibility
			Rectangle(const Vector3 &position, const Vector2 &size, const Color &color, bool visible = true);

			//Construct a new rectangle with the given position, rotation, size, color and visibility
			Rectangle(const Vector3 &position, real rotation, const Vector2 &size, const Color &color, bool visible = true);


			/*
				Modifiers
			*/

			//Sets the position of this rectangle to the given position
			inline void Position(const Vector3 &position) noexcept
			{
				if (position_ != position)
				{
					position_ = position;
					Mesh::VertexData(GetVertices());
				}
			}

			//Sets the rotation of this rectangle to the given angle (in radians)
			inline void Rotation(real angle) noexcept
			{
				if (rotation_ != angle)
				{
					rotation_ = angle;
					Mesh::VertexData(GetVertices());
				}
			}

			//Sets the size of this rectangle to the given size
			inline void Size(const Vector2 &size) noexcept
			{
				if (size_ != size)
				{
					size_ = size;
					Mesh::VertexData(GetVertices());
				}
			}


			/*
				Observers
			*/

			//Returns the position of this rectangle
			[[nodiscard]] inline auto& Position() const noexcept
			{
				return position_;
			}

			//Returns the angle of rotation (in radians) for this rectangle
			[[nodiscard]] inline auto Rotation() const noexcept
			{
				return rotation_;
			}

			//Returns the size of this rectangle
			[[nodiscard]] inline auto& Size() const noexcept
			{
				return size_;
			}
	};
} //ion::graphics::scene::shapes

#endif