/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/shapes
File:	IonBorder.h
-------------------------------------------
*/

#ifndef ION_BORDER_H
#define ION_BORDER_H

#include <optional>
#include <utility>

#include <optional>
#include <tuple>

#include "IonRectangle.h"
#include "graphics/utilities/IonAabb.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"
#include "graphics/utilities/IonVector3.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene::shapes
{
	using namespace utilities;

	namespace border
	{
		enum class BorderCornerStyle
		{
			None,
			Square,
			Oblique
		};


		namespace detail
		{
			mesh::Vertices border_vertices(
				const Vector3 &position, real rotation, const Vector2 &size, const Vector2 &border_size,
				BorderCornerStyle corner_style, const Color &color,
				const std::optional<Color> &top_side_color, const std::optional<Color> &bottom_side_color,
				const std::optional<Color> &left_side_color, const std::optional<Color> &right_side_color,
				const std::optional<Color> &top_left_corner_color, const std::optional<Color> &top_right_corner_color,
				const std::optional<Color> &bottom_left_corner_color, const std::optional<Color> &bottom_right_corner_color);
		} //detail
	} //border


	//A border class where the width and height is the exclusive size without the borders
	class Border : public Rectangle
	{
		protected:

			Vector2 border_size_;
			border::BorderCornerStyle corner_style_ = border::BorderCornerStyle::None;

			std::optional<Color> top_side_color_;
			std::optional<Color> bottom_side_color_;
			std::optional<Color> left_side_color_;
			std::optional<Color> right_side_color_;

			std::optional<Color> top_left_corner_color_;	
			std::optional<Color> top_right_corner_color_;
			std::optional<Color> bottom_left_corner_color_;
			std::optional<Color> bottom_right_corner_color_;


			virtual mesh::Vertices GetVertices() const noexcept override;

		public:

			//Construct a new border with the given size, border size, color and visibility
			Border(const Vector2 &size, const Vector2 &border_size, const Color &color, bool visible = true);

			//Construct a new border with the given position, size, border size, color and visibility
			Border(const Vector3 &position, const Vector2 &size, const Vector2 &border_size, const Color &color, bool visible = true);

			//Construct a new border with the given position, rotation, size, border size, color and visibility
			Border(const Vector3 &position, real rotation, const Vector2 &size, const Vector2 &border_size, const Color &color, bool visible = true);


			//Construct a new border with the given size, border size, corner style, color and visibility
			Border(const Vector2 &size, const Vector2 &border_size,
				border::BorderCornerStyle corner_style, const Color &color, bool visible = true);

			//Construct a new border with the given position, size, border size, corner style, color and visibility
			Border(const Vector3 &position, const Vector2 &size, const Vector2 &border_size,
				border::BorderCornerStyle corner_style, const Color &color, bool visible = true);

			//Construct a new border with the given position, rotation, size, border size, corner style, color and visibility
			Border(const Vector3 &position, real rotation, const Vector2 &size, const Vector2 &border_size,
				border::BorderCornerStyle corner_style, const Color &color, bool visible = true);


			/*
				Modifiers
			*/

			//Sets the border size of this border to the given size
			inline void BorderSize(const Vector2 &size) noexcept
			{
				if (border_size_ != size)
				{
					border_size_ = size;
					Mesh::VertexData(GetVertices());
				}
			}


			//Sets the color of this border to the given color
			//Resets custom side and corner colors
			inline void BorderColor(const Color &color) noexcept
			{
				SideColor({});
				CornerColor({});
				FillColor(color);
			}

			//Sets the color of all of the border sides (without the corners) to the given color
			//Resets custom side colors by passing nullopt
			inline void SideColor(const std::optional<Color> &color) noexcept
			{
				if (top_side_color_ != color ||
					bottom_side_color_ != color ||
					left_side_color_ != color ||
					right_side_color_ != color)
				{
					top_side_color_ = color;
					bottom_side_color_ = color;
					left_side_color_ = color;
					right_side_color_ = color;
					Mesh::VertexData(GetVertices());
				}
			}

			//Sets the color of each of the border sides (without the corners) to the given colors
			//Resets custom side colors by passing nullopt
			inline void SideColor(const std::optional<Color> &top, const std::optional<Color> &bottom,
								  const std::optional<Color> &left, const std::optional<Color> &right) noexcept
			{
				if (top_side_color_ != top ||
					bottom_side_color_ != bottom ||
					left_side_color_ != left ||
					right_side_color_ != right)
				{
					top_side_color_ = top;
					bottom_side_color_ = bottom;
					left_side_color_ = left;
					right_side_color_ = right;
					Mesh::VertexData(GetVertices());
				}
			}

			//Sets the color of all of the border corners (without the sides) to the given color
			//Resets custom corner colors by passing nullopt
			inline void CornerColor(const std::optional<Color> &color) noexcept
			{
				if (top_left_corner_color_ != color ||
					top_right_corner_color_ != color ||
					bottom_left_corner_color_ != color ||
					bottom_right_corner_color_ != color)
				{
					top_left_corner_color_ = color;
					top_right_corner_color_ = color;
					bottom_left_corner_color_ = color;
					bottom_right_corner_color_ = color;
					Mesh::VertexData(GetVertices());
				}
			}

			//Sets the color of each of the border corners (without the sides) to the given colors
			//Resets custom corner colors by passing nullopt
			inline void CornerColor(const std::optional<Color> &top_left, const std::optional<Color> &top_right,
									const std::optional<Color> &bottom_left, const std::optional<Color> &bottom_right) noexcept
			{
				if (top_left_corner_color_ != top_left ||
					top_right_corner_color_ != top_right ||
					bottom_left_corner_color_ != bottom_left ||
					bottom_right_corner_color_ != bottom_right)
				{
					top_left_corner_color_ = top_left;
					top_right_corner_color_ = top_right;
					bottom_left_corner_color_ = bottom_left;
					bottom_right_corner_color_ = bottom_right;
					Mesh::VertexData(GetVertices());
				}
			}


			/*
				Observers
			*/

			//Returns the border size of this border
			[[nodiscard]] inline auto& BorderSize() const noexcept
			{
				return size_;
			}

			//Returns the corner style of this border
			[[nodiscard]] inline auto CornerStyle() const noexcept
			{
				return corner_style_;
			}


			//Returns the (base) color of this border
			[[nodiscard]] inline auto& BorderColor() const noexcept
			{
				return FillColor();
			}

			//Returns the color of each of the sides of this border
			//Returns nullopt if the side does not have a custom color
			[[nodiscard]] inline auto SideColors() const noexcept
			{
				return std::tuple{top_side_color_, bottom_side_color_,
								  left_side_color_, right_side_color_};
			}

			//Returns the color of each of the corners of this border
			//Returns nullopt if the corner does not have a custom color
			[[nodiscard]] inline auto CornerColor() const noexcept
			{
				return std::tuple{top_left_corner_color_, top_right_corner_color_,
								  bottom_left_corner_color_, bottom_right_corner_color_};
			}
	};
} //ion::graphics::scene::shapes

#endif