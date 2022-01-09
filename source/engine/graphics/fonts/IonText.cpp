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

TextLines formatted_blocks_to_formatted_lines(TextBlocks text_blocks, TextOverflow overflow,
	const std::optional<Vector2> &area_size, const Vector2 &padding, TypeFace &type_face)
{
	using namespace graphics::utilities;

	//Overflow text wider than area size
	if (area_size)
	{
		auto max_width = static_cast<int>(text_area_max_size(*area_size, padding).X());

		switch (overflow)
		{
			case TextOverflow::Truncate:
			text_blocks =
				utilities::TruncateTextBlocks(std::move(text_blocks), max_width, "", type_face).
				value_or(TextBlocks{});
			break;

			case TextOverflow::TruncateEllipsis:
			text_blocks =
				utilities::TruncateTextBlocks(std::move(text_blocks), max_width, type_face).
				value_or(TextBlocks{});
			break;

			case TextOverflow::WordTruncate:
			text_blocks =
				utilities::WordTruncate(std::move(text_blocks), max_width, type_face).
				value_or(TextBlocks{});
			break;

			case TextOverflow::WordWrap:
			default:
			text_blocks =
				utilities::WordWrap(std::move(text_blocks), max_width, type_face).
				value_or(TextBlocks{});
			break;
		}
	}

	auto lines = utilities::SplitTextBlocks(std::move(text_blocks));

	//Measure blocks and lines
	for (auto &line : lines)
	{
		line.Size = vector2::Zero;

		for (auto &block : line.Blocks)
		{
			auto size = utilities::MeasureTextBlock(block, type_face);
			block.Size = size.value_or(vector2::Zero);

			line.Size->X(line.Size->X() + block.Size->X());
			line.Size->Y(std::max(line.Size->Y(), block.Size->Y()));
		}
	}

	return lines;
}


int get_character_count(const TextBlocks &text_blocks) noexcept
{
	auto count = 0;

	for (auto &text_block : text_blocks)
		count += std::ssize(text_block.Content);

	return count;
}


std::pair<size_t, size_t> find_new_line(size_t off, std::string_view content, TextFormatting formatting) noexcept
{
	auto closest = std::pair{content.find('\n', off), 1U};

	if (formatting == TextFormatting::HTML)
	{
		if (auto br_off = content.substr(0, closest.first).find("<br>", off); br_off < closest.first)
			closest = {br_off, 4U};
	}

	return closest;
}

size_t get_content_offset(int line_off, std::string_view content, TextFormatting formatting) noexcept
{
	if (line_off <= 0)
		return 0;

	for (auto iter = std::pair{0U, 0U};
		(iter = find_new_line(iter.first, content, formatting)).first != std::string_view::npos;
		iter.first += iter.second)
	{
		if (--line_off == 0)
			return iter.first + iter.second;
	}

	return std::size(content);
}

size_t get_formatted_blocks_offset(int line_off, const TextBlocks &text_blocks) noexcept
{
	if (line_off <= 0)
		return 0;

	for (auto i = 0; auto &text_block : text_blocks)
	{
		if (text_block.HardBreak && --line_off == 0)
			return i + 1;
		else
			++i;
	}

	return std::size(text_blocks);
}

size_t get_formatted_lines_offset(int line_off, const TextLines &text_lines) noexcept
{
	if (line_off <= 0)
		return 0;

	for (auto i = 0; auto &text_line : text_lines)
	{
		if (text_line.Tail && --line_off == 0)
			return i + 1;
		else
			++i;
	}

	return std::size(text_lines);
}

int get_line_offset(size_t content_off, std::string_view content, TextFormatting formatting) noexcept
{
	if (content_off == 0)
		return 0;

	auto line_off = 0;
	for (auto iter = std::pair{0U, 0U};
		(iter = find_new_line(iter.first, content, formatting)).first != std::string_view::npos;
		iter.first += iter.second, ++line_off)
	{
		if (iter.first >= content_off)
			break;
	}

	return line_off;
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

text::TextLines Text::MakeFormattedLines(text::TextBlocks text_blocks,
	const std::optional<Vector2> &area_size, const Vector2 &padding,
	NonOwningPtr<TypeFace> type_face) const
{
	if (type_face)
		return detail::formatted_blocks_to_formatted_lines(
			std::move(text_blocks), overflow_, area_size, padding, *type_face);
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

void Text::Overflow(text::TextOverflow overflow) noexcept
{
	if (overflow_ != overflow)
	{
		overflow_ = overflow;
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

Vector2 Text::MinimumAreaSize() const noexcept
{
	using namespace graphics::utilities;

	//One or more lines to display
	if (auto max_lines = max_lines_.value_or(std::ssize(formatted_lines_));
		!std::empty(formatted_lines_) &&
		from_line_ < std::ssize(formatted_lines_) && max_lines > 0)
	{
		if (from_line_ + max_lines > std::ssize(formatted_lines_))
			max_lines = std::ssize(formatted_lines_) - from_line_;

		auto area_size = vector2::Zero;

		for (auto iter = std::begin(formatted_lines_) + from_line_,
			end = iter + max_lines; iter != end; ++iter)
			area_size.X(std::max(area_size.X(), iter->Size->X()));

		if (auto line_height = LineHeight();
			line_height && *line_height > 0.0_r)
			area_size.Y((max_lines - from_line_) * *line_height);

		return (area_size + padding_ * 2.0_r).CeilCopy(vector2::Zero);
	}
	else
		return vector2::Zero;
}

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


int Text::LineOffsetAt(int off) const noexcept
{
	if (off >= 0)
		return detail::get_line_offset(static_cast<size_t>(off), content_, formatting_);
	else
		return 0;
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
	formatted_blocks.insert(std::begin(formatted_blocks), {{}, "\n", true});

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
	formatted_blocks.push_back({{}, "\n", true});

	std::move(std::begin(formatted_blocks), std::end(formatted_blocks),
		std::inserter(formatted_blocks_, std::begin(formatted_blocks_)));
	std::move(std::begin(formatted_lines), std::end(formatted_lines),
		std::inserter(formatted_lines_, std::begin(formatted_lines_)));
}

void Text::InsertLine(int line_off, std::string_view content)
{
	if (std::empty(content_))
	{
		Content(std::string{content});
		return;
	}
	else if (line_off <= 0)
	{
		PrependLine(content);
		return;
	}

	if (auto off = detail::get_content_offset(line_off, content_, formatting_); off < std::size(content_))
		content_.insert(off, std::string{content} + "\n");
	else
	{
		AppendLine(content);
		return;
	}

	auto formatted_blocks = MakeFormattedBlocks(content);
	auto formatted_lines = MakeFormattedLines(formatted_blocks, area_size_, padding_, type_face_);
	formatted_blocks.push_back({{}, "\n", true});

	std::move(std::begin(formatted_blocks), std::end(formatted_blocks),
		std::inserter(formatted_blocks_, std::begin(formatted_blocks_) +
		detail::get_formatted_blocks_offset(line_off, formatted_blocks_)));
	std::move(std::begin(formatted_lines), std::end(formatted_lines),
		std::inserter(formatted_lines_, std::begin(formatted_lines_) +
		detail::get_formatted_lines_offset(line_off, formatted_lines_)));
}


void Text::ReplaceLine(int line_off, std::string_view content)
{
	ReplaceLines(line_off, line_off + 1, content);
}

void Text::ReplaceLines(int first_line, int last_line, std::string_view content)
{
	if (first_line >= 0 && first_line < last_line)
	{
		RemoveLines(first_line, last_line);
		InsertLine(first_line, content);
	}
}


void Text::RemoveLine(int line_off)
{
	RemoveLines(line_off, line_off + 1);
}

void Text::RemoveLines(int first_line, int last_line)
{
	if (first_line >= 0 && first_line < last_line)
	{
		content_.erase(
			std::begin(content_) + detail::get_content_offset(first_line, content_, formatting_),
			std::begin(content_) + detail::get_content_offset(last_line, content_, formatting_));
		formatted_blocks_.erase(
			std::begin(formatted_blocks_) + detail::get_formatted_blocks_offset(first_line, formatted_blocks_),
			std::begin(formatted_blocks_) + detail::get_formatted_blocks_offset(last_line, formatted_blocks_));
		formatted_lines_.erase(
			std::begin(formatted_lines_) + detail::get_formatted_lines_offset(first_line, formatted_lines_),
			std::begin(formatted_lines_) + detail::get_formatted_lines_offset(last_line, formatted_lines_));
	}
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
		auto content = utilities::detail::text_blocks_to_string(formatted_lines_.front().Blocks);

		//Rest
		for (auto iter = std::begin(formatted_lines_) + 1,
			end = std::end(formatted_lines_); iter != end; ++iter)
			content += "\n" + utilities::detail::text_blocks_to_string(iter->Blocks);

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
		auto content = utilities::detail::text_blocks_to_string(iter->Blocks);

		//Rest
		for (++iter; iter != end; ++iter)
			content += "\n" + utilities::detail::text_blocks_to_string(iter->Blocks);

		return content;
	}
	//Nothing to display
	//Could be missing/invalid type face
	else
		return "";
}


int Text::UnformattedCharacterCount() const
{
	return detail::get_character_count(formatted_blocks_);
}

int Text::UnformattedDisplayedCharacterCount() const
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
		auto count = 0;

		for (; iter != end; ++iter)
			count += detail::get_character_count(iter->Blocks);

		return count;
	}
	//Nothing to display
	//Could be missing/invalid type face
	else
		return 0;
}

} //ion::graphics::fonts