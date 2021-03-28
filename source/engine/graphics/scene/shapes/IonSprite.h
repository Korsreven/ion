/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/shapes
File:	IonSprite.h
-------------------------------------------
*/

#ifndef ION_SPRITE_H
#define ION_SPRITE_H

#include <optional>
#include <utility>

#include "IonRectangle.h"
#include "graphics/utilities/IonAabb.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"
#include "graphics/utilities/IonVector3.h"
#include "memory/IonNonOwningPtr.h"

namespace ion::graphics::scene::shapes
{
	using namespace utilities;

	namespace sprite::detail
	{
		mesh::Vertices sprite_vertices(const Vector3 &position, const Vector2 &size, const Color &color,
			const Vector2 &lower_left_tex_coord, const Vector2 &upper_right_tex_coord);
	} //sprite::detail


	class Sprite : public Rectangle
	{
		protected:

			Vector2 lower_left_tex_coord_ = vector2::Zero;
			Vector2 upper_right_tex_coord_ = vector2::UnitScale;

		public:
		
			//Construct a new sprite with the given size, material and visibility
			Sprite(const Vector2 &size, NonOwningPtr<materials::Material> material, bool visible = true);

			//Construct a new sprite with the given position, size, material and visibility
			Sprite(const Vector3 &position, const Vector2 &size, NonOwningPtr<materials::Material> material, bool visible = true);


			//Construct a new sprite with the given size, material, color and visibility
			Sprite(const Vector2 &size, NonOwningPtr<materials::Material> material, const Color &color, bool visible = true);

			//Construct a new sprite with the given position, size, material, color and visibility
			Sprite(const Vector3 &position, const Vector2 &size, NonOwningPtr<materials::Material> material, const Color &color, bool visible = true);


			/*
				Modifiers
			*/

			//Sets the lower left and upper right texture coordinates for this sprite to the given coordinates
			inline void TexCoords(const Vector2 &lower_left, const Vector2 &upper_right) noexcept
			{
				if (lower_left_tex_coord_ != lower_left || upper_right_tex_coord_ != upper_right)
				{
					lower_left_tex_coord_ = lower_left;
					upper_right_tex_coord_ = upper_right;

					Mesh::TexCoordMode(mesh::MeshTexCoordMode::Manual);
					Mesh::VertexData(sprite::detail::sprite_vertices(position_, size_, color_,
						lower_left_tex_coord_, upper_right_tex_coord_));
				}
			}


			/*
				Observers
			*/

			//Returns the lower left and upper right texture coordinates for this sprite
			[[nodiscard]] inline auto TexCoords() const noexcept
			{
				return std::pair{lower_left_tex_coord_, upper_right_tex_coord_};
			}


			/*
				Texture coordinates
			*/

			//Crop sprite by the given area, where values are in range [0.0, 1.0]
			void Crop(const std::optional<Aabb> &area) noexcept;

			//Flip sprite horizontally (mirror)
			void FlipHorizontal() noexcept;

			//Flip sprite vertically (up-down)
			void FlipVertical() noexcept;


			//Returns true if this sprite is cropped
			[[nodiscard]] bool IsCropped() const noexcept;

			//Returns true if this sprite is flipped horizontally
			[[nodiscard]] bool IsFlippedHorizontally() const noexcept;

			//Returns true if this sprite is flipped vertically
			[[nodiscard]] bool IsFlippedVertically() const noexcept;
	};
} //ion::graphics::scene::shapes

#endif