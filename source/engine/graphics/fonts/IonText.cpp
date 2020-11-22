/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/fonts
File:	IonText.cpp
-------------------------------------------
*/

#include "IonText.h"

#include "types/IonTypes.h"
#include "utilities/IonFontUtility.h"

namespace ion::graphics::fonts
{

using namespace text;
using namespace types::type_literals;

namespace text::detail
{

MeasuredTextLines string_to_formatted_lines(std::string_view content,
	const std::optional<Vector2> &area_size, const std::optional<Vector2> &padding, TypeFace &type_face)
{
	using namespace graphics::utilities;
	MeasuredTextLines formatted_lines;

	auto text_blocks =
		[&]() -> TextBlocks
		{
			//Make sure content is word wrapped inside area
			if (area_size)
			{
				auto max_width = static_cast<int>(
					(*area_size - padding.value_or(vector2::Zero) * 2.0_r).Ceil(vector2::Zero).X()
				);

				if (auto text_blocks = utilities::WordWrap(
					utilities::HTMLToTextBlocks(content), max_width, type_face);
					text_blocks)

					return *text_blocks;

				else
					return {};
			}
			else
				return utilities::HTMLToTextBlocks(content);
		}();

	for (auto lines = utilities::SplitTextBlocks(std::move(text_blocks));
		auto &line : lines)
	{
		auto size = utilities::MeasureTextBlocks(line.Blocks, type_face);
		formatted_lines.emplace_back(std::move(line), size.value_or(vector2::Zero));
	}

	return formatted_lines;
}

} //text::detail


//Private

text::MeasuredTextLines Text::GetFormattedLines() const
{
	if (formatting_ == TextFormatting::HTML && type_face_)
		return text::detail::string_to_formatted_lines(content_, area_size_, padding_, *type_face_.Object());
	else
		return {};
}


//Public

Text::Text(std::string name, std::string content, TypeFace &type_face) :

	managed::ManagedObject<TextManager>{std::move(name)},

	content_{std::move(content)},
	type_face_{type_face},
	formatted_lines_{GetFormattedLines()}
{
	//Empty
}


/*
	Modifiers
*/

void Text::Content(std::string content)
{
	if (content_ != content)
	{
		content_ = std::move(content);
		formatted_lines_ = GetFormattedLines();
	}
}

void Text::Formatting(text::TextFormatting formatting)
{
	if (formatting_ != formatting)
	{
		formatting_ = formatting;
		formatted_lines_ = GetFormattedLines();
	}
}


void Text::AreaSize(const std::optional<Vector2> &area_size)
{
	if (area_size_ != area_size)
	{
		area_size_ = area_size;
		formatted_lines_ = GetFormattedLines();
	}
}

void Text::Padding(const std::optional<Vector2> &padding)
{
	if (padding_ != padding)
	{
		padding_ = padding;
		formatted_lines_ = GetFormattedLines();
	}
}


void Text::Lettering(TypeFace &type_face)
{
	if (type_face_.Observe(type_face))
		formatted_lines_ = GetFormattedLines();
}

void Text::Lettering(std::nullptr_t) noexcept
{
	if (type_face_.Release())
		formatted_lines_.clear();
}


/*
	Observers
*/

TypeFace* Text::Lettering() noexcept
{
	return type_face_.Object();
}

const TypeFace* Text::Lettering() const noexcept
{
	return type_face_.Object();
}


/*
	Content
*/

void Text::AppendContent(std::string_view content)
{
	content_ += content;
}

void Text::PrependContent(std::string_view content)
{
	content_.insert(0, content);
}


/*
	Unformatted
*/

std::string Text::UnformattedContent() const
{
	if (formatting_ == TextFormatting::None)
		return content_;

	//HTML
	else if (!std::empty(formatted_lines_))
	{
		auto content = utilities::detail::text_blocks_to_string(formatted_lines_.front().first.Blocks);

		//Rest
		for (auto iter = std::begin(formatted_lines_) + 1,
			end = std::end(formatted_lines_); iter != end; ++iter)
			content += "\n" + utilities::detail::text_blocks_to_string(iter->first.Blocks);

		return content;
	}
	else
		return "";
}

} //ion::graphics::fonts