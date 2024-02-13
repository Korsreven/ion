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

#include <optional>
#include <string>

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

		mesh::Vertices ellipse_vertices(const Vector3 &position, real rotation, const Vector2 &size, const Color &color, int sides);
	} //ellipse::detail


	///@brief A class representing an ellipse shape (or circle)
	class Ellipse : public Shape
	{
		protected:

			Vector3 position_;
			real rotation_ = 0.0_r;
			Vector2 size_;
			int sides_ = ellipse::detail::default_ellipse_sides;


			virtual mesh::Vertices GetVertices() const noexcept override;

		public:
		
			///@brief Constructs a new ellipse with the given name, size, color and visibility
			Ellipse(std::optional<std::string> name, const Vector2 &size,
				const Color &color, bool visible = true);

			///@brief Constructs a new ellipse with the given name, position, size, color and visibility
			Ellipse(std::optional<std::string> name, const Vector3 &position, const Vector2 &size,
				const Color &color, bool visible = true);

			///@brief Constructs a new ellipse with the given name, position, rotation, size, color and visibility
			Ellipse(std::optional<std::string> name, const Vector3 &position, real rotation, const Vector2 &size,
				const Color &color, bool visible = true);


			///@brief Constructs a new ellipse with the given name, size, color, sides and visibility
			Ellipse(std::optional<std::string> name, const Vector2 &size,
				const Color &color, int sides, bool visible = true);

			///@brief Constructs a new ellipse with the given name, position, size, color, sides and visibility
			Ellipse(std::optional<std::string> name, const Vector3 &position, const Vector2 &size,
				const Color &color, int sides, bool visible = true);

			///@brief Constructs a new ellipse with the given name, position, rotation, size, color, sides and visibility
			Ellipse(std::optional<std::string> name, const Vector3 &position, real rotation, const Vector2 &size,
				const Color &color, int sides, bool visible = true);


			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the position of this ellipse to the given position
			inline void Position(const Vector3 &position) noexcept
			{
				if (position_ != position)
				{
					position_ = position;
					update_vertices_ = true;
				}
			}

			///@brief Sets the position of this ellipse to the given position
			inline void Position(const Vector2 &position) noexcept
			{
				Position({position.X(), position.Y(), position_.Z()});
			}

			///@brief Sets the rotation of this ellipse to the given angle (in radians)
			inline void Rotation(real angle) noexcept
			{
				if (rotation_ != angle)
				{
					rotation_ = angle;
					update_vertices_ = true;
				}
			}

			///@brief Sets the size of this ellipse to the given size
			inline void Size(const Vector2 &size) noexcept
			{
				if (size_ != size)
				{
					size_ = size;
					update_vertices_ = true;
				}
			}


			///@brief Sets the diameter of this ellipse to the given value
			inline void Diameter(real diameter) noexcept
			{
				Size({diameter});
			}

			///@brief Sets the radius of this ellipse to the given value
			inline void Radius(real radius) noexcept
			{
				Size({radius * 2.0_r});
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the position of this ellipse
			[[nodiscard]] inline auto& Position() const noexcept
			{
				return position_;
			}

			///@brief Returns the angle of rotation (in radians) for this ellipse
			[[nodiscard]] inline auto Rotation() const noexcept
			{
				return rotation_;
			}

			///@brief Returns the size of this ellipse
			[[nodiscard]] inline auto& Size() const noexcept
			{
				return size_;
			}

			///@brief Returns the number of sides used to render this ellipse
			[[nodiscard]] inline auto Sides() const noexcept
			{
				return sides_;
			}

			///@}
	};
} //ion::graphics::scene::shapes

#endif