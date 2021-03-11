/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonMovableText.cpp
-------------------------------------------
*/

#include "IonMovableText.h"

#include <algorithm>

#include "graphics/IonGraphicsAPI.h"
#include "graphics/fonts/IonFont.h"
#include "graphics/fonts/IonTypeFace.h"
#include "graphics/fonts/utilities/IonFontUtility.h"
#include "graphics/render/IonViewport.h"
#include "graphics/scene/IonSceneManager.h"
#include "graphics/shaders/IonShaderProgram.h"
#include "graphics/shaders/IonShaderProgramManager.h"

#undef min

namespace ion::graphics::scene
{

using namespace movable_text;

using namespace graphics::utilities;
using namespace types::type_literals;

namespace movable_text::detail
{

glyph_vertex_stream::glyph_vertex_stream(vertex_container vertex_data, int texture_handle) :

	vertex_data{std::move(vertex_data)},
	vertex_batch
	{
		render::vertex::vertex_batch::VertexDrawMode::Triangles,
		get_vertex_declaration(),
		this->vertex_data,
		texture_handle
	}
{
	//Empty
}


Color get_foreground_color(const fonts::text::TextBlock &text_block, const fonts::Text &text) noexcept
{
	return text_block.ForegroundColor ? *text_block.ForegroundColor : text.DefaultForegroundColor();
}

std::optional<Color> get_background_color(const fonts::text::TextBlock &text_block, const fonts::Text &text) noexcept
{
	return text_block.BackgroundColor ? text_block.BackgroundColor : text.DefaultBackgroundColor();
}

std::optional<fonts::text::TextFontStyle> get_font_style(const fonts::text::TextBlock &text_block, const fonts::Text &text) noexcept
{
	return text_block.FontStyle ? text_block.FontStyle : text.DefaultFontStyle();
}

std::optional<fonts::text::TextDecoration> get_text_decoration(const fonts::text::TextBlock &text_block, const fonts::Text &text) noexcept
{
	return text_block.Decoration ? text_block.Decoration : text.DefaultDecoration();
}

std::optional<Color> get_text_decoration_color(const fonts::text::TextBlock &text_block, const fonts::Text &text) noexcept
{
	return text_block.DecorationColor ? text_block.DecorationColor : text.DefaultDecorationColor();
}


fonts::Font* get_default_font(const fonts::text::TextBlock &text_block, const fonts::Text &text) noexcept
{
	if (auto type_face = text.Lettering(); type_face)
	{
		if (auto font_style = get_font_style(text_block, text); font_style)
		{
			switch (*font_style)
			{
				case fonts::text::TextFontStyle::Bold:
				{
					if (type_face->BoldFont())
						return type_face->BoldFont().get();
					break;
				}

				case fonts::text::TextFontStyle::Italic:
				{
					if (type_face->ItalicFont())
						return type_face->ItalicFont().get();
					break;
				}

				case fonts::text::TextFontStyle::BoldItalic:
				{
					if (type_face->BoldItalicFont())
						return type_face->BoldItalicFont().get();
					break;
				}
			}
		}

		return type_face->RegularFont().get();
	}
	else
		return nullptr;
}


real get_glyph_horizontal_position(const std::optional<Vector2> &area_size, const Vector2 &padding,
	fonts::text::TextAlignment horizontal_alignment, real line_width, const Vector3 &position) noexcept
{
	auto [x, y, z] = position.XYZ();

	if (area_size)
	{
		auto area_max_size = fonts::text::detail::text_area_max_size(*area_size, padding);
		auto [width, height] = area_max_size.XY();

		switch (horizontal_alignment)
		{
			case fonts::text::TextAlignment::Left:
			return x - width * 0.5_r;

			case fonts::text::TextAlignment::Center:
			return x - line_width * 0.5_r;

			case fonts::text::TextAlignment::Right:
			return x + width * 0.5_r - line_width;
		}
	}
	else
	{
		switch (horizontal_alignment)
		{
			case fonts::text::TextAlignment::Left:
			return x;

			case fonts::text::TextAlignment::Center:
			return x - line_width * 0.5_r;

			case fonts::text::TextAlignment::Right:
			return x - line_width;
		}
	}

	return x;
}

real get_glyph_vertical_position(const std::optional<Vector2> &area_size, const Vector2 &padding,
	fonts::text::TextVerticalAlignment vertical_alignment, real font_height, real line_height, int total_lines, const Vector3 &position) noexcept
{
	auto [x, y, z] = position.XYZ();

	if (area_size)
	{
		auto area_max_size = fonts::text::detail::text_area_max_size(*area_size, padding);
		auto [width, height] = area_max_size.XY();

		switch (vertical_alignment)
		{
			case fonts::text::TextVerticalAlignment::Top:	
			return y + height * 0.5_r - line_height + (line_height - font_height) * 0.5_r;

			case fonts::text::TextVerticalAlignment::Middle:
			return y + (line_height * total_lines) * 0.5_r - (line_height - font_height) * 0.5_r - font_height;

			case fonts::text::TextVerticalAlignment::Bottom:
			return y - height * 0.5_r + line_height * (total_lines - 1) + (line_height - font_height) * 0.5_r;
		}
	}
	else
	{
		switch (vertical_alignment)
		{
			case fonts::text::TextVerticalAlignment::Top:
			return y - line_height + (line_height - font_height) * 0.5_r;

			case fonts::text::TextVerticalAlignment::Middle:
			return y + (line_height * total_lines) * 0.5_r - (line_height - font_height) * 0.5_r - font_height;

			case fonts::text::TextVerticalAlignment::Bottom:
			return y + line_height * (total_lines - 1) + (line_height - font_height) * 0.5_r;
		}
	}

	return y;
}


vertex_container get_glyph_vertex_data(const fonts::font::GlyphMetric &metric,
	const Vector3 &position, const Vector2 &scaling, const Vector2 &coordinate_scaling, const Color &color)
{
	auto [x, y, z] = position.XYZ();
	auto [r, g, b, a] = color.RGBA();

	auto s = static_cast<real>(metric.Width) / metric.ActualWidth;
	auto t = static_cast<real>(metric.Height) / metric.ActualHeight;

	auto left = metric.Left * coordinate_scaling.X();
	auto top = metric.Top * coordinate_scaling.Y();
	auto width = metric.Width * scaling.X() * coordinate_scaling.X();
	auto height = metric.Height * scaling.Y() * coordinate_scaling.Y();

	x += left;
	y += top - height;

	//Note:
	//The vertices follows [0, height] -> [width, 0] (normal coordinate system)
	//The texture coordinates follows [0, 0] -> [width, height] (GUI coordinate system)

	//Vertex format:
	//x, y, z
	//r, g, b, a
	//s, t

	return
		{
			//Vertex #1
			x, y + height, z,
			r, g, b, a,
			0.0_r, 0.0_r,

			//Vertex #2
			x, y, z,
			r, g, b, a,
			0.0_r, t,

			//Vertex #3
			x + width, y, z,
			r, g, b, a,
			s, t,

			//Vertex #4
			x + width, y, z,
			r, g, b, a,
			s, t,

			//Vertex #5
			x + width, y + height, z,
			r, g, b, a,
			s, 0.0_r,

			//Vertex #6
			x, y + height, z,
			r, g, b, a,
			0.0_r, 0.0_r
		};
}

void get_block_vertex_streams(const fonts::text::TextBlock &text_block, const fonts::Text &text, int &glyph_count,
	Vector3 &position, const Vector2 &coordinate_scaling, glyph_vertex_streams &streams)
{
	if (auto font = get_default_font(text_block, text); font)
	{
		if (auto &handles = font->GlyphHandles(); handles)
		{
			if (auto &metrics = font->GlyphMetrics(); metrics)
			{
				auto foreground_color = get_foreground_color(text_block, text);
				auto scaling =
					[&]() noexcept
					{
						if (text_block.FontSize)
						{
							switch (*text_block.FontSize)
							{
								case fonts::text::TextBlockFontSize::Smaller:
								return fonts::utilities::detail::smaller_scale_factor;

								case fonts::text::TextBlockFontSize::Larger:
								return fonts::utilities::detail::larger_scale_factor;
							}
						}

						return 1.0_r;
					}();

				//For each character
				for (auto c : text_block.Content)
				{
					auto glyph_index = static_cast<unsigned char>(c);
					
					if (glyph_index >= static_cast<int>(font->CharacterEncoding()))
						glyph_index = static_cast<unsigned char>('?');
							//Use question mark for characters outside current font encoding

					//Update existing stream
					if (glyph_count < std::ssize(streams))
					{
						streams[glyph_count].vertex_data =
							get_glyph_vertex_data((*metrics)[glyph_index], position, scaling, coordinate_scaling, foreground_color);
						streams[glyph_count].vertex_batch.VertexData(streams[glyph_count].vertex_data);
						streams[glyph_count].vertex_batch.BatchTexture((*handles)[glyph_index]);
					}

					//New stream
					else
					{
						streams.emplace_back(
							get_glyph_vertex_data((*metrics)[glyph_index], position, scaling, coordinate_scaling, foreground_color),
							(*handles)[glyph_index]
						);

						streams.back().vertex_batch.UseVertexArray(false);
							//Turn off vertex array object (VAO) for each glyph
							//There could be a lot of glyphs in a text,
							//so keep hardware VAOs for other geometry
					}

					position.X(position.X() + (*metrics)[glyph_index].Advance * scaling * coordinate_scaling.X());
					++glyph_count;
				}

				//Decorations
				//auto background_color = get_foreground_color(text_block, text);
				//auto decoration = get_foreground_color(text_block, text);
				//auto decoration_color = get_foreground_color(text_block, text);
			}
		}
	}
}

void get_text_vertex_streams(const fonts::Text &text, const Vector3 &position, const Vector2 &coordinate_scaling, glyph_vertex_streams &streams)
{
	auto line_height = text.LineHeight();

	if (!line_height)
		return; //Text type face is not available/loaded

	auto area_size = text.AreaSize();
	auto padding = text.Padding();
	auto formatted_lines = text.FormattedLines();
	auto from_line = text.FromLine();
	auto max_lines = text.MaxLines().value_or(std::ssize(formatted_lines));
	auto font_height = static_cast<real>(text.Lettering()->RegularFont()->Size());

	if (area_size)
	{
		if (*line_height > 0.0_r)
		{
			auto area_max_lines = fonts::text::detail::text_area_max_lines(*area_size, padding, *line_height);
			max_lines = std::min(max_lines, area_max_lines);
		}
	}

	//Convert text sizes to camera coordinates
	*line_height *= coordinate_scaling.Y();

	if (area_size)
		*area_size *= coordinate_scaling.Y();

	padding *= coordinate_scaling;
	font_height *= coordinate_scaling.Y();

	//One or more text lines to display
	if (!std::empty(formatted_lines) &&
		from_line < std::ssize(formatted_lines) && max_lines > 0)
	{
		auto glyph_count = 0;
		auto glyph_position = position;	

		//Get first glyph y position
		glyph_position.Y(
			get_glyph_vertical_position(
				area_size, padding, text.VerticalAlignment(),
				font_height, *line_height, max_lines - from_line, position
			));

		for (auto iter = std::begin(formatted_lines) + from_line,
			end = iter + max_lines; iter != end; ++iter)
		{
			//Get first glyph x position
			glyph_position.X(
				get_glyph_horizontal_position(
					area_size, padding, text.Alignment(),
					iter->second.X() * coordinate_scaling.X(), position
				));

			for (auto &block : iter->first.Blocks)
				get_block_vertex_streams(block, text, glyph_count, glyph_position, coordinate_scaling, streams);

			glyph_position.Y(glyph_position.Y() - *line_height); //Next glyph y position
		}
	}
}

} //movable_text::detail


//Private

void MovableText::PrepareVertexStreams()
{
	auto coordinate_scaling = vector2::UnitScale;

	//Has viewport connected to scene
	if (auto viewport = Owner()->ConnectedViewport(); viewport)
	{
		//Has camera connected to viewport
		if (auto camera = viewport->ConnectedCamera(); camera)
		{
			auto viewport_size = viewport->Bounds().ToSize();
			auto [width, height] = viewport_size.XY();

			//Get scaling factor from viewport to camera coordinates
			auto [left, right, bottom, top, z_near, z_far] = camera->ViewFrustum().ToOrthoBounds(viewport_size);
			coordinate_scaling = {(right - left) / width, (top - bottom) / height};
		}
	}

	auto max_glyphs = text_->UnformattedDisplayedCharacterCount();

	if (max_glyphs > static_cast<int>(vertex_streams_.capacity()))
	{
		vertex_streams_.reserve(max_glyphs);
		reload_vertex_buffer_ = true;
	}

	get_text_vertex_streams(*text_, {0.0_r, 0.0_r, -1.0_r}, coordinate_scaling, vertex_streams_);

	if (std::ssize(vertex_streams_) > max_glyphs)
		vertex_streams_.erase(std::begin(vertex_streams_) + max_glyphs, std::end(vertex_streams_));
}

//Public

MovableText::MovableText(NonOwningPtr<fonts::Text> text, bool visible) :
	
	MovableObject{visible},
	text_{text ? std::make_optional(*text) : std::nullopt},
	initial_text_{text},

	reload_vertex_streams_{!!text_}
{
	//Empty
}


/*
	Modifiers
*/

void MovableText::Revert()
{
	if (initial_text_)
		text_ = *initial_text_;
}


/*
	Preparing / drawing
*/

void MovableText::Prepare() noexcept
{
	if (!text_)
		return;

	if (reload_vertex_streams_)
	{
		PrepareVertexStreams();
		reload_vertex_streams_ = false;
	}

	if (reload_vertex_buffer_)
	{
		if (!vbo_)
			vbo_.emplace(render::vertex::vertex_buffer_object::VertexBufferUsage::Dynamic);

		if (vbo_ && *vbo_)
		{
			if (!std::empty(vertex_streams_))
			{
				auto size = detail::vertex_components * 6;
				vbo_->Reserve(size * std::ssize(vertex_streams_) * sizeof(real));

				for (auto offset = 0; auto &stream : vertex_streams_)
				{
					stream.vertex_batch.VertexBuffer(vbo_->SubBuffer(offset * sizeof(real), size * sizeof(real)));
					offset += size;
				}
			}
		}

		reload_vertex_buffer_ = false;
	}

	for (auto &stream : vertex_streams_)
		stream.vertex_batch.Prepare();
}


void MovableText::Draw(shaders::ShaderProgram *shader_program) noexcept
{
	if (visible_ && text_ && !std::empty(vertex_streams_))
	{
		auto use_shader = shader_program && shader_program->Owner() && shader_program->Handle();

		if (use_shader)
			shader_program->Owner()->ActivateShaderProgram(*shader_program);

		for (auto &stream : vertex_streams_)
			stream.vertex_batch.Draw(shader_program);

		if (use_shader)
			shader_program->Owner()->DeactivateShaderProgram(*shader_program);
	}
}


/*
	Elapse time
*/

void MovableText::Elapse(duration time) noexcept
{
	if (text_)
	{
		for (auto &stream : vertex_streams_)
			stream.vertex_batch.Elapse(time);
	}
}

} //ion::graphics::scene