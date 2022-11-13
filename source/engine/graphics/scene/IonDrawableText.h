/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonDrawableText.h
-------------------------------------------
*/

#ifndef ION_MOVABLE_TEXT_H
#define ION_MOVABLE_TEXT_H

#include <array>
#include <cstddef>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

#include "IonDrawableObject.h"
#include "adaptors/IonFlatMap.h"
#include "graphics/fonts/IonText.h"
#include "graphics/render/IonRenderPrimitive.h"
#include "graphics/render/vertex/IonVertexDeclaration.h"
#include "graphics/shaders/IonShaderLayout.h"
#include "graphics/textures/IonTexture.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"
#include "graphics/utilities/IonVector3.h"
#include "memory/IonNonOwningPtr.h"
#include "memory/IonOwningPtr.h"
#include "types/IonTypes.h"

//Forward declarations
namespace ion::graphics
{
	namespace fonts
	{
		class Font;
	}
}

namespace ion::graphics::scene
{
	class DrawableText; //Forward declaration

	using namespace types::type_literals;
	using utilities::Color;
	using utilities::Vector2;
	using utilities::Vector3;


	namespace drawable_text::detail
	{
		constexpr auto position_components = 3; //x,y,z
		constexpr auto color_components = 4; //r,g,b,a
		constexpr auto tex_coord_components = 3; //s,t,u

		constexpr auto position_offset = 0;
		constexpr auto color_offset = position_offset + position_components;
		constexpr auto tex_coord_offset = color_offset + color_components;

		constexpr auto vertex_components =
			position_components + color_components + tex_coord_components;


		struct text_primitive : render::RenderPrimitive
		{
			protected:

				/**
					@name Events
					@{
				*/

				void RenderPassesChanged() noexcept override;

				///@}

			public:

				DrawableText *owner = nullptr;
				render::render_primitive::VertexContainer vertex_data;


				text_primitive();
				text_primitive(textures::texture::TextureHandle texture_handle);
		};

		struct text_glyph_primitive final : text_primitive
		{
			text_glyph_primitive(textures::texture::TextureHandle texture_handle);
		};

		struct text_decoration_primitive final : text_primitive
		{
			text_decoration_primitive();
		};


		struct text_glyph_primitive_key final
		{
			const fonts::Font *font = nullptr;
			int glyph_index = 0;

			bool operator<(const text_glyph_primitive_key &key) const noexcept;
		};

		using text_glyph_primitives = adaptors::FlatMap<text_glyph_primitive_key, OwningPtr<text_glyph_primitive>>;
		using text_decoration_primitives = std::pair<OwningPtr<text_decoration_primitive>, OwningPtr<text_decoration_primitive>>;


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
							render::vertex::vertex_declaration::VertexElementType::Float3,
							tex_coord_offset * sizeof(real), vertex_components * sizeof(real)}
					}
				};
		}

		std::tuple<Aabb, Obb, Sphere> generate_bounding_volumes(const fonts::Text &text,
			const Vector2 &position, real rotation) noexcept;


		/**
			@name Rendering
			@{
		*/

		Color get_foreground_color(const fonts::text::TextBlock &text_block, const fonts::Text &text) noexcept;
		std::optional<Color> get_background_color(const fonts::text::TextBlock &text_block, const fonts::Text &text) noexcept;
		std::optional<fonts::text::TextFontStyle> get_font_style(const fonts::text::TextBlock &text_block, const fonts::Text &text) noexcept;
		std::optional<fonts::text::TextDecoration> get_text_decoration(const fonts::text::TextBlock &text_block, const fonts::Text &text) noexcept;
		std::optional<Color> get_text_decoration_color(const fonts::text::TextBlock &text_block, const fonts::Text &text) noexcept;

		fonts::Font* get_default_font(const fonts::Text &text) noexcept;
		fonts::Font* get_default_font(const fonts::text::TextBlock &text_block, const fonts::Text &text) noexcept;
		
		real get_glyph_horizontal_position(const std::optional<Vector2> &area_size, const Vector2 &padding,
			fonts::text::TextAlignment horizontal_alignment, real line_width, const Vector3 &position) noexcept;
		real get_glyph_vertical_position(const std::optional<Vector2> &area_size, const Vector2 &padding,
			fonts::text::TextVerticalAlignment vertical_alignment, int font_size, real line_height, int total_lines, const Vector3 &position) noexcept;

		render::render_primitive::VertexContainer get_glyph_vertex_data(real glyph_index, const fonts::font::GlyphMetric &metric,
			const Vector3 &position, real rotation, const Vector2 &scaling,
			const Color &color, const Vector3 &origin);
		render::render_primitive::VertexContainer get_decoration_vertex_data(
			const Vector3 &position, real rotation, const Vector2 &size,
			const Color &color, const Vector3 &origin, real delta_z);

		void get_block_primitives(const fonts::text::TextBlock &text_block, const fonts::Text &text,
			int font_size, int &glyph_count, Vector3 &position, real rotation, const Vector3 &origin,
			text_glyph_primitives &glyph_primitives, text_decoration_primitives &decoration_primitives);
		void get_text_primitives(const fonts::Text &text, Vector3 position, real rotation,
			text_glyph_primitives &glyph_primitives, text_decoration_primitives &decoration_primitives);

		///@}
	} //drawable_text::detail


	///@brief A class representing a drawable text that can be attached to a scene node
	class DrawableText final : public DrawableObject
	{
		private:

			Vector3 position_;
			real rotation_ = 0.0_r;

			std::optional<fonts::Text> text_;
			NonOwningPtr<fonts::Text> initial_text_;

			drawable_text::detail::text_glyph_primitives glyph_primitives_;
			drawable_text::detail::text_decoration_primitives decoration_primitives_;

			bool reload_primitives_ = false;
			bool update_bounding_volumes_ = false;


			void ReloadPrimitives();

		public:

			///@brief Constructs a new drawable text with the given name, text and visibility
			DrawableText(std::optional<std::string> name,
				NonOwningPtr<fonts::Text> text, bool visible = true);

			///@brief Constructs a new drawable text with the given name, position, text and visibility
			DrawableText(std::optional<std::string> name, const Vector3 &position,
				NonOwningPtr<fonts::Text> text, bool visible = true);

			///@brief Constructs a new drawable text with the given name, position, rotation, text and visibility
			DrawableText(std::optional<std::string> name, const Vector3 &position, real rotation,
				NonOwningPtr<fonts::Text> text, bool visible = true);


			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the position of this text to the given position
			inline void Position(const Vector3 &position) noexcept
			{
				if (position_ != position)
				{
					position_ = position;
					reload_primitives_ = true;
				}
			}

			///@brief Sets the position of this text to the given position
			inline void Position(const Vector2 &position) noexcept
			{
				Position({position.X(), position.Y(), position_.Z()});
			}

			///@brief Sets the rotation of this text to the given angle (in radians)
			inline void Rotation(real angle) noexcept
			{
				if (rotation_ != angle)
				{
					rotation_ = angle;
					reload_primitives_ = true;
				}
			}


			///@brief Reverts to the initial text
			void Revert();

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the position of this text
			[[nodiscard]] inline auto& Position() const noexcept
			{
				return position_;
			}

			///@brief Returns the angle of rotation (in radians) for this text
			[[nodiscard]] inline auto Rotation() const noexcept
			{
				return rotation_;
			}


			///@brief Returns a mutable reference to the text
			[[nodiscard]] auto& Get() noexcept
			{
				reload_primitives_ = true; //Text could be changed
				return text_;
			}

			///@brief Returns an immutable reference to the text
			[[nodiscard]] auto& Get() const noexcept
			{
				return text_;
			}

			///@brief Returns an immutable reference to the text
			[[nodiscard]] auto& GetImmutable() const noexcept
			{
				return text_;
			}

			///@}

			/**
				@name Preparing
				@{
			*/

			///@brief Prepares this text such that it is ready to be drawn
			///@details This function is typically called each frame
			void Prepare() override;

			///@}
	};
} //ion::graphics::scene

#endif