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

#include <optional>
#include <string>
#include <utility>

#include "IonShape.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"
#include "graphics/utilities/IonVector3.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

#undef min
#undef max

namespace ion::graphics::scene::shapes
{
	using namespace utilities;
	using namespace types::type_literals;

	namespace rectangle::detail
	{
		inline auto scale_to_fill(const Vector2 &size, const Vector2 &fill_size)
		{
			auto [x, y] = size.XY();
			auto [dst_x, dst_y] = fill_size.XY();
			return size * std::max(dst_x / x, dst_y / y);
		}

		inline auto scale_to_fit(const Vector2 &size, const Vector2 &fit_size)
		{
			auto [x, y] = size.XY();
			auto [dst_x, dst_y] = fit_size.XY();
			return size * std::min(dst_x / x, dst_y / y);
		}


		mesh::Vertices rectangle_vertices(const Vector3 &position, real rotation, const Vector2 &size, const Color &color);
	} //rectangle::detail


	///@brief A class representing a rectangle shape
	class Rectangle : public Shape
	{
		protected:

			Vector3 position_;
			real rotation_ = 0.0_r;
			Vector2 size_;


			///@brief Constructs a new rectangle with the given name, vertices, position, rotation, size, color and visibility
			///@details Can only be instantiated by derived
			Rectangle(std::optional<std::string> name, const mesh::Vertices &vertices,
				const Vector3 &position, real rotation, const Vector2 &size,
				const Color &color, bool visible = true);

			///@brief Constructs a new texturized rectangle with the given name, vertices, position, rotation, size, material, color and visibility
			///@details Can only be instantiated by derived
			Rectangle(std::optional<std::string> name, const mesh::Vertices &vertices,
				const Vector3 &position, real rotation, const Vector2 &size,
				NonOwningPtr<materials::Material> material, const Color &color, bool visible = true);


			virtual mesh::Vertices GetVertices() const noexcept override;

		public:
		
			///@brief Constructs a new rectangle with the given name, size, color and visibility
			Rectangle(std::optional<std::string> name, const Vector2 &size,
				const Color &color, bool visible = true);

			///@brief Constructs a new rectangle with the given name, position, size, color and visibility
			Rectangle(std::optional<std::string> name, const Vector3 &position, const Vector2 &size,
				const Color &color, bool visible = true);

			///@brief Constructs a new rectangle with the given name, position, rotation, size, color and visibility
			Rectangle(std::optional<std::string> name, const Vector3 &position, real rotation, const Vector2 &size,
				const Color &color, bool visible = true);


			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the position of this rectangle to the given position
			inline void Position(const Vector3 &position) noexcept
			{
				if (position_ != position)
				{
					position_ = position;
					update_vertices_ = true;
				}
			}

			///@brief Sets the position of this rectangle to the given position
			inline void Position(const Vector2 &position) noexcept
			{
				Position({position.X(), position.Y(), position_.Z()});
			}

			///@brief Sets the rotation of this rectangle to the given angle (in radians)
			inline void Rotation(real angle) noexcept
			{
				if (rotation_ != angle)
				{
					rotation_ = angle;
					update_vertices_ = true;
				}
			}

			///@brief Sets the size of this rectangle to the given size
			inline virtual void Size(const Vector2 &size) noexcept
			{
				if (size_ != size)
				{
					size_ = size;
					update_vertices_ = true;
				}
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the position of this rectangle
			[[nodiscard]] inline auto& Position() const noexcept
			{
				return position_;
			}

			///@brief Returns the angle of rotation (in radians) for this rectangle
			[[nodiscard]] inline auto Rotation() const noexcept
			{
				return rotation_;
			}

			///@brief Returns the size of this rectangle
			[[nodiscard]] inline auto& Size() const noexcept
			{
				return size_;
			}

			///@}

			/**
				@name Keep aspect ratio
				@{
			*/

			///@brief Sets the width of this rectangle to the given width, while keeping the aspect ratio
			inline void Width(real width) noexcept
			{
				if (auto [w, h] = size_.XY(); w != width && w > 0.0_r)
					Size({width, h * (width / w)});
			}

			///@brief Sets the height of this rectangle to the given height, while keeping the aspect ratio
			inline void Height(real height) noexcept
			{
				if (auto [w, h] = size_.XY(); h != height && h > 0.0_r)
					Size({w * (height / h), height});
			}


			///@brief Resizes this rectangle to fill out the given size, while keeping the aspect ratio
			inline void ResizeToFill(const Vector2 &size) noexcept
			{
				Size(rectangle::detail::scale_to_fill(size_, size));
			}

			///@brief Resizes this rectangle to fit in the given size, while keeping the aspect ratio
			inline void ResizeToFit(const Vector2 &size) noexcept
			{
				Size(rectangle::detail::scale_to_fit(size_, size));
			}

			///@}
	};
} //ion::graphics::scene::shapes

#endif