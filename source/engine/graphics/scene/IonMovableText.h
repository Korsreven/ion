/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonMovableText.h
-------------------------------------------
*/

#ifndef ION_MOVABLE_TEXT_H
#define ION_MOVABLE_TEXT_H

#include <array>
#include <cstddef>
#include <optional>
#include <vector>

#include "IonMovableObject.h"
#include "graphics/fonts/IonText.h"
#include "graphics/render/vertex/IonVertexBatch.h"
#include "graphics/render/vertex/IonVertexBufferObject.h"
#include "graphics/render/vertex/IonVertexDeclaration.h"
#include "graphics/shaders/IonShaderLayout.h"
#include "graphics/utilities/IonColor.h"
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


	namespace movable_text::detail
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
		using decoration_vertex_container = std::vector<real>;	


		struct glyph_vertex_stream
		{
			vertex_container vertex_data;
			render::vertex::VertexBatch vertex_batch;

			glyph_vertex_stream(vertex_container vertex_data, int texture_handle);
		};

		struct decoration_vertex_stream
		{
			decoration_vertex_container back_vertex_data;
			decoration_vertex_container front_vertex_data;
			render::vertex::VertexBatch back_vertex_batch;
			render::vertex::VertexBatch front_vertex_batch;

			decoration_vertex_stream();
		};

		using glyph_vertex_streams = std::vector<glyph_vertex_stream>;


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


		/*
			Rendering
		*/

		Color get_foreground_color(const fonts::text::TextBlock &text_block, const fonts::Text &text) noexcept;
		std::optional<Color> get_background_color(const fonts::text::TextBlock &text_block, const fonts::Text &text) noexcept;
		std::optional<fonts::text::TextFontStyle> get_font_style(const fonts::text::TextBlock &text_block, const fonts::Text &text) noexcept;
		std::optional<fonts::text::TextDecoration> get_text_decoration(const fonts::text::TextBlock &text_block, const fonts::Text &text) noexcept;
		std::optional<Color> get_text_decoration_color(const fonts::text::TextBlock &text_block, const fonts::Text &text) noexcept;

		fonts::Font* get_default_font(const fonts::text::TextBlock &text_block, const fonts::Text &text) noexcept;
		
		real get_glyph_horizontal_position(const std::optional<Vector2> &area_size, const Vector2 &padding,
			fonts::text::TextAlignment horizontal_alignment, real line_width, const Vector3 &position) noexcept;
		real get_glyph_vertical_position(const std::optional<Vector2> &area_size, const Vector2 &padding,
			fonts::text::TextVerticalAlignment vertical_alignment, int font_size, real line_height, int total_lines, const Vector3 &position) noexcept;

		vertex_container get_glyph_vertex_data(const fonts::font::GlyphMetric &metric,
			const Vector3 &position, real rotation, const Vector2 &scaling, const Color &color,
			const Vector3 &origin, const Vector2 &coordinate_scaling);
		decoration_vertex_container get_decoration_vertex_data(const Vector3 &position, real rotation, const Vector2 &size, const Color &color,
			const Vector3 &origin, const Vector2 &coordinate_scaling);

		void get_block_vertex_streams(const fonts::text::TextBlock &text_block, const fonts::Text &text,
			int font_size, int &glyph_count, Vector3 &position, real rotation,
			const Vector3 &origin, const Vector2 &coordinate_scaling,
			glyph_vertex_streams &glyph_streams, decoration_vertex_stream &decoration_stream);
		void get_text_vertex_streams(const fonts::Text &text, Vector3 position, real rotation, const Vector2 &coordinate_scaling,
			glyph_vertex_streams &glyph_streams, decoration_vertex_stream &decoration_stream);
	} //movable_text::detail


	//A movable text that can be attached to a scene node
	class MovableText final : public MovableObject
	{
		private:

			Vector3 position_;
			real rotation_ = 0.0_r;

			std::optional<fonts::Text> text_;
			NonOwningPtr<fonts::Text> initial_text_;

			movable_text::detail::glyph_vertex_streams glyph_vertex_streams_;
			movable_text::detail::decoration_vertex_stream decoration_vertex_stream_;
			std::optional<render::vertex::VertexBufferObject> vbo_;

			bool reload_vertex_streams_ = false;
			bool reload_vertex_buffer_ = false;


			void PrepareVertexStreams();

		public:

			//Construct a new movable text with the given text and visibility
			explicit MovableText(NonOwningPtr<fonts::Text> text, bool visible = true);

			//Construct a new movable text with the given position, text and visibility
			MovableText(const Vector3 &position, NonOwningPtr<fonts::Text> text, bool visible = true);

			//Construct a new movable text with the given position, rotation, text and visibility
			MovableText(const Vector3 &position, real rotation, NonOwningPtr<fonts::Text> text, bool visible = true);


			/*
				Modifiers
			*/

			//Sets the position of this text to the given position
			inline void Position(const Vector3 &position) noexcept
			{
				if (position_ != position)
				{
					position_ = position;
					reload_vertex_streams_ = true;
				}
			}

			//Sets the rotation of this text to the given angle (in radians)
			inline void Rotation(real angle) noexcept
			{
				if (rotation_ != angle)
				{
					rotation_ = angle;
					reload_vertex_streams_ = true;
				}
			}


			//Revert to the initial text
			void Revert();


			/*
				Observers
			*/

			//Returns the position of this text
			[[nodiscard]] inline auto& Position() const noexcept
			{
				return position_;
			}

			//Returns the angle of rotation (in radians) for this text
			[[nodiscard]] inline auto Rotation() const noexcept
			{
				return rotation_;
			}


			//Returns a mutable reference to the text
			[[nodiscard]] auto& Get() noexcept
			{
				reload_vertex_streams_ = true; //Text could be changed
				return text_;
			}

			//Returns an immutable reference to the text
			[[nodiscard]] auto& Get() const noexcept
			{
				return text_;
			}


			//Return the vertex buffer this text uses
			[[nodiscard]] inline auto& VertexBuffer() const noexcept
			{
				return vbo_;
			}


			/*
				Preparing / drawing
			*/

			//Prepare this text such that it is ready to be drawn
			//This is called once regardless of passes
			void Prepare() noexcept override;

			//Draw this text with the given shader program (optional)
			//This can be called multiple times if more than one pass
			void Draw(shaders::ShaderProgram *shader_program = nullptr) noexcept override;


			/*
				Elapse time
			*/

			//Elapse the total time for this text by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept override;
	};
} //ion::graphics::scene

#endif