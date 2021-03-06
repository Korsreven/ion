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

#undef min

namespace ion::graphics::scene
{

using namespace movable_text;
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
	return 0.0_r;
}

real get_glyph_vertical_position(const std::optional<Vector2> &area_size, const Vector2 &padding,
	fonts::text::TextVerticalAlignment vertical_alignment, real line_height, int current_line, int total_lines, const Vector3 &position) noexcept
{
	return 0.0_r;
}


vertex_container get_glyph_vertex_data(const fonts::font::GlyphMetric &metric, const Vector3 &position, const Color &color)
{
	auto [x, y, z] = position.XYZ();
	auto [r, g, b, a] = color.RGBA();

	x += metric.Left;
	y += static_cast<real>(metric.Top) - metric.Height;

	auto s = static_cast<real>(metric.Width) / metric.ActualWidth;
	auto t = static_cast<real>(metric.Height) / metric.ActualHeight;

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
			x, y + metric.Height, z,
			r, g, b, a,
			0.0_r, 0.0_r,

			//Vertex #2
			x, y, z,
			r, g, b, a,
			0.0_r, t,

			//Vertex #3
			x + metric.Width, y, z,
			r, g, b, a,
			s, t,

			//Vertex #4
			x + metric.Width, y, z,
			r, g, b, a,
			s, t,

			//Vertex #5
			x + metric.Width, y + metric.Height, z,
			r, g, b, a,
			s, 0.0_r,

			//Vertex #6
			x, y + metric.Height, z,
			r, g, b, a,
			0.0_r, 0.0_r
		};
}

void get_block_vertex_streams(const fonts::text::TextBlock &text_block, const fonts::Text &text, Vector3 &position, glyph_vertex_streams &streams)
{
	if (auto font = get_default_font(text_block, text); font)
	{
		if (auto &handles = font->GlyphHandles(); handles)
		{
			if (auto &metrics = font->GlyphMetrics(); metrics)
			{
				auto foreground_color = get_foreground_color(text_block, text);	

				//For each character
				for (auto c : text_block.Content)
				{
					auto glyph_index = static_cast<unsigned char>(c);
					
					if (glyph_index >= static_cast<int>(font->CharacterEncoding()))
						glyph_index = static_cast<unsigned char>('?');
							//Use question mark for characters outside current font encoding

					streams.emplace_back(
						get_glyph_vertex_data((*metrics)[glyph_index], position, foreground_color),
						(*handles)[glyph_index]
					);

					streams.back().vertex_batch.UseVertexArray(false);
						//Turn off vertex array object (VAO) for each glyph
						//There could be a lot of glyphs in a text,
						//so keep hardware VAOs for other geometry

					position.X(position.X() + (*metrics)[glyph_index].Advance);
				}

				//Decorations
				//auto background_color = get_foreground_color(text_block, text);
				//auto decoration = get_foreground_color(text_block, text);
				//auto decoration_color = get_foreground_color(text_block, text);
			}
		}
	}
}

void get_text_vertex_streams(const fonts::Text &text, const Vector3 &position, glyph_vertex_streams &streams)
{
	auto line_height = text.LineHeight();

	if (!line_height)
		return; //Text type face is not available/loaded


	auto formatted_lines = text.FormattedLines();
	auto from_line = text.FromLine();
	auto max_lines = text.MaxLines().value_or(std::ssize(formatted_lines));

	if (auto area_size = text.AreaSize(); area_size)
	{
		if (*line_height > 0.0_r)
		{
			auto area_max_lines = fonts::text::detail::text_area_max_lines(*area_size, text.Padding(), *line_height);
			max_lines = std::min(max_lines, area_max_lines);
		}
	}

	//One or more text lines to display
	if (!std::empty(formatted_lines) &&
		from_line < std::ssize(formatted_lines) && max_lines > 0)
	{
		auto total_lines = max_lines - from_line;
		auto glyph_position = position;

		//Get first glyph y position
		glyph_position.Y(
			get_glyph_vertical_position(
				text.AreaSize(), text.Padding(), text.VerticalAlignment(),
				*line_height, 0, total_lines, position
			));

		for (auto iter = std::begin(formatted_lines) + from_line,
			end = iter + max_lines; iter != end; ++iter)
		{
			//Get first glyph x position
			glyph_position.X(
				get_glyph_horizontal_position(
					text.AreaSize(), text.Padding(), text.Alignment(),
					iter->second.X(), position
				));

			for (auto &block : iter->first.Blocks)
				get_block_vertex_streams(block, text, glyph_position, streams);

			glyph_position.Y(glyph_position.Y() - *line_height); //Next glyph y position
		}
	}
}

} //movable_text::detail


//Private

void MovableText::PrepareGlyphVertexStreams()
{
	/*if (std::size(text_->Emitters()) > vertex_streams_.capacity())
		vertex_streams_.reserve(std::size(text_->Emitters()));

	for (auto off = 0; auto &emitter : text_->Emitters())
	{
		//Update existing emitter
		if (off < std::ssize(vertex_streams_))
		{
			if (vertex_streams_[off].particle_quota != emitter.ParticleQuota())
			{
				vertex_streams_[off].particle_quota = emitter.ParticleQuota();
				reload_vertex_buffer_ = true;
			}

			if (vertex_streams_[off].emitter.get() != &emitter)
				vertex_streams_[off].emitter = text_->GetEmitter(*emitter.Name());

			vertex_streams_[off].vertex_batch.VertexData({std::data(emitter.Particles()), std::ssize(emitter.Particles())});
			vertex_streams_[off].vertex_batch.ReloadData(); //Must reload data even if vertex data view (range) is unchanged
			vertex_streams_[off].vertex_batch.BatchMaterial(emitter.ParticleMaterial());
		}

		//New emitter
		else
		{
			vertex_streams_.emplace_back(
				emitter.ParticleQuota(),
				text_->GetEmitter(*emitter.Name()),
				render::vertex::VertexBatch{
					render::vertex::vertex_batch::VertexDrawMode::Points,
					detail::get_vertex_declaration(),
					{std::data(emitter.Particles()), std::ssize(emitter.Particles())},
					emitter.ParticleMaterial()
				}
			);

			reload_vertex_buffer_ = true;
		}

		++off;
	}

	//Erase unused vertex streams
	if (std::size(vertex_streams_) > std::size(text_->Emitters()))
		vertex_streams_.erase(std::begin(vertex_streams_) + std::size(text_->Emitters()), std::end(vertex_streams_));*/
}

//Public

MovableText::MovableText(NonOwningPtr<fonts::Text> text, bool visible) :
	
	MovableObject{visible},
	text_{text ? std::make_optional(*text) : std::nullopt},
	initial_text_{text}
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

	PrepareGlyphVertexStreams();

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
	if (visible_ && text_)
	{
		for (auto &stream : vertex_streams_)
			stream.vertex_batch.Draw(shader_program);
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