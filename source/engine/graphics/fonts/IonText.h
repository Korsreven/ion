/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/fonts
File:	IonText.h
-------------------------------------------
*/

#ifndef ION_TEXT_H
#define ION_TEXT_H

#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "IonTypeFace.h"
#include "adaptors/ranges/IonIterable.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

#undef RGB

namespace ion::graphics::fonts
{
	class TextManager; //Forward declaration
	
	using utilities::Color;
	using utilities::Vector2;

	using namespace types::type_literals;

	namespace text
	{
		enum class TextAlignment
		{
			Left,
			Center,
			Right
		};

		enum class TextVerticalAlignment
		{
			Top,
			Middle,
			Bottom
		};

		enum class TextFormatting : bool
		{
			None,
			HTML
		};

		enum class TextOverflow
		{
			Truncate,
			TruncateEllipsis,
			WordTruncate,
			WordWrap,
			Wrap
		};


		enum class TextFontStyle
		{
			Bold,
			Italic,
			BoldItalic
		};

		enum class TextDecoration
		{
			Underline,
			LineThrough,
			Overline
		};


		enum class TextBlockFontSize : bool
		{
			Smaller,
			Larger
		};

		enum class TextBlockVerticalAlign : bool
		{
			Subscript,
			Superscript
		};


		struct TextBlockStyle
		{
			std::optional<Color> ForegroundColor;
			std::optional<Color> BackgroundColor;
			std::optional<TextFontStyle> FontStyle;
			std::optional<TextDecoration> Decoration;
			std::optional<Color> DecorationColor;
			std::optional<TextBlockFontSize> FontSize;
			std::optional<TextBlockVerticalAlign> VerticalAlign;


			///@brief Returns true if all styles is equal to the given text block style
			[[nodiscard]] inline auto operator==(const TextBlockStyle &rhs) const noexcept
			{
				return ForegroundColor == rhs.ForegroundColor &&
					   BackgroundColor == rhs.BackgroundColor &&
					   FontStyle == rhs.FontStyle &&
					   Decoration == rhs.Decoration &&
					   DecorationColor == rhs.DecorationColor &&
					   FontSize == rhs.FontSize &&
					   VerticalAlign == rhs.VerticalAlign;
			}

			///@brief Returns true if this text block style has no styles
			[[nodiscard]] inline auto IsPlain() const noexcept
			{
				return !ForegroundColor &&
					   !BackgroundColor &&
					   !FontStyle &&
					   !Decoration &&
					   !DecorationColor &&
					   !FontSize &&
					   !VerticalAlign;
			}
		};

		using TextBlockStyles = std::vector<TextBlockStyle>;


		struct TextBlock final : TextBlockStyle
		{
			std::string Content;
			bool HardBreak = false;
			std::optional<Vector2> Size;	
		};

		using TextBlocks = std::vector<TextBlock>;

		struct TextLine final
		{
			TextBlocks Blocks;
			bool Tail = false;
			std::optional<Vector2> Size;
		};
		
		using TextLines = std::vector<TextLine>;


		namespace detail
		{
			constexpr auto default_line_height_factor = 2.0_r;
			inline const auto jet_black = Color::RGB(52, 52, 52);
			

			inline auto text_area_max_size(const Vector2 &area_size, const Vector2 &padding) noexcept
			{
				using namespace graphics::utilities;
				return (area_size - padding * 2.0_r).CeilCopy(vector2::Zero);
			}

			inline auto text_area_max_lines(const Vector2 &area_size, const Vector2 &padding, real line_height) noexcept
			{
				auto [width, height] = text_area_max_size(area_size, padding).XY();
				return static_cast<int>(height / line_height);
			}


			TextBlocks html_to_formatted_blocks(std::string_view content);
			TextLines formatted_blocks_to_formatted_lines(TextBlocks text_blocks, TextOverflow overflow,
				const std::optional<Vector2> &area_size, const Vector2 &padding, TypeFace &type_face);

			int get_character_count(const TextBlocks &text_blocks) noexcept;


			std::pair<size_t, size_t> find_new_line(size_t off, std::string_view content, TextFormatting formatting) noexcept;
			size_t get_content_offset(int line_off, std::string_view content, TextFormatting formatting) noexcept;
			size_t get_formatted_blocks_offset(int line_off, const TextBlocks &text_blocks) noexcept;
			size_t get_formatted_lines_offset(int line_off, const TextLines &text_lines) noexcept;
			int get_line_offset(size_t content_off, std::string_view content, TextFormatting formatting) noexcept;
		} //detail
	} //text


	///@brief A class representing a text that contains a typeface and some content that can be HTML formatted
	class Text final : public managed::ManagedObject<TextManager>
	{
		private:

			std::string content_;
			text::TextFormatting formatting_ = text::TextFormatting::HTML;
			text::TextAlignment alignment_ = text::TextAlignment::Left;
			text::TextVerticalAlignment vertical_alignment_ = text::TextVerticalAlignment::Top;
			text::TextOverflow overflow_ = text::TextOverflow::WordWrap;

			std::optional<Vector2> area_size_;
			Vector2 padding_;
			real line_height_factor_ = text::detail::default_line_height_factor;

			int from_line_ = 0;				//Render lines in range:
			std::optional<int> max_lines_;	//[from_line, from_line + max_lines)

			Color default_foreground_color_ = text::detail::jet_black;	
			std::optional<Color> default_background_color_;	
			std::optional<text::TextFontStyle> default_font_style_;
			std::optional<text::TextDecoration> default_decoration_;
			std::optional<Color> default_decoration_color_;

			NonOwningPtr<TypeFace> type_face_;
			text::TextBlocks formatted_blocks_;
			text::TextLines formatted_lines_;


			text::TextBlocks MakeFormattedBlocks(std::string_view content) const;
			text::TextLines MakeFormattedLines(text::TextBlocks text_blocks,
				const std::optional<Vector2> &area_size, const Vector2 &padding,
				NonOwningPtr<TypeFace> type_face) const;

		public:

			///@brief Constructs a new (unbounded) text with the given name, content, alignment and a type face
			Text(std::string name, std::string content, text::TextAlignment alignment,
				NonOwningPtr<TypeFace> type_face);

			///@brief Constructs a new (unbounded) text with the given name, content and a type face
			Text(std::string name, std::string content, NonOwningPtr<TypeFace> type_face);

			///@brief Constructs a new text (area) with the given name, content, formatting,
			///horizontal and vertical alignment, area size, padding, line height factor and a type face
			Text(std::string name, std::string content, text::TextFormatting formatting,
				text::TextAlignment alignment, text::TextVerticalAlignment vertical_alignment,
				const std::optional<Vector2> &area_size, const Vector2 &padding,
				std::optional<real> line_height_factor, NonOwningPtr<TypeFace> type_face);

			///@brief Constructs a new text (area) with the given name, content,
			///horizontal and vertical alignment, area size, padding, line height factor and a type face
			Text(std::string name, std::string content,
				text::TextAlignment alignment, text::TextVerticalAlignment vertical_alignment,
				const std::optional<Vector2> &area_size, const Vector2 &padding,
				std::optional<real> line_height_factor, NonOwningPtr<TypeFace> type_face);

			///@brief Constructs a new text (area) with the given name, content,
			///area size, padding, line height factor and a type face
			Text(std::string name, std::string content,
				const std::optional<Vector2> &area_size, const Vector2 &padding,
				std::optional<real> line_height_factor, NonOwningPtr<TypeFace> type_face);


			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the (raw) content to the given content
			///@details Content can contain HTML tags and CSS code
			void Content(std::string content);

			///@brief Sets the formatting of the text to the given format
			void Formatting(text::TextFormatting formatting);

			///@brief Sets the horizontal alignment of the text to the given alignment
			inline void Alignment(text::TextAlignment alignment) noexcept
			{
				alignment_ = alignment;
			}

			///@brief Sets the vertical alignment of the text to the given alignment
			inline void VerticalAlignment(text::TextVerticalAlignment vertical_alignment) noexcept
			{
				vertical_alignment_ = vertical_alignment;
			}

			///@brief Sets the overflow property of the text to the given overflow
			void Overflow(text::TextOverflow overflow) noexcept;


			///@brief Sets the area size of the text to the given size
			///@details If nullopt is passed, no area size will be used
			void AreaSize(const std::optional<Vector2> &area_size);

			///@brief Sets the padding size of the text area to the given padding (in pixels)
			///@details Padding size is the space between the area border and the displayed text
			void Padding(const Vector2 &padding);

			///@brief Sets the line height in pixels
			///@details Line height is the amount of space between lines of text
			void LineHeight(real height) noexcept;

			///@brief Sets the line height factor to the given factor (of the font size)
			///@details Line height is the amount of space between lines of text.
			///If nullopt is passed, default line height factor is used
			inline void LineHeightFactor(std::optional<real> factor) noexcept
			{
				line_height_factor_ = factor.value_or(text::detail::default_line_height_factor);
			}


			///@brief Sets which line (from the displayed text) to start rendering from to the given value
			inline void FromLine(int from_line) noexcept
			{
				from_line_ = from_line >= 0 ? from_line : 0;
			}

			///@brief Sets the maximum allowed lines to display at once to the given value
			///@details If nullopt is passed, no maximum lines will be used
			inline void MaxLines(std::optional<int> max_lines) noexcept
			{
				if (max_lines && *max_lines < 0)
					*max_lines = 0;

				max_lines_ = max_lines;
			}


			///@brief Sets the default foreground color for the displayed text to the given color
			inline void DefaultForegroundColor(const Color &color) noexcept
			{
				default_foreground_color_ = color;
			}

			///@brief Sets the default background color behind the displayed text to the given color
			///@details If nullopt is passed, no default background color will be used
			inline void DefaultBackgroundColor(const std::optional<Color> &color) noexcept
			{
				default_background_color_ = color;
			}

			///@brief Sets the default font style for the displayed text to the given style
			///@details If nullopt is passed, no default font style will be used
			inline void DefaultFontStyle(std::optional<text::TextFontStyle> font_style) noexcept
			{
				default_font_style_ = font_style;
			}

			///@brief Sets the default decoration for the displayed text to the given decoration
			///@details If nullopt is passed, no default decoration will be used
			inline void DefaultDecoration(std::optional<text::TextDecoration> decoration) noexcept
			{
				default_decoration_ = decoration;
			}

			///@brief Sets the default decoration color for the displayed text decoration to the given color
			///@details If nullopt is passed, no default decoration color will be used
			inline void DefaultDecorationColor(const std::optional<Color> &color) noexcept
			{
				default_decoration_color_ = color;
			}

			///@brief Sets the default base opacity for the displayed text to the given opacity
			inline void DefaultBaseOpacity(real opacity) noexcept
			{
				default_foreground_color_.A(opacity);

				if (default_background_color_)
					default_background_color_->A(opacity);

				if (default_decoration_color_)
					default_decoration_color_->A(opacity);
			}


			///@brief Attaches the given type face to the text (used for lettering)
			void Lettering(NonOwningPtr<TypeFace> type_face) noexcept;

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the (raw) content
			///@details Content can contain HTML tags and CSS code
			[[nodiscard]] inline auto& Content() const noexcept
			{
				return content_;
			}

			///@brief Returns the formatting of the text
			[[nodiscard]] inline auto Formatting() const noexcept
			{
				return formatting_;
			}

			///@brief Returns the horizontal alignment of the text
			[[nodiscard]] inline auto Alignment() const noexcept
			{
				return alignment_;
			}

			///@brief Returns the vertical alignment of the text
			[[nodiscard]] inline auto VerticalAlignment() const noexcept
			{
				return vertical_alignment_;
			}

			///@brief Returns the overflow property of the text
			[[nodiscard]] inline auto Overflow() const noexcept
			{
				return overflow_;
			}


			///@brief Returns the area size of the text
			///@details Returns nullopt if no area size has been specified
			[[nodiscard]] inline auto& AreaSize() const noexcept
			{
				return area_size_;
			}

			///@brief Returns the minimum area size needed to display the content in this text
			[[nodiscard]] Vector2 MinimumAreaSize() const noexcept;

			///@brief Returns the padding size of the text area in pixels
			///@details Padding size is the space between the area border and the displayed text
			[[nodiscard]] inline auto& Padding() const noexcept
			{
				return padding_;
			}

			///@brief Returns the line height in pixels
			///@details Line height is the amount of space between lines of text.
			///Returns nullopt if the text does not have a type face or regular font
			[[nodiscard]] std::optional<real> LineHeight() const noexcept;

			///@brief Returns the line height factor (of the font size)
			///@details Line height is the amount of space between lines of text
			[[nodiscard]] inline auto LineHeightFactor() const noexcept
			{
				return line_height_factor_;
			}


			///@brief Returns which line (from the displayed text) to start rendering from
			[[nodiscard]] inline auto FromLine() const noexcept
			{
				return from_line_;
			}

			///@brief Returns the maximum allowed lines to display at once
			///@details Returns nullopt if no maximum lines has been specified
			[[nodiscard]] inline auto MaxLines() const noexcept
			{
				return max_lines_;
			}


			///@brief Returns the default foreground color for the displayed text
			[[nodiscard]] inline auto& DefaultForegroundColor() const noexcept
			{
				return default_foreground_color_;
			}

			///@brief Returns the default background color behind the displayed text
			///@details Returns nullopt if no default background color has been specified
			[[nodiscard]] inline auto& DefaultBackgroundColor() const noexcept
			{
				return default_background_color_;
			}

			///@brief Returns the default font style for the displayed text
			///@details Returns nullopt if no default font style has been specified
			[[nodiscard]] inline auto DefaultFontStyle() const noexcept
			{
				return default_font_style_;
			}

			///@brief Returns the default decoration for the displayed text
			///@details Returns nullopt if no default decoration has been specified
			[[nodiscard]] inline auto DefaultDecoration() const noexcept
			{
				return default_decoration_;
			}

			///@brief Returns the default decoration color for the displayed text decoration
			///@details Returns nullopt if no default decoration color has been specified
			[[nodiscard]] inline auto DefaultDecorationColor() const noexcept
			{
				return default_decoration_color_;
			}

			///@brief Returns the default base opacity for the displayed text
			[[nodiscard]] inline auto DefaultBaseOpacity() const noexcept
			{
				return default_foreground_color_.A();
			}


			///@brief Returns a pointer to the type face in this text (used for lettering)
			///@details Returns nullptr if this text does not have a type face
			[[nodiscard]] inline auto Lettering() const noexcept
			{
				return type_face_;
			}


			///@brief Returns the number of lines in this text
			int LineCount() const noexcept;

			///@brief Returns the number of lines that are being displayed in this text
			int DisplayedLineCount() const noexcept;

			///@brief Returns the number of line that can be displayed in this text
			///@details Returns nullopt if no area size or maximum lines has been specified
			std::optional<int> DisplayedLineCapacity() const noexcept;

			///@brief Returns the line offset at the given offset
			int LineOffsetAt(int off) const noexcept;

			///@}

			/**
				@name Content
				@{
			*/

			///@brief Inserts the given content to the back of the (raw) content
			///@details This will only parse and format the added content (unlike Text::Content)
			void AppendContent(std::string_view content);

			///@brief Inserts the given content to the front of the (raw) content
			///@details This will only parse and format the added content (unlike Text::Content)
			void PrependContent(std::string_view content);


			///@brief Inserts the given content as a new line to the back of the (raw) content
			///@details This is an optimization of Text::AppendContent (no changes to existing lines or blocks)
			void AppendLine(std::string_view content);

			///@brief Inserts the given content as a new line to the front of the (raw) content
			///@details This is an optimization of Text::PrependContent (no changes to existing lines or blocks)
			void PrependLine(std::string_view content);

			///@brief Inserts the given content as a new line at the given line offset of the (raw) content
			///@details This is an optimization of Text::Content (no changes to existing lines or blocks)
			void InsertLine(int line_off, std::string_view content);


			///@brief Replaces the line at the given line offset of the (raw) content, with the given content
			///@details This is an optimization of Text::Content (no changes to other lines or blocks)
			void ReplaceLine(int line_off, std::string_view content);

			///@brief Replaces all lines in range [first, last) of the (raw) content, with the given content
			///@details This is an optimization of Text::Content (no changes to other lines or blocks)
			void ReplaceLines(int first_line, int last_line, std::string_view content);


			///@brief Removes the line at the given line offset from the (raw) content
			///@details This is an optimization of Text::Content (no changes to other lines or blocks)
			void RemoveLine(int line_off);

			///@brief Removes all lines in range [first, last) from the (raw) content
			///@details This is an optimization of Text::Content (no changes to other lines or blocks)
			void RemoveLines(int first_line, int last_line);


			///@brief Clears all content in this text
			void Clear() noexcept;

			///@}

			/**
				@name Formatted
				@{
			*/

			///@brief Returns all of the formatted blocks in this text, with associated styles
			///@details Text styles has been parsed from HTML tags and CSS code
			[[nodiscard]] inline auto& FormattedBlocks() const noexcept
			{
				return formatted_blocks_;
			}

			///@brief Returns an immutable range of all formatted lines in this text
			///@details Each line contains formatted blocks of text, with associated styles.
			///This can be used directly with a range-based for loop
			[[nodiscard]] inline auto FormattedLines() const noexcept
			{
				return adaptors::ranges::Iterable<const text::TextLines&>{formatted_lines_};
			}

			///@}

			/**
				@name Unformatted
				@{
			*/

			///@brief Returns the (plain) unformatted content
			///@details All valid HTML tags are removed if TextFormatting is HTML
			[[nodiscard]] std::string UnformattedContent() const;

			///@brief Returns the (plain) unformatted word-wrapped content
			///@details All valid HTML tags are removed if TextFormatting is HTML
			[[nodiscard]] std::string UnformattedWrappedContent() const;

			///@brief Returns the (plain) unformatted content as displayed
			///@details All valid HTML tags are removed if TextFormatting is HTML.
			///If area size has been specified, content may be word wrapped
			[[nodiscard]] std::string UnformattedDisplayedContent() const;


			///@brief Returns the number of characters in the (plain) unformatted content
			///@details All valid HTML tags are removed if TextFormatting is HTML
			[[nodiscard]] int UnformattedCharacterCount() const;

			///@brief Returns the number of characters in the (plain) unformatted content as displayed
			///@details All valid HTML tags are removed if TextFormatting is HTML
			[[nodiscard]] int UnformattedDisplayedCharacterCount() const;

			///@}
	};
} //ion::graphics::fonts

#endif