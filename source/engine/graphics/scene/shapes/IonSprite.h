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
#include "types/IonTypes.h"

namespace ion::graphics::scene::shapes
{
	using namespace types::type_literals;
	using namespace utilities;

	namespace sprite::detail
	{
		mesh::Vertices sprite_vertices(const Vector3 &position, real rotation, const Vector2 &size, const Color &color,
			const Vector2 &lower_left_tex_coord, const Vector2 &upper_right_tex_coord);

		std::optional<Vector2> get_texture_size(materials::Material &material) noexcept;
	} //sprite::detail


	//A class representing a texturized rectangle
	class Sprite : public Rectangle
	{
		protected:

			bool auto_size_ = false;
			bool auto_repeat_ = false;

			Vector2 lower_left_tex_coord_ = vector2::Zero;
			Vector2 upper_right_tex_coord_ = vector2::UnitScale;


			virtual mesh::Vertices GetVertices() const noexcept override;

			std::optional<Vector2> GetTextureSize() const noexcept;
			void RecalculateSize() noexcept;
			void RecalculateTexCoords() noexcept;


			/*
				Events
			*/

			virtual void MaterialChanged() noexcept override;

		public:
		
			//Construct a new sprite with the given material and visibility
			explicit Sprite(NonOwningPtr<materials::Material> material, bool visible = true);

			//Construct a new sprite with the given size, material and visibility
			Sprite(const Vector2 &size, NonOwningPtr<materials::Material> material, bool visible = true);

			//Construct a new sprite with the given position, size, material and visibility
			Sprite(const Vector3 &position, const Vector2 &size, NonOwningPtr<materials::Material> material, bool visible = true);

			//Construct a new sprite with the given position, rotation, size, material and visibility
			Sprite(const Vector3 &position, real rotation, const Vector2 &size, NonOwningPtr<materials::Material> material, bool visible = true);


			//Construct a new sprite with the given material, color and visibility
			Sprite(NonOwningPtr<materials::Material> material, const Color &color, bool visible = true);

			//Construct a new sprite with the given size, material, color and visibility
			Sprite(const Vector2 &size, NonOwningPtr<materials::Material> material, const Color &color, bool visible = true);

			//Construct a new sprite with the given position, size, material, color and visibility
			Sprite(const Vector3 &position, const Vector2 &size, NonOwningPtr<materials::Material> material, const Color &color, bool visible = true);

			//Construct a new sprite with the given position, rotation, size, material, color and visibility
			Sprite(const Vector3 &position, real rotation, const Vector2 &size, NonOwningPtr<materials::Material> material, const Color &color, bool visible = true);


			/*
				Modifiers
			*/

			//Sets whether or not this sprite should be auto sized
			inline void AutoSize(bool auto_size) noexcept
			{
				if (auto_size_ != auto_size)
				{
					auto_size_ = auto_size;

					if (auto_size)
						RecalculateSize();
					if (auto_repeat_)
						RecalculateTexCoords();
				}
			}

			//Sets whether or not this sprite should be auto repeated
			inline void AutoRepeat(bool auto_repeat) noexcept
			{
				if (auto_repeat_ != auto_repeat)
				{
					auto_repeat_ = auto_repeat;

					if (auto_repeat)
						RecalculateTexCoords();
				}
			}

			//Sets the lower left and upper right texture coordinates for this sprite to the given coordinates
			inline void TexCoords(const Vector2 &lower_left, const Vector2 &upper_right) noexcept
			{
				if (lower_left_tex_coord_ != lower_left || upper_right_tex_coord_ != upper_right)
				{
					lower_left_tex_coord_ = lower_left;
					upper_right_tex_coord_ = upper_right;

					auto_repeat_ = false;
					update_vertices_ = true;
				}
			}


			//Sets the size of this sprite to the given size
			inline void Size(const Vector2 &size) noexcept override
			{
				if (size_ != size)
				{
					Rectangle::Size(size);
					auto_size_ = false;

					if (auto_repeat_)
						RecalculateTexCoords();
				}
			}

			//Sets the surface material used by this sprite to the given material
			void SurfaceMaterial(NonOwningPtr<materials::Material> material) noexcept
			{
				Mesh::SurfaceMaterial(material);
			}


			/*
				Observers
			*/

			//Returns whether or not this sprite is auto sized
			[[nodiscard]] inline auto AutoSize() const noexcept
			{
				return auto_size_;
			}

			//Returns whether or not this sprite is auto repeated
			[[nodiscard]] inline auto AutoRepeat() const noexcept
			{
				return auto_repeat_;
			}

			//Returns the lower left and upper right texture coordinates for this sprite
			[[nodiscard]] inline auto TexCoords() const noexcept
			{
				return std::pair{lower_left_tex_coord_, upper_right_tex_coord_};
			}


			//Returns the size of this sprite
			[[nodiscard]] inline auto& Size() const noexcept
			{
				return Rectangle::Size();
			}

			//Returns a pointer to the material used by this sprite
			//Returns nullptr if this sprite does not have a material
			[[nodiscard]] inline auto SurfaceMaterial() const noexcept
			{
				return  RenderMaterial();
			}


			/*
				Texture coordinates
			*/

			//Crop sprite by the given area, where values are in range [0.0, 1.0]
			//This operation will discard any repeating previously applied
			void Crop(const std::optional<Aabb> &area) noexcept;

			//Repeat sprite by the given amount, where values are in range [0.0, oo)
			//This operation will discard any cropping previously applied
			void Repeat(const std::optional<Vector2> &amount) noexcept;


			//Flip sprite horizontally (mirror)
			void FlipHorizontal() noexcept;

			//Flip sprite vertically (up-down)
			void FlipVertical() noexcept;


			//Returns true if this sprite is cropped
			[[nodiscard]] bool IsCropped() const noexcept;

			//Returns true if this sprite is repeated
			[[nodiscard]] bool IsRepeated() const noexcept;


			//Returns true if this sprite is flipped horizontally
			[[nodiscard]] bool IsFlippedHorizontally() const noexcept;

			//Returns true if this sprite is flipped vertically
			[[nodiscard]] bool IsFlippedVertically() const noexcept;
	};
} //ion::graphics::scene::shapes

#endif