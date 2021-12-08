/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonDrawableText.cpp
-------------------------------------------
*/

#include "IonDrawableText.h"

#include <algorithm>
#include <cmath>

#include "graphics/IonGraphicsAPI.h"
#include "graphics/fonts/IonFont.h"
#include "graphics/fonts/IonTypeFace.h"
#include "graphics/fonts/utilities/IonFontUtility.h"
#include "graphics/render/IonViewport.h"
#include "graphics/scene/IonSceneManager.h"
#include "graphics/shaders/IonShaderProgram.h"
#include "graphics/shaders/IonShaderProgramManager.h"
#include "query/IonSceneQuery.h"

#undef min

namespace ion::graphics::scene
{

using namespace drawable_text;
using namespace graphics::utilities;

namespace drawable_text::detail
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

decoration_vertex_stream::decoration_vertex_stream() :

	back_vertex_batch
	{
		render::vertex::vertex_batch::VertexDrawMode::Triangles,
		get_vertex_declaration()
	},
	front_vertex_batch
	{
		render::vertex::vertex_batch::VertexDrawMode::Triangles,
		get_vertex_declaration()
	}
{
	//Empty
}


std::tuple<Aabb, Obb, Sphere> generate_bounding_volumes(const fonts::Text &text,
	Vector2 position, real rotation, const Vector2 &pixel_unit_ratio) noexcept
{
	auto size = text.MinimumAreaSize() * pixel_unit_ratio;

	if (!text.AreaSize())
	{
		auto [x, y] = position.XY();
		auto [half_width, half_height] = (size * 0.5_r).XY();

		//Adjust x to center
		switch (text.Alignment())
		{
			case fonts::text::TextAlignment::Left:
			x += half_width;
			break;

			case fonts::text::TextAlignment::Right:
			x -= half_width;
			break;
		}

		//Adjust y to center
		switch (text.VerticalAlignment())
		{
			case fonts::text::TextVerticalAlignment::Top:
			y -= half_height;
			break;

			case fonts::text::TextVerticalAlignment::Bottom:
			y += half_height;
			break;
		}

		position = {x, y};
	}

	auto aabb = Aabb::Size(size, position).RotateCopy(rotation);
	return {aabb, aabb, {aabb.ToHalfSize().Max(), aabb.Center()}};
}


/*
	Rendering
*/

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
	fonts::text::TextVerticalAlignment vertical_alignment, int font_size, real line_height, int total_lines, const Vector3 &position) noexcept
{
	auto [x, y, z] = position.XYZ();

	if (area_size)
	{
		auto area_max_size = fonts::text::detail::text_area_max_size(*area_size, padding);
		auto [width, height] = area_max_size.XY();

		switch (vertical_alignment)
		{
			case fonts::text::TextVerticalAlignment::Top:	
			return y + height * 0.5_r - line_height + (line_height - font_size) * 0.5_r;

			case fonts::text::TextVerticalAlignment::Middle:
			return y + (line_height * total_lines) * 0.5_r - (line_height - font_size) * 0.5_r - font_size;

			case fonts::text::TextVerticalAlignment::Bottom:
			return y - height * 0.5_r + line_height * (total_lines - 1) + (line_height - font_size) * 0.5_r;
		}
	}
	else
	{
		switch (vertical_alignment)
		{
			case fonts::text::TextVerticalAlignment::Top:
			return y - line_height + (line_height - font_size) * 0.5_r;

			case fonts::text::TextVerticalAlignment::Middle:
			return y + (line_height * total_lines) * 0.5_r - (line_height - font_size) * 0.5_r - font_size;

			case fonts::text::TextVerticalAlignment::Bottom:
			return y + line_height * (total_lines - 1) + (line_height - font_size) * 0.5_r;
		}
	}

	return y;
}


vertex_container get_glyph_vertex_data(const fonts::font::GlyphMetric &metric,
	const Vector3 &position, real rotation, const Vector2 &scaling,
	const Color &color, real opacity, const Vector3 &origin, const Vector2 &pixel_unit_ratio)
{
	auto [x, y, z] = position.XYZ();
	auto [r, g, b, a] = color.RGBA();
	a *= opacity;

	auto s = static_cast<real>(metric.Width) / metric.ActualWidth;
	auto t = static_cast<real>(metric.Height) / metric.ActualHeight;

	x += metric.Left;
	y += metric.Top - metric.Height;
	auto width = metric.Width * scaling.X();
	auto height = metric.Height * scaling.Y();

	//Floor/ceil values (glyphs may appear blurry if positioned off-pixel)
	x = std::floor(x);
	y = std::floor(y);
	width = std::ceil(width);
	height = std::ceil(height);

	//Scale values from viewport to camera coordinates
	x *= pixel_unit_ratio.X();
	y *= pixel_unit_ratio.Y();
	width *= pixel_unit_ratio.X();
	height *= pixel_unit_ratio.Y();

	auto v1 = Vector3{x, y + height, z}.RotateCopy(rotation, origin);
	auto v2 = Vector3{x, y, z}.RotateCopy(rotation, origin);
	auto v3 = Vector3{x + width, y, z}.RotateCopy(rotation, origin);
	auto v4 = Vector3{x + width, y + height, z}.RotateCopy(rotation, origin);

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
			v1.X(), v1.Y(), v1.Z(),
			r, g, b, a,
			0.0_r, 0.0_r,

			//Vertex #2
			v2.X(), v2.Y(), v2.Z(),
			r, g, b, a,
			0.0_r, t,

			//Vertex #3
			v3.X(), v3.Y(), v3.Z(),
			r, g, b, a,
			s, t,

			//Vertex #4
			v3.X(), v3.Y(), v3.Z(),
			r, g, b, a,
			s, t,

			//Vertex #5
			v4.X(), v4.Y(), v4.Z(),
			r, g, b, a,
			s, 0.0_r,

			//Vertex #6
			v1.X(), v1.Y(), v1.Z(),
			r, g, b, a,
			0.0_r, 0.0_r
		};
}

decoration_vertex_container get_decoration_vertex_data(
	const Vector3 &position, real rotation, const Vector2 &size,
	const Color &color, real opacity, const Vector3 &origin, const Vector2 &pixel_unit_ratio)
{
	auto [x, y, z] = position.XYZ();
	auto [width, height] = size.XY();
	auto [r, g, b, a] = color.RGBA();
	a *= opacity;

	//Floor/ceil values (decorations may appear blurry if positioned off-pixel)
	x = std::floor(x);
	y = std::floor(y);
	width = std::ceil(width);
	height = std::ceil(height);

	//Scale values from viewport to camera coordinates
	x *= pixel_unit_ratio.X();
	y *= pixel_unit_ratio.Y();
	width *= pixel_unit_ratio.X();
	height *= pixel_unit_ratio.Y();

	auto v1 = Vector3{x, y + height, z}.RotateCopy(rotation, origin);
	auto v2 = Vector3{x, y, z}.RotateCopy(rotation, origin);
	auto v3 = Vector3{x + width, y, z}.RotateCopy(rotation, origin);
	auto v4 = Vector3{x + width, y + height, z}.RotateCopy(rotation, origin);

	//Vertex format:
	//x, y, z
	//r, g, b, a
	//s, t

	return
		{
			//Vertex #1
			v1.X(), v1.Y(), v1.Z(),
			r, g, b, a,
			0.0_r, 0.0_r,

			//Vertex #2
			v2.X(), v2.Y(), v2.Z(),
			r, g, b, a,
			0.0_r, 1.0_r,

			//Vertex #3
			v3.X(), v3.Y(), v3.Z(),
			r, g, b, a,
			1.0_r, 1.0_r,

			//Vertex #4
			v3.X(), v3.Y(), v3.Z(),
			r, g, b, a,
			1.0_r, 1.0_r,

			//Vertex #5
			v4.X(), v4.Y(), v4.Z(),
			r, g, b, a,
			1.0_r, 0.0_r,

			//Vertex #6
			v1.X(), v1.Y(), v1.Z(),
			r, g, b, a,
			0.0_r, 0.0_r
		};
}


void get_block_vertex_streams(const fonts::text::TextBlock &text_block, const fonts::Text &text,
	int font_size, int &glyph_count, Vector3 &position, real rotation, real opacity,
	const Vector3 &origin, const Vector2 &pixel_unit_ratio,
	glyph_vertex_streams &glyph_streams, decoration_vertex_stream &decoration_stream)
{
	if (auto font = get_default_font(text_block, text); font)
	{
		if (auto &handles = font->GlyphHandles(); handles)
		{
			if (auto &metrics = font->GlyphMetrics(); metrics)
			{
				auto base_y = position.Y();
				position.Y(position.Y() + font_size * fonts::utilities::detail::get_text_block_translate_factor(text_block));
				auto scaling = fonts::utilities::detail::get_text_block_scale_factor(text_block);
				auto foreground_color = get_foreground_color(text_block, text);

				auto line_thickness = std::max(1.0_r, std::floor(font_size / 8.0_r));
				auto line_margin = std::ceil(font_size / 16.0_r);

				//Background
				if (auto background_color = get_background_color(text_block, text); background_color)
				{
					auto decoration_position = Vector3{position.X(), base_y - (line_margin * 2.0_r + line_thickness * 2.0_r), position.Z()};
					auto decoration_size = Vector2{text_block.Size->X(), font_size + (line_margin * 4.0_r + line_thickness * 4.0_r)};

					auto vertex_data = get_decoration_vertex_data(
						decoration_position, rotation, decoration_size,
						*background_color, opacity, origin, pixel_unit_ratio);
					decoration_stream.back_vertex_data.insert(std::end(decoration_stream.back_vertex_data), std::begin(vertex_data), std::end(vertex_data));
				}
				
				//Text decoration
				if (auto decoration = get_text_decoration(text_block, text); decoration)
				{
					auto decoration_position =
						[&]() noexcept -> Vector3
						{
							switch (*decoration)
							{
								case fonts::text::TextDecoration::Underline:
								return {position.X(), position.Y() - (line_margin + line_thickness), position.Z()};

								case fonts::text::TextDecoration::LineThrough:
								return {position.X(), position.Y() + font_size * scaling * 0.5_r - line_thickness, position.Z()};

								case fonts::text::TextDecoration::Overline:
								return {position.X(), position.Y() + font_size * scaling + (line_margin + line_thickness), position.Z()};
							}

							return position;
						}();
					
					auto decoration_size = Vector2{text_block.Size->X(), line_thickness};
					auto decoration_color = get_text_decoration_color(text_block, text).value_or(foreground_color);
					auto vertex_data = get_decoration_vertex_data(
						decoration_position, rotation, decoration_size,
						decoration_color, opacity, origin, pixel_unit_ratio);

					if (*decoration == fonts::text::TextDecoration::LineThrough)
						decoration_stream.front_vertex_data.insert(std::end(decoration_stream.front_vertex_data), std::begin(vertex_data), std::end(vertex_data));
					else
						decoration_stream.back_vertex_data.insert(std::end(decoration_stream.back_vertex_data), std::begin(vertex_data), std::end(vertex_data));
				}

				//For each character
				for (auto c : text_block.Content)
				{
					if (auto glyph_index = static_cast<unsigned char>(c);
						glyph_index < std::size(*metrics))
					{
						//Update existing stream
						if (glyph_count < std::ssize(glyph_streams))
						{
							glyph_streams[glyph_count].vertex_data =
								get_glyph_vertex_data((*metrics)[glyph_index],
									position, rotation, scaling,
									foreground_color, opacity, origin, pixel_unit_ratio);
							glyph_streams[glyph_count].vertex_batch.VertexData(glyph_streams[glyph_count].vertex_data);
							glyph_streams[glyph_count].vertex_batch.ReloadData(); //Must reload data even if vertex data view (range) is unchanged
							glyph_streams[glyph_count].vertex_batch.BatchTexture((*handles)[glyph_index]);
						}

						//New stream
						else
						{
							glyph_streams.emplace_back(
								get_glyph_vertex_data((*metrics)[glyph_index],
									position, rotation, scaling,
									foreground_color, opacity, origin, pixel_unit_ratio),
								(*handles)[glyph_index]
							);

							glyph_streams.back().vertex_batch.UseVertexArray(false);
								//Turn off vertex array object (VAO) for each glyph
								//There could be a lot of glyphs in a text,
								//so keep hardware VAOs for other geometry
						}

						position.X(position.X() + (*metrics)[glyph_index].Advance * scaling);
						++glyph_count;
					}
				}

				position.Y(base_y);
			}
		}
	}
}

void get_text_vertex_streams(const fonts::Text &text, Vector3 position, real rotation, real opacity,
	const Vector2 &pixel_unit_ratio, glyph_vertex_streams &glyph_streams, decoration_vertex_stream &decoration_stream)
{
	auto line_height = text.LineHeight();

	if (!line_height)
		return; //Text type face is not available/loaded

	auto formatted_lines = text.FormattedLines();
	auto &area_size = text.AreaSize();
	auto &padding = text.Padding();
	auto from_line = text.FromLine();
	auto max_lines = text.MaxLines().value_or(std::ssize(formatted_lines));
	auto font_size = text.Lettering()->RegularFont()->Size();

	if (area_size)
	{
		if (*line_height > 0.0_r)
		{
			auto area_max_lines = fonts::text::detail::text_area_max_lines(*area_size, padding, *line_height);
			max_lines = std::min(max_lines, area_max_lines);
		}
	}

	//One or more text lines to display
	if (!std::empty(formatted_lines) &&
		from_line < std::ssize(formatted_lines) && max_lines > 0)
	{
		if (from_line + max_lines > std::ssize(formatted_lines))
			max_lines = std::ssize(formatted_lines) - from_line;
		
		Vector3 origin = position;
		position /= Vector3{pixel_unit_ratio.X(), pixel_unit_ratio.Y(), 1.0_r};
			//Adjust coordinates from viewport to glyph space

		auto glyph_count = 0;
		auto glyph_position = position;	

		//Get first glyph y position
		glyph_position.Y(
			get_glyph_vertical_position(
				area_size, padding, text.VerticalAlignment(),
				font_size, *line_height, max_lines - from_line, position
			));

		for (auto iter = std::begin(formatted_lines) + from_line,
			end = iter + max_lines; iter != end; ++iter)
		{
			//Get first glyph x position
			glyph_position.X(
				get_glyph_horizontal_position(
					area_size, padding, text.Alignment(),
					iter->Size->X(), position
				));

			for (auto &block : iter->Blocks)
				get_block_vertex_streams(block, text,
					font_size, glyph_count, glyph_position, rotation, opacity,
					origin, pixel_unit_ratio,
					glyph_streams, decoration_stream);

			glyph_position.Y(glyph_position.Y() - *line_height); //Next glyph y position
		}
	}
}

} //drawable_text::detail


//Private

void DrawableText::PrepareVertexStreams(const Vector2 &pixel_unit_ratio)
{
	auto glyph_count = text_->UnformattedDisplayedCharacterCount();
	auto back_decoration_count = std::ssize(decoration_vertex_stream_.back_vertex_data);
	auto front_decoration_count = std::ssize(decoration_vertex_stream_.front_vertex_data);

	if (glyph_count > static_cast<int>(glyph_vertex_streams_.capacity()))
	{
		glyph_vertex_streams_.reserve(glyph_count);
		reload_vertex_buffer_ = true;
	}

	if (front_decoration_count > 0)
	{
		decoration_vertex_stream_.front_vertex_data.clear();
		decoration_vertex_stream_.front_vertex_batch.VertexData(decoration_vertex_stream_.front_vertex_data);
	}

	if (back_decoration_count > 0)
	{
		decoration_vertex_stream_.back_vertex_data.clear();
		decoration_vertex_stream_.back_vertex_batch.VertexData(decoration_vertex_stream_.back_vertex_data);
	}


	detail::get_text_vertex_streams(*text_, position_, rotation_, Opacity(),
		pixel_unit_ratio, glyph_vertex_streams_, decoration_vertex_stream_);


	if (std::ssize(glyph_vertex_streams_) > glyph_count)
		glyph_vertex_streams_.erase(std::begin(glyph_vertex_streams_) + glyph_count, std::end(glyph_vertex_streams_));

	if (!std::empty(decoration_vertex_stream_.front_vertex_data))
	{
		decoration_vertex_stream_.front_vertex_batch.VertexData(decoration_vertex_stream_.front_vertex_data);

		if (std::ssize(decoration_vertex_stream_.front_vertex_data) > front_decoration_count)
			reload_vertex_buffer_ = true;
	}

	if (!std::empty(decoration_vertex_stream_.back_vertex_data))
	{
		decoration_vertex_stream_.back_vertex_batch.VertexData(decoration_vertex_stream_.back_vertex_data);

		if (std::ssize(decoration_vertex_stream_.back_vertex_data) > back_decoration_count)
			reload_vertex_buffer_ = true;
	}
}


//Protected

/*
	Events
*/

void DrawableText::OpacityChanged() noexcept
{
	reload_vertex_streams_ = true;
}


//Public

DrawableText::DrawableText(NonOwningPtr<fonts::Text> text, bool visible) :
	DrawableText{vector3::Zero, 0.0_r, text, visible}
{
	//Empty
}

DrawableText::DrawableText(const Vector3 &position, NonOwningPtr<fonts::Text> text, bool visible) :
	DrawableText{position, 0.0_r, text, visible}
{
	//Empty
}

DrawableText::DrawableText(const Vector3 &position, real rotation, NonOwningPtr<fonts::Text> text, bool visible) :
	
	DrawableObject{visible},

	position_{position},
	rotation_{rotation},

	text_{text ? std::make_optional(*text) : std::nullopt},
	initial_text_{text},

	reload_vertex_streams_{!!text_}
{
	query_type_flags_ |= query::scene_query::QueryType::Text;
}


/*
	Modifiers
*/

void DrawableText::Revert()
{
	if (initial_text_)
		text_ = *initial_text_;
}


/*
	Preparing / drawing
*/

void DrawableText::Prepare() noexcept
{
	if (!text_)
		return;

	auto pixel_unit_ratio = vector2::UnitScale;

	if (reload_vertex_streams_)
	{
		//Adjust size from viewport to ortho space
		if (auto viewport = Owner()->ConnectedViewport(); viewport)
			pixel_unit_ratio = viewport->ViewportToOrthoRatio();

		PrepareVertexStreams(pixel_unit_ratio);
		reload_vertex_streams_ = false;
		update_bounding_volumes_ = true;
	}

	if (reload_vertex_buffer_)
	{
		if (!vbo_)
			vbo_.emplace(render::vertex::vertex_buffer_object::VertexBufferUsage::Dynamic);

		if (vbo_ && *vbo_)
		{
			if (!std::empty(glyph_vertex_streams_))
			{
				auto glyph_size = detail::vertex_components * 6;
				auto back_decoration_size = std::ssize(decoration_vertex_stream_.back_vertex_data);
				auto front_decoration_size = std::ssize(decoration_vertex_stream_.front_vertex_data);

				vbo_->Reserve((std::ssize(glyph_vertex_streams_) * glyph_size + back_decoration_size + front_decoration_size) * sizeof(real));

				{
					auto offset = 0;

					decoration_vertex_stream_.back_vertex_batch.VertexBuffer(vbo_->SubBuffer(offset * sizeof(real), back_decoration_size * sizeof(real)));
					offset += back_decoration_size;

					for (auto &stream : glyph_vertex_streams_)
					{
						stream.vertex_batch.VertexBuffer(vbo_->SubBuffer(offset * sizeof(real), glyph_size * sizeof(real)));
						offset += glyph_size;
					}
					
					decoration_vertex_stream_.front_vertex_batch.VertexBuffer(vbo_->SubBuffer(offset * sizeof(real), front_decoration_size * sizeof(real)));
				}
			}
		}

		reload_vertex_buffer_ = false;
	}

	decoration_vertex_stream_.back_vertex_batch.Prepare();

	for (auto &stream : glyph_vertex_streams_)
		stream.vertex_batch.Prepare();

	decoration_vertex_stream_.front_vertex_batch.Prepare();

	if (update_bounding_volumes_)
	{
		auto [aabb, obb, sphere] =
			detail::generate_bounding_volumes(*text_, position_, rotation_, pixel_unit_ratio);
		aabb_ = aabb;
		obb_ = obb;
		sphere_ = sphere;

		update_bounding_volumes_ = false;
	}
}


void DrawableText::Draw(shaders::ShaderProgram *shader_program) noexcept
{
	if (visible_ && text_ && !std::empty(glyph_vertex_streams_))
	{
		auto use_shader = shader_program && shader_program->Owner() && shader_program->Handle();
		auto shader_in_use = use_shader && shader_program->Owner()->IsShaderProgramActive(*shader_program);

		if (use_shader && !shader_in_use)
			shader_program->Owner()->ActivateShaderProgram(*shader_program);

		decoration_vertex_stream_.back_vertex_batch.Draw(shader_program);
			//Draw back decorations before character glyphs

		for (auto &stream : glyph_vertex_streams_)
			stream.vertex_batch.Draw(shader_program);

		decoration_vertex_stream_.front_vertex_batch.Draw(shader_program);
			//Draw front decorations after character glyphs

		if (use_shader && !shader_in_use)
			shader_program->Owner()->DeactivateShaderProgram(*shader_program);
	}
}


/*
	Elapse time
*/

void DrawableText::Elapse(duration time) noexcept
{
	if (text_)
	{
		decoration_vertex_stream_.back_vertex_batch.Elapse(time);

		for (auto &stream : glyph_vertex_streams_)
			stream.vertex_batch.Elapse(time);

		decoration_vertex_stream_.front_vertex_batch.Elapse(time);
	}
}

} //ion::graphics::scene