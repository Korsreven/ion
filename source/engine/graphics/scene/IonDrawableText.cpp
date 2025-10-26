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
#include <utility>

#include "IonEngine.h"
#include "graphics/fonts/IonFont.h"
#include "graphics/fonts/IonTextManager.h"
#include "graphics/fonts/IonTypeFace.h"
#include "graphics/fonts/utilities/IonFontUtility.h"
#include "graphics/render/vertex/IonVertexBatch.h"
#include "graphics/scene/shapes/IonSprite.h"
#include "graphics/utilities/IonObb.h"
#include "query/IonSceneQuery.h"

#undef min

namespace ion::graphics::scene
{

using namespace drawable_text;
using namespace graphics::utilities;

namespace drawable_text::detail
{

//text_primitive
//Protected

/*
	Events
*/

void text_primitive::RenderPassesChanged() noexcept
{
	if (owner)
		owner->NotifyRenderPassesChanged(*this);
}


//Public

text_primitive::text_primitive() :
	render::RenderPrimitive{render::vertex::vertex_batch::VertexDrawMode::Triangles, get_vertex_declaration()}
{
	//Empty
}

text_primitive::text_primitive(textures::texture::TextureHandle texture_handle) :
	text_primitive()
{
	RenderTexture(texture_handle);
}

text_primitive::text_primitive(NonOwningPtr<materials::Material> material) :
	text_primitive()
{
	RenderMaterial(material);
}


//text_glyph_primitive
//Public

text_glyph_primitive::text_glyph_primitive(textures::texture::TextureHandle texture_handle) :
	text_primitive{texture_handle}
{
	//Empty
}


//text_decoration_primitive
//Public

text_decoration_primitive::text_decoration_primitive()
{
	//Empty
}


//text_image_primitive
//Public

text_image_primitive::text_image_primitive(NonOwningPtr<materials::Material> material) :
	text_primitive(material)
{
	//Empty
}


//text_glyph_primitive_key
//Public

bool text_glyph_primitive_key::operator<(const text_glyph_primitive_key &key) const noexcept
{
	return std::pair{font, glyph_index} < std::pair{key.font, key.glyph_index};
}


//text_image_primitive_key
//Public

bool text_image_primitive_key::operator<(const text_image_primitive_key &key) const noexcept
{
	return material < key.material;
}


std::tuple<Aabb, Obb, Sphere> generate_bounding_volumes(const fonts::Text &text,
	const Vector2 &position, real rotation) noexcept
{
	auto ppu = Engine::PixelsPerUnit();
	auto size = text.DisplayedSize() / ppu;
	auto [half_width, half_height] = (size * 0.5_r).XY();
	auto [x, y] = position.XY();

	if (text.AreaSize())
	{
		auto area_size = *text.AreaSize() / ppu;
		auto [area_half_width, area_half_height] = (area_size * 0.5_r).XY();

		//Adjust x horizontally
		switch (text.Alignment())
		{
			case fonts::text::TextAlignment::Left:
			x -= area_half_width;
			break;

			case fonts::text::TextAlignment::Right:
			x += area_half_width;
			break;
		}

		//Adjust y vertically
		switch (text.VerticalAlignment())
		{
			case fonts::text::TextVerticalAlignment::Top:
			y += area_half_height;
			break;

			case fonts::text::TextVerticalAlignment::Bottom:
			y -= area_half_height;
			break;
		}
	}

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

	auto aabb = Aabb::Size(size, {x, y}).RotateCopy(rotation);
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

std::optional<fonts::text::TextDecorationLine> get_text_decoration_line(const fonts::text::TextBlock &text_block, const fonts::Text &text) noexcept
{
	return text_block.DecorationLine ? text_block.DecorationLine : text.DefaultDecorationLine();
}

std::optional<fonts::text::TextDecorationStyle> get_text_decoration_style(const fonts::text::TextBlock &text_block, const fonts::Text &text) noexcept
{
	return text_block.DecorationStyle ? text_block.DecorationStyle : text.DefaultDecorationStyle();
}

std::optional<Color> get_text_decoration_color(const fonts::text::TextBlock &text_block, const fonts::Text &text) noexcept
{
	return text_block.DecorationColor ? text_block.DecorationColor : text.DefaultDecorationColor();
}


fonts::Font* get_default_font(const fonts::Text &text) noexcept
{
	if (auto type_face = text.Lettering(); type_face)
		return fonts::utilities::detail::get_font(*type_face, {});
	else
		return nullptr;
}

fonts::Font* get_default_font(const fonts::text::TextBlock &text_block, const fonts::Text &text) noexcept
{
	if (auto type_face = text.Lettering(); type_face)
		return fonts::utilities::detail::get_font(*type_face, get_font_style(text_block, text));
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


render::render_primitive::VertexContainer get_glyph_vertex_data(real glyph_index, const fonts::font::GlyphMetric &metric,
	const Vector3 &position, real rotation, const Vector2 &scaling, const Color &color, const Vector3 &origin,
	bool sub_pixel_correction)
{
	auto [x, y, z] = position.XYZ();
	auto [r, g, b, a] = color.RGBA();

	auto s = static_cast<real>(metric.Width) / metric.ActualWidth;
	auto t = static_cast<real>(metric.Height) / metric.ActualHeight;

	x += metric.Left;
	y += metric.Top - metric.Height;
	auto width = metric.Width * scaling.X();
	auto height = metric.Height * scaling.Y();

	//Correct for sub-pixel rendering (glyphs may appear blurry if positioned off-pixel)
	if (sub_pixel_correction)
	{
		x = std::floor(x);
		y = std::floor(y);
		width = std::ceil(width);
		height = std::ceil(height);
	}

	auto ppu = Engine::PixelsPerUnit();
	x /= ppu;
	y /= ppu;
	width /= ppu;
	height /= ppu;

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
	//s, t, u

	return
		{
			//Vertex #1
			v1.X(), v1.Y(), v1.Z(),
			r, g, b, a,
			0.0_r, 0.0_r, glyph_index,

			//Vertex #2
			v2.X(), v2.Y(), v2.Z(),
			r, g, b, a,
			0.0_r, t, glyph_index,

			//Vertex #3
			v3.X(), v3.Y(), v3.Z(),
			r, g, b, a,
			s, t, glyph_index,

			//Vertex #4
			v3.X(), v3.Y(), v3.Z(),
			r, g, b, a,
			s, t, glyph_index,

			//Vertex #5
			v4.X(), v4.Y(), v4.Z(),
			r, g, b, a,
			s, 0.0_r, glyph_index,

			//Vertex #6
			v1.X(), v1.Y(), v1.Z(),
			r, g, b, a,
			0.0_r, 0.0_r, glyph_index
		};
}

render::render_primitive::VertexContainer get_decoration_vertex_data(
	const Vector3 &position, real rotation, const Vector2 &size, const Color &color, const Vector3 &origin,
	real delta_z, bool sub_pixel_correction)
{
	auto [x, y, z] = position.XYZ();
	auto [width, height] = size.XY();
	auto [r, g, b, a] = color.RGBA();

	z += delta_z;

	//Correct for sub-pixel rendering  (decorations may appear blurry if positioned off-pixel)
	if (sub_pixel_correction)
	{
		x = std::floor(x);
		y = std::floor(y);
		width = std::ceil(width);
		height = std::ceil(height);
	}

	auto ppu = Engine::PixelsPerUnit();
	x /= ppu;
	y /= ppu;
	width /= ppu;
	height /= ppu;

	auto v1 = Vector3{x, y + height, z}.RotateCopy(rotation, origin);
	auto v2 = Vector3{x, y, z}.RotateCopy(rotation, origin);
	auto v3 = Vector3{x + width, y, z}.RotateCopy(rotation, origin);
	auto v4 = Vector3{x + width, y + height, z}.RotateCopy(rotation, origin);

	//Vertex format:
	//x, y, z
	//r, g, b, a
	//s, t, u

	return
		{
			//Vertex #1
			v1.X(), v1.Y(), v1.Z(),
			r, g, b, a,
			0.0_r, 1.0_r, 0.0_r,

			//Vertex #2
			v2.X(), v2.Y(), v2.Z(),
			r, g, b, a,
			0.0_r, 0.0_r, 0.0_r,

			//Vertex #3
			v3.X(), v3.Y(), v3.Z(),
			r, g, b, a,
			1.0_r, 0.0_r, 0.0_r,

			//Vertex #4
			v3.X(), v3.Y(), v3.Z(),
			r, g, b, a,
			1.0_r, 0.0_r, 0.0_r,

			//Vertex #5
			v4.X(), v4.Y(), v4.Z(),
			r, g, b, a,
			1.0_r, 1.0_r, 0.0_r,

			//Vertex #6
			v1.X(), v1.Y(), v1.Z(),
			r, g, b, a,
			0.0_r, 1.0_r, 0.0_r
		};
}

render::render_primitive::VertexContainer get_image_vertex_data(
	const Vector3 &position, real rotation, const Vector2 &size, const Color &color, const Vector3 &origin,
	bool sub_pixel_correction)
{
	return get_decoration_vertex_data(position, rotation, size, color, origin, 0.0_r, sub_pixel_correction);
}


void get_block_primitives(const fonts::text::TextBlock &text_block, const fonts::Text &text,
	int font_size, int &glyph_count, Vector3 &position, real rotation, const Vector3 &origin,
	text_glyph_primitives &glyph_primitives, text_decoration_primitives &decoration_primitives,
	text_image_primitives &image_primitives, text_tooltip_elements &tooltip_elements,
	bool sub_pixel_correction)
{
	if (auto font = get_default_font(text_block, text); font)
	{
		if (auto &handle = font->GlyphHandle(); handle)
		{
			if (auto &metrics = font->GlyphMetrics(); metrics)
			{
				auto base_x = position.X();
				auto base_y = position.Y();
				position.Y(position.Y() + font_size * fonts::utilities::detail::get_text_block_translate_factor(text_block));
				auto scaling = fonts::utilities::detail::get_text_block_scale_factor(text_block);
				auto foreground_color = get_foreground_color(text_block, text);

				//Background (back decoration)
				if (auto background_color = get_background_color(text_block, text); background_color)
				{
					auto background_size = fonts::utilities::detail::get_text_decoration_background_size(font_size);

					auto decoration_position = Vector3{position.X(), base_y - (background_size - font_size) * 0.5_r, position.Z()};
					auto decoration_size = Vector2{text_block.Size->X(), background_size};

					auto vertex_data = get_decoration_vertex_data(
						decoration_position, rotation, decoration_size, *background_color, origin,
						-Engine::ZEpsilon(), sub_pixel_correction);

					//New primitive
					if (!decoration_primitives.second)
						decoration_primitives.second = make_owning<text_decoration_primitive>();

					decoration_primitives.second->vertex_data.insert(
						std::end(decoration_primitives.second->vertex_data),
						std::begin(vertex_data), std::end(vertex_data));
				}
				
				//Text decoration line (front/back decoration)
				if (auto decoration_line = get_text_decoration_line(text_block, text); decoration_line)
				{
					auto line_thickness = fonts::utilities::detail::get_text_decoration_line_thickness(font_size);
					auto line_margin = fonts::utilities::detail::get_text_decoration_line_margin(font_size);
					auto line_style = get_text_decoration_style(text_block, text);
					
					auto decoration_size = Vector2{text_block.Size->X(), line_thickness};
					auto decoration_color = get_text_decoration_color(text_block, text).value_or(foreground_color);
					auto front_decoration = *decoration_line == fonts::text::TextDecorationLine::LineThrough;

					auto decoration_delta_z = front_decoration ?
						Engine::ZEpsilon() :
						-Engine::ZEpsilon();
					auto &decoration_primitive = front_decoration ?
						decoration_primitives.first :
						decoration_primitives.second;

					//Go through each line set in decoration line
					for (auto mask = decoration_line->Value; mask; mask &= mask - 1)
					{
						auto line = static_cast<fonts::text::TextDecorationLine::Flags>(mask & (~mask + 1));
						auto decoration_position =
							[&]() noexcept -> Vector3
							{
								switch (line)
								{
									case fonts::text::TextDecorationLine::Underline:
									return {position.X(), position.Y() - (line_margin + line_thickness), position.Z()};

									case fonts::text::TextDecorationLine::LineThrough:
									return {position.X(), position.Y() + font_size * scaling * 0.5_r - line_thickness, position.Z()};

									case fonts::text::TextDecorationLine::Overline:
									return {position.X(), position.Y() + font_size * scaling + (line_margin + line_thickness), position.Z()};
								}

								return position;
							}();

						if (line_style == fonts::text::TextDecorationStyle::Dotted ||
							line_style == fonts::text::TextDecorationStyle::Dashed)
						{
							auto line_size = line_style == fonts::text::TextDecorationStyle::Dotted ?
								fonts::utilities::detail::get_text_decoration_dotted_line_size(font_size) :
								fonts::utilities::detail::get_text_decoration_dashed_line_size(font_size);
							auto line_spacing = line_style == fonts::text::TextDecorationStyle::Dotted ?
								fonts::utilities::detail::get_text_decoration_dotted_line_spacing(font_size) :
								fonts::utilities::detail::get_text_decoration_dashed_line_spacing(font_size);
							auto count = static_cast<int>(std::max(1.0_r, decoration_size.X() / (line_size + line_spacing)));

							auto vertex_data = get_decoration_vertex_data(
								decoration_position, rotation, {line_size, decoration_size.Y()}, decoration_color, origin,
								decoration_delta_z, sub_pixel_correction);

							//New primitive
							if (!decoration_primitive)
								decoration_primitive = make_owning<text_decoration_primitive>();

							decoration_primitive->vertex_data.insert(
								std::end(decoration_primitive->vertex_data),
								std::begin(vertex_data), std::end(vertex_data));

							for (auto i = 1; i < count; ++i)
							{
								decoration_position.X(decoration_position.X() + line_size + line_spacing);

								vertex_data = get_decoration_vertex_data(
									decoration_position, rotation, {line_size, decoration_size.Y()}, decoration_color, origin,
									decoration_delta_z, sub_pixel_correction);

								decoration_primitive->vertex_data.insert(
									std::end(decoration_primitive->vertex_data),
									std::begin(vertex_data), std::end(vertex_data));
							}
						}
						else //Solid or double
						{
							auto vertex_data = get_decoration_vertex_data(
								decoration_position, rotation, decoration_size, decoration_color, origin,
								decoration_delta_z, sub_pixel_correction);

							//New primitive
							if (!decoration_primitive)
								decoration_primitive = make_owning<text_decoration_primitive>();

							decoration_primitive->vertex_data.insert(
								std::end(decoration_primitive->vertex_data),
								std::begin(vertex_data), std::end(vertex_data));

							//One more solid line
							if (line_style == fonts::text::TextDecorationStyle::Double)
							{
								auto line_spacing = fonts::utilities::detail::get_text_decoration_double_line_spacing(font_size);
								auto line_offset = line == fonts::text::TextDecorationLine::Overline ?
									decoration_size.Y() + line_spacing :
									-decoration_size.Y() - line_spacing;

								vertex_data = get_decoration_vertex_data(
									{decoration_position.X(), decoration_position.Y() + line_offset, decoration_position.Z()},
									rotation, decoration_size, decoration_color, origin,
									decoration_delta_z, sub_pixel_correction);

								decoration_primitive->vertex_data.insert(
									std::end(decoration_primitive->vertex_data),
									std::begin(vertex_data), std::end(vertex_data));
							}
						}
					}
				}

				//Text image
				if (text_block.Image)
				{
					auto material = fonts::text_manager::detail::get_material(text_block.Image->Source, text.Owner()->MaterialManagers());
					auto texture_size = material ? shapes::sprite::detail::get_texture_size(*material) : std::nullopt;

					//Calculate image size
					auto image_square_size = fonts::utilities::detail::get_text_image_default_size(font_size);

					if (text_block.Image->Width || text_block.Image->Height)
					{
						image_square_size = std::min(
							std::max(text_block.Image->Width.value_or(0.0_r), text_block.Image->Height.value_or(0.0_r)),
							fonts::utilities::detail::get_text_image_max_size(font_size)
						);
						
						//Use custom aspect ratio
						if (text_block.Image->Width && text_block.Image->Height)
							texture_size = {*text_block.Image->Width, *text_block.Image->Height};
					}

					auto image_size = texture_size.value_or(image_square_size);

					//Scale to fit (keep aspect ratio)
					if (image_size.Max() != image_square_size)
						image_size *= image_square_size / image_size.Max();

					auto image_x = position.X() +
						text_block.Size->X() * 0.5_r -
						image_size.X() * 0.5_r;
					auto image_position = Vector3{image_x, base_y - (image_size.Y() - font_size) * 0.5_r, position.Z()};

					auto vertex_data =
						get_image_vertex_data(
							image_position, rotation, image_size, foreground_color, origin,
							sub_pixel_correction);
					auto key = text_image_primitive_key{material.get()};
						//Group on material

					auto iter = std::end(image_primitives);

					//New primitive
					if (iter = image_primitives.find(key); iter == std::end(image_primitives))
						iter = image_primitives.emplace(std::make_pair(key, make_owning<text_image_primitive>(material))).first;

					iter->second->vertex_data.insert(std::end(iter->second->vertex_data),
						std::begin(vertex_data), std::end(vertex_data));

					position.X(position.X() + text_block.Size->X());
				}
				else
				{
					//For each character
					for (auto c : text_block.Content)
					{
						if (auto glyph_index = fonts::utilities::detail::get_glyph_index(c, *metrics);
							glyph_index < std::size(*metrics))
						{
							auto vertex_data =
								get_glyph_vertex_data(glyph_index, (*metrics)[glyph_index],
									position, rotation, scaling, foreground_color, origin,
									sub_pixel_correction);
							auto iter = std::end(glyph_primitives);

							if (handle->Type == textures::texture::TextureType::ArrayTexture2D)
							{
								auto key = text_glyph_primitive_key{font};
									//Group on font

								//New primitive
								if (iter = glyph_primitives.find(key); iter == std::end(glyph_primitives))
									iter = glyph_primitives.emplace(std::make_pair(key, make_owning<text_glyph_primitive>((*handle)[0]))).first;
							}
							else
							{
								auto key = text_glyph_primitive_key{font, glyph_index};
									//Group on font and glyph index

								//New primitive
								if (iter = glyph_primitives.find(key); iter == std::end(glyph_primitives))
									iter = glyph_primitives.emplace(std::make_pair(key, make_owning<text_glyph_primitive>((*handle)[glyph_index]))).first;
							}

							iter->second->vertex_data.insert(std::end(iter->second->vertex_data),
								std::begin(vertex_data), std::end(vertex_data));

							position.X(position.X() + (*metrics)[glyph_index].Advance * scaling);
							++glyph_count;
						}
					}
				}

				//Text tooltip
				if (text_block.Title)
				{
					auto element_size = fonts::utilities::detail::get_text_decoration_background_size(font_size);

					auto ppu = Engine::PixelsPerUnit();
					auto min = Vector2{base_x, base_y - (element_size - font_size) * 0.5_r} / ppu;
					auto max = min + Vector2{text_block.Size->X(), element_size} / ppu;

					auto tl = Vector2{min.X(), max.Y()}.RotateCopy(rotation, origin);
					auto tr = max.RotateCopy(rotation, origin);
					auto bl = min.RotateCopy(rotation, origin);
					auto br = Vector2{max.X(), min.Y()}.RotateCopy(rotation, origin);

					min = tl.FloorCopy(tr).FloorCopy(bl).FloorCopy(br);
					max = tl.CeilCopy(tr).CeilCopy(bl).CeilCopy(br);
					
					tooltip_elements.push_back({Aabb{min, max}, *text_block.Title});
				}

				position.Y(base_y);
			}
		}
	}
}

void get_text_primitives(const fonts::Text &text, Vector3 position, real rotation,
	text_glyph_primitives &glyph_primitives, text_decoration_primitives &decoration_primitives,
	text_image_primitives &image_primitives, text_tooltip_elements &tooltip_elements,
	bool sub_pixel_correction)
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

		auto ppu = Engine::PixelsPerUnit();
		position *= Vector3{ppu, ppu, 1.0_r};

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
				get_block_primitives(block, text,
					font_size, glyph_count, glyph_position, rotation, origin,
					glyph_primitives, decoration_primitives, image_primitives,
					tooltip_elements, sub_pixel_correction);

			glyph_position.Y(glyph_position.Y() - *line_height); //Next glyph y position
		}
	}
}

} //drawable_text::detail


//Private

void DrawableText::ReloadPrimitives()
{
	render_primitives_.clear();
	tooltip_elements_.clear();

	if (text_)
	{
		text_->Owner(*initial_text_->Owner());
		detail::get_text_primitives(*text_, position_, rotation_,
			glyph_primitives_, decoration_primitives_, image_primitives_,
			tooltip_elements_, sub_pixel_correction_);
		text_->Owner(nullptr);
	}

	//Glyphs
	glyph_primitives_.erase_if(
		[&](auto &primitive) noexcept
		{
			if (!std::empty(primitive.second->vertex_data))
			{
				primitive.second->owner = this;
				primitive.second->VertexData(std::move(primitive.second->vertex_data));
				AddPrimitive(*primitive.second);
				return false; //Keep
			}
			else
				return true;
		});

	//Back decoration
	if (decoration_primitives_.first)
	{
		if (!std::empty(decoration_primitives_.first->vertex_data))
		{
			decoration_primitives_.first->owner = this;
			decoration_primitives_.first->VertexData(std::move(decoration_primitives_.first->vertex_data));
			AddPrimitive(*decoration_primitives_.first);
		}
		else
			decoration_primitives_.first.reset();
	}

	//Front decoration
	if (decoration_primitives_.second)
	{
		if (!std::empty(decoration_primitives_.second->vertex_data))
		{
			decoration_primitives_.second->owner = this;
			decoration_primitives_.second->VertexData(std::move(decoration_primitives_.second->vertex_data));
			AddPrimitive(*decoration_primitives_.second);
		}
		else
			decoration_primitives_.second.reset();
	}

	//Images
	image_primitives_.erase_if(
		[&](auto &primitive) noexcept
		{
			if (!std::empty(primitive.second->vertex_data))
			{
				primitive.second->owner = this;
				primitive.second->VertexData(std::move(primitive.second->vertex_data));
				AddPrimitive(*primitive.second);
				return false; //Keep
			}
			else
				return true;
		});
}


//Public

DrawableText::DrawableText(std::optional<std::string> name,
	NonOwningPtr<fonts::Text> text, bool visible) :
	DrawableText{std::move(name), vector3::Zero, 0.0_r, text, visible}
{
	//Empty
}

DrawableText::DrawableText(std::optional<std::string> name, const Vector3 &position,
	NonOwningPtr<fonts::Text> text, bool visible) :
	DrawableText{std::move(name), position, 0.0_r, text, visible}
{
	//Empty
}

DrawableText::DrawableText(std::optional<std::string> name, const Vector3 &position, real rotation,
	NonOwningPtr<fonts::Text> text, bool visible) :
	
	DrawableObject{std::move(name), visible},

	position_{position},
	rotation_{rotation},

	text_{text ? std::make_optional(*text) : std::nullopt},
	initial_text_{text},

	reload_primitives_{!!text_}
{
	query_type_flags_ |= query::scene_query::QueryType::Text;
}


/*
	Modifiers
*/

void DrawableText::Revert()
{
	if (initial_text_)
	{
		text_ = *initial_text_;
		reload_primitives_ = true;
	}
}


/*
	Observers
*/

std::optional<std::string> DrawableText::IntersectsTooltipElement(const Vector2 &point) const noexcept
{
	if (auto node = ParentNode(); node)
	{
		for (auto &tooltip_element : tooltip_elements_)
		{
			//Check for intersection
			if (Aabb{tooltip_element.aabb}.Transform(Matrix3::Transformation(node->FullTransformation())).Intersects(point))
			{
				if (node->AxisAligned() ||
					Obb{tooltip_element.aabb}.Transform(Matrix3::Transformation(node->FullTransformation())).Intersects(point))
					return tooltip_element.title;
			}
		}
	}

	return {};
}


/*
	Preparing
*/

void DrawableText::Prepare()
{
	if (reload_primitives_)
	{
		ReloadPrimitives();
		reload_primitives_ = false;
		update_bounding_volumes_ = true;
	}

	//Prepare glyph primitives
	for (auto &primitive : glyph_primitives_)
		primitive.second->Prepare();

	//Prepare decoration primitives
	if (decoration_primitives_.first) //Front
		decoration_primitives_.first->Prepare();
	
	if (decoration_primitives_.second) //Back
		decoration_primitives_.second->Prepare();

	//Prepare image primitives
	for (auto &primitive : image_primitives_)
		primitive.second->Prepare();

	if (update_bounding_volumes_)
	{
		if (text_)
		{
			auto [aabb, obb, sphere] =
				detail::generate_bounding_volumes(*text_, position_, rotation_);
			aabb_ = aabb;
			obb_ = obb;
			sphere_ = sphere;
		}

		update_bounding_volumes_ = false;
	}

	DrawableObject::Prepare();
}

} //ion::graphics::scene