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

TextBlocks html_to_formatted_blocks(std::string_view content)
{
	return utilities::HTMLToTextBlocks(content);
}

MeasuredTextLines formatted_blocks_to_formatted_lines(TextBlocks text_blocks,
	const std::optional<Vector2> &area_size, const Vector2 &padding, TypeFace &type_face)
{
	using namespace graphics::utilities;

	//Word wrap text blocks to area size
	if (area_size)
	{
		auto max_width = static_cast<int>(text_area_max_size(*area_size, padding).X());
		text_blocks = std::move(
			utilities::WordWrap(std::move(text_blocks), max_width, type_face).
			value_or(TextBlocks{})
		);
	}

	MeasuredTextLines formatted_lines;
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

text::TextBlocks Text::MakeFormattedBlocks(std::string_view content) const
{
	if (formatting_ == TextFormatting::HTML)
		return detail::html_to_formatted_blocks(content);
	else
		return {{{}, std::string{content}}};
}

text::MeasuredTextLines Text::MakeFormattedLines(text::TextBlocks text_blocks,
	const std::optional<Vector2> &area_size, const Vector2 &padding,
	NonOwningPtr<TypeFace> type_face) const
{
	if (type_face)
		return detail::formatted_blocks_to_formatted_lines(std::move(text_blocks), area_size, padding, *type_face);
	else
		return {};
}


//Public

Text::Text(std::string name, std::string content, text::TextAlignment alignment,
	NonOwningPtr<TypeFace> type_face) :
	
	managed::ManagedObject<TextManager>{std::move(name)},

	content_{std::move(content)},
	alignment_{alignment},
	type_face_{type_face},

	formatted_blocks_{MakeFormattedBlocks(content_)},
	formatted_lines_{MakeFormattedLines(formatted_blocks_, area_size_, padding_, type_face_)}
{
	//Empty
}

Text::Text(std::string name, std::string content, NonOwningPtr<TypeFace> type_face) :

	Text{std::move(name), std::move(content), text::TextAlignment::Left, type_face}
{
	//Empty
}

Text::Text(std::string name, std::string content, text::TextFormatting formatting,
	text::TextAlignment alignment, text::TextVerticalAlignment vertical_alignment,
	const std::optional<Vector2> &area_size, const Vector2 &padding,
	std::optional<real> line_height_factor, NonOwningPtr<TypeFace> type_face) :

	managed::ManagedObject<TextManager>{std::move(name)},

	content_{std::move(content)},
	formatting_{formatting},
	alignment_{alignment},
	vertical_alignment_{vertical_alignment},
	area_size_{area_size},
	padding_{padding},
	line_height_factor_{line_height_factor.value_or(detail::default_line_height_factor)},
	type_face_{type_face},

	formatted_blocks_{MakeFormattedBlocks(content_)},
	formatted_lines_{MakeFormattedLines(formatted_blocks_, area_size_, padding_, type_face_)}
{
	//Empty
}

Text::Text(std::string name, std::string content,
	text::TextAlignment alignment, text::TextVerticalAlignment vertical_alignment,
	const std::optional<Vector2> &area_size, const Vector2 &padding,
	std::optional<real> line_height_factor, NonOwningPtr<TypeFace> type_face) :

	Text{std::move(name), std::move(content), text::TextFormatting::HTML,
		 alignment, vertical_alignment,
		 area_size, padding, line_height_factor, type_face}
{
	//Empty
}

Text::Text(std::string name, std::string content,
	const std::optional<Vector2> &area_size, const Vector2 &padding,
	std::optional<real> line_height_factor, NonOwningPtr<TypeFace> type_face) :

	Text{std::move(name), std::move(content), text::TextFormatting::HTML,
		 text::TextAlignment::Left, text::TextVerticalAlignment::Top,
		 area_size, padding, line_height_factor, type_face}
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
		formatted_blocks_ = MakeFormattedBlocks(content_);
		formatted_lines_ = MakeFormattedLines(formatted_blocks_, area_size_, padding_, type_face_);
	}
}

void Text::Formatting(text::TextFormatting formatting)
{
	if (formatting_ != formatting)
	{
		formatting_ = formatting;
		formatted_blocks_ = MakeFormattedBlocks(content_);
		formatted_lines_ = MakeFormattedLines(formatted_blocks_, area_size_, padding_, type_face_);
	}
}


void Text::AreaSize(const std::optional<Vector2> &area_size)
{
	if (area_size_ != area_size)
	{
		area_size_ = area_size;
		formatted_lines_ = MakeFormattedLines(formatted_blocks_, area_size_, padding_, type_face_);
	}
}

void Text::Padding(const Vector2 &padding)
{
	if (padding_ != padding)
	{
		padding_ = padding;
		formatted_lines_ = MakeFormattedLines(formatted_blocks_, area_size_, padding_, type_face_);
	}
}

void Text::LineHeight(real height) noexcept
{
	if (type_face_ && type_face_->HasRegularFont())
	{
		if (auto size = type_face_->RegularFont()->Size(); size > 0)
			LineHeightFactor(height / size);
	}
}


void Text::Lettering(NonOwningPtr<TypeFace> type_face) noexcept
{
	if (type_face)
		formatted_lines_ = MakeFormattedLines(formatted_blocks_, area_size_, padding_, type_face_);
	else
	{
		formatted_lines_.clear();
		formatted_lines_.shrink_to_fit();
	}

	type_face_ = type_face;
}


/*
	Observers
*/

std::optional<real> Text::LineHeight() const noexcept
{
	if (type_face_ && type_face_->HasRegularFont())
	{
		auto factor = line_height_factor_ >= 0.0_r ? line_height_factor_ : 0.0_r;
		return type_face_->RegularFont()->Size() * factor;
	}
	else
		return {};
}


NonOwningPtr<TypeFace> Text::Lettering() noexcept
{
	return type_face_;
}

NonOwningPtr<const TypeFace> Text::Lettering() const noexcept
{
	return type_face_;
}


/*
	Content
*/

void Text::AppendContent(std::string_view content)
{
	if (std::empty(content))
		return;

	else if (std::empty(content_))
	{
		Content(std::string{content});
		return;
	}
	else if (content.front() == '\n')
	{
		content.remove_prefix(1);
		AppendLine(content); //Faster
		return;
	}

	content_ += content;
	auto formatted_blocks = MakeFormattedBlocks(content);
	auto iter = std::begin(formatted_blocks);

	//Merge first block with the last one already in text
	if (formatted_blocks.front() == formatted_blocks_.back())
	{
		formatted_blocks_.back().Content += formatted_blocks.front().Content;
		++iter; //Skip first
	}

	std::move(iter, std::end(formatted_blocks), std::back_inserter(formatted_blocks_));
	formatted_lines_ = MakeFormattedLines(formatted_blocks_, area_size_, padding_, type_face_);
}

void Text::PrependContent(std::string_view content)
{
	if (std::empty(content))
		return;

	else if (std::empty(content_))
	{
		Content(std::string{content});
		return;
	}
	else if (content.back() == '\n')
	{
		content.remove_suffix(1);
		PrependLine(content); //Faster
		return;
	}

	content_.insert(0, content);
	auto formatted_blocks = MakeFormattedBlocks(content);
	auto iter = std::end(formatted_blocks);

	//Merge last block with the first one already in text
	if (formatted_blocks.back() == formatted_blocks_.front())
	{
		formatted_blocks_.front().Content.insert(0, formatted_blocks.back().Content);
		--iter; //Skip last
	}

	std::move(std::begin(formatted_blocks), iter,
		std::inserter(formatted_blocks_, std::begin(formatted_blocks_)));
	formatted_lines_ = MakeFormattedLines(formatted_blocks_, area_size_, padding_, type_face_);
}

void Text::AppendLine(std::string_view content)
{
	if (std::empty(content_))
	{
		Content(std::string{content});
		return;
	}

	content_ += "\n" + std::string{content};
	auto formatted_blocks = MakeFormattedBlocks(content);
	auto formatted_lines = MakeFormattedLines(formatted_blocks, area_size_, padding_, type_face_);
	formatted_blocks.insert(std::begin(formatted_blocks), {{}, "\n"});

	std::move(std::begin(formatted_blocks), std::end(formatted_blocks),
		std::back_inserter(formatted_blocks_));
	std::move(std::begin(formatted_lines), std::end(formatted_lines),
		std::back_inserter(formatted_lines_));
}

void Text::PrependLine(std::string_view content)
{
	if (std::empty(content_))
	{
		Content(std::string{content});
		return;
	}

	content_.insert(0, std::string{content} + "\n");
	auto formatted_blocks = MakeFormattedBlocks(content);
	auto formatted_lines = MakeFormattedLines(formatted_blocks, area_size_, padding_, type_face_);
	formatted_blocks.push_back({{}, "\n"});

	std::move(std::begin(formatted_blocks), std::end(formatted_blocks),
		std::inserter(formatted_blocks_, std::begin(formatted_blocks_)));
	std::move(std::begin(formatted_lines), std::end(formatted_lines),
		std::inserter(formatted_lines_, std::begin(formatted_lines_)));
}


void Text::Clear() noexcept
{
	content_.clear();
	formatted_blocks_.clear();
	formatted_lines_.clear();

	content_.shrink_to_fit();
	formatted_blocks_.shrink_to_fit();
	formatted_lines_.shrink_to_fit();
}


/*
	Unformatted
*/

std::string Text::UnformattedContent() const
{
	return utilities::detail::text_blocks_to_string(formatted_blocks_);
}

std::string Text::UnformattedWrappedContent() const
{
	//One or more lines to display
	if (!std::empty(formatted_lines_))
	{
		//First
		auto content = utilities::detail::text_blocks_to_string(formatted_lines_.front().first.Blocks);

		//Rest
		for (auto iter = std::begin(formatted_lines_) + 1,
			end = std::end(formatted_lines_); iter != end; ++iter)
			content += "\n" + utilities::detail::text_blocks_to_string(iter->first.Blocks);

		return content;
	}
	//Nothing to display
	//Could be missing/invalid type face
	else
		return "";
}

std::string Text::UnformattedDisplayedContent() const
{
	auto max_lines = max_lines_.value_or(std::ssize(formatted_lines_));

	if (area_size_)
	{
		if (auto line_height = LineHeight();
			line_height && *line_height > 0.0_r)
		{
			auto area_max_lines = detail::text_area_max_lines(*area_size_, padding_, *line_height);
			max_lines = std::min(max_lines, area_max_lines);
		}
	}

	//One or more lines to display
	if (!std::empty(formatted_lines_) &&
		from_line_ < std::ssize(formatted_lines_) && max_lines > 0)
	{
		if (from_line_ + max_lines > std::ssize(formatted_lines_))
			max_lines = std::ssize(formatted_lines_) - from_line_;

		auto iter = std::begin(formatted_lines_) + from_line_;
		auto end = iter + max_lines;

		//First
		auto content = utilities::detail::text_blocks_to_string(iter->first.Blocks);

		//Rest
		for (++iter; iter != end; ++iter)
			content += "\n" + utilities::detail::text_blocks_to_string(iter->first.Blocks);

		return content;
	}
	//Nothing to display
	//Could be missing/invalid type face
	else
		return "";
}

} //ion::graphics::fonts