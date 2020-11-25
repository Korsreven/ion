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

#include "IonTypeFaceManager.h"
#include "adaptors/ranges/IonIterable.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"
#include "managed/IonManagedObject.h"
#include "managed/IonObservedObject.h"

#undef RGB

namespace ion::graphics::fonts
{
	struct TextManager; //Forward declaration
	
	using utilities::Color;
	using utilities::Vector2;

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


		struct TextBlockStyle
		{
				std::optional<Color> ForegroundColor;
				std::optional<Color> BackgroundColor;
				std::optional<TextFontStyle> FontStyle;
				std::optional<TextDecoration> Decoration;
				std::optional<Color> DecorationColor;


				//Returns true if all styles is equal to the given text block style
				[[nodiscard]] inline auto operator==(const TextBlockStyle &rhs) const noexcept
				{
					return ForegroundColor == rhs.ForegroundColor &&
						   BackgroundColor == rhs.BackgroundColor &&
						   FontStyle == rhs.FontStyle &&
						   Decoration == rhs.Decoration &&
						   DecorationColor == rhs.DecorationColor;
				}

				//Returns true if this text block style has no styles
				[[nodiscard]] inline auto IsPlain() const noexcept
				{
					return !ForegroundColor &&
						   !BackgroundColor &&
						   !FontStyle &&
						   !Decoration &&
						   !DecorationColor;
				}
		};

		using TextBlockStyles = std::vector<TextBlockStyle>;


		struct TextBlock final : TextBlockStyle
		{
			std::string Content;
		};

		using TextBlocks = std::vector<TextBlock>;

		struct TextLine final
		{
			TextBlocks Blocks;
		};
		
		using TextLines = std::vector<TextLine>;

		using MeasuredTextLine = std::pair<TextLine, Vector2>;
		using MeasuredTextLines = std::vector<MeasuredTextLine>;


		namespace detail
		{
			inline const auto jet_black = Color::RGB(52, 52, 52);

			TextBlocks html_to_formatted_blocks(std::string_view content);
			MeasuredTextLines formatted_blocks_to_formatted_lines(TextBlocks text_blocks,
				const std::optional<Vector2> &area_size, const std::optional<Vector2> &padding, TypeFace &type_face);
		} //detail
	} //text


	//A text class that contains a typeface and some content
	class Text final : public managed::ManagedObject<TextManager>
	{
		private:

			std::string content_;
			text::TextAlignment alignment_ = text::TextAlignment::Left;
			text::TextVerticalAlignment vertical_alignment_ = text::TextVerticalAlignment::Top;
			text::TextFormatting formatting_ = text::TextFormatting::HTML;

			std::optional<Vector2> area_size_;
			std::optional<Vector2> padding_;
			std::optional<int> line_spacing_;

			int from_line_ = 0;				//Render lines in range:
			std::optional<int> max_lines_;	//[from_line, from_line + max_lines)

			Color default_foreground_color_ = text::detail::jet_black;	
			std::optional<Color> default_background_color_;	
			std::optional<text::TextFontStyle> default_font_style_;
			std::optional<text::TextDecoration> default_decoration_;
			std::optional<Color> default_decoration_color_;

			managed::ObservedObject<TypeFace> type_face_;
			text::TextBlocks formatted_blocks_;
			text::MeasuredTextLines formatted_lines_;


			text::TextBlocks MakeFormattedBlocks(std::string_view content) const;
			text::MeasuredTextLines MakeFormattedLines(text::TextBlocks text_blocks,
				const std::optional<Vector2> &area_size, const std::optional<Vector2> &padding,
				managed::ObservedObject<TypeFace> &type_face) const;

		public:

			//Construct a new text with the given name, content and a type face
			Text(std::string name, std::string content, TypeFace &type_face);
			

			/*
				Static text conversions
			*/




			/*
				Modifiers
			*/

			//Sets the (raw) content used by this text to the given content
			//Content can contain HTML tags and CSS code
			void Content(std::string content);

			//Sets the horizontal alignment of the text to the given alignment
			inline void Alignment(text::TextAlignment alignment) noexcept
			{
				alignment_ = alignment;
			}

			//Sets the vertical alignment of the text to the given alignment
			inline void VerticalAlignment(text::TextVerticalAlignment vertical_alignment) noexcept
			{
				vertical_alignment_ = vertical_alignment;
			}

			//Sets the formatting of the text to the given format
			void Formatting(text::TextFormatting formatting);


			//Sets the area size of the text to the given size
			//If nullopt is passed, no area size will be used
			void AreaSize(const std::optional<Vector2> &area_size);

			//Sets the padding size of the text area to the given padding (in pixels)
			//Padding size is the space between the area and the displayed text
			//If nullopt is passed, default padding is used (could vary based on type face)
			void Padding(const std::optional<Vector2> &padding);

			//Sets the line spacing to the given spacing (in pixels)
			//Line spacing is the space between lines in the displayed text
			//If nullopt is passed, default line spacing is used (could vary based on type face)
			inline void LineSpacing(std::optional<int> line_spacing) noexcept
			{
				line_spacing_ = line_spacing;
			}


			//Sets which line (from the displayed text) to start rendering from to the given value
			inline void FromLine(int from_line) noexcept
			{
				from_line_ = from_line;
			}

			//Sets the maximum allowed lines to display at once to the given value
			//If nullopt is passed, no maximum lines will be used
			inline void MaxLines(int max_lines) noexcept
			{
				max_lines_ = max_lines;
			}


			//Sets the default foreground color for the displayed text to the given color
			inline void DefaultForegroundColor(const Color &color) noexcept
			{
				default_foreground_color_ = color;
			}

			//Sets the default background color behind the displayed text to the given color
			inline void DefaultBackgroundColor(const Color &color) noexcept
			{
				default_background_color_ = color;
			}

			//Sets the default font style for the displayed text to the given style
			//If nullopt is passed, no default font style will be used
			inline void DefaultFontStyle(std::optional<text::TextFontStyle> font_style) noexcept
			{
				default_font_style_ = font_style;
			}

			//Sets the default decoration for the displayed text to the given decoration
			//If nullopt is passed, no default decoration will be used
			inline void DefaultDecoration(std::optional<text::TextDecoration> decoration) noexcept
			{
				default_decoration_ = decoration;
			}

			//Sets the default decoration color for the displayed text decoration to the given color
			//If nullopt is passed, no default decoration color will be used
			inline void DefaultDecoration(const std::optional<Color> &color) noexcept
			{
				default_decoration_color_ = color;
			}


			//Attach the given type face to the text (used for lettering)
			void Lettering(TypeFace &type_face);

			//Detach the type face from the text
			void Lettering(std::nullptr_t) noexcept;


			/*
				Observers
			*/

			//Returns the (raw) content used by this text
			//Content can contain HTML tags and CSS code
			[[nodiscard]] inline auto& Content() const noexcept
			{
				return content_;
			}

			//Returns the horizontal alignment of the text
			[[nodiscard]] inline auto Alignment() const noexcept
			{
				return alignment_;
			}

			//Returns the vertical alignment of the text
			[[nodiscard]] inline auto VerticalAlignment() const noexcept
			{
				return vertical_alignment_;
			}

			//Returns the formatting of the text
			[[nodiscard]] inline auto Formatting() const noexcept
			{
				return formatting_;
			}


			//Returns the area size of the text
			//Returns nullopt if no area size has been specified
			[[nodiscard]] inline auto& AreaSize() const noexcept
			{
				return area_size_;
			}

			//Returns the padding size of the text area in pixels
			//Padding size is the space between the area and the displayed text
			//Returns nullopt if default padding is used (could vary based on type face)
			[[nodiscard]] inline auto& Padding() const noexcept
			{
				return padding_;
			}

			//Returns the line spacing in pixels
			//Line spacing is the space between lines in the displayed text
			//Returns nullopt if default line spacing is used (could vary based on type face)
			[[nodiscard]] inline auto LineSpacing() const noexcept
			{
				return line_spacing_;
			}


			//Returns which line (from the displayed text) to start rendering from
			[[nodiscard]] inline auto FromLine() const noexcept
			{
				return from_line_;
			}

			//Returns the maximum allowed lines to display at once
			//Returns nullopt if no maximum lines has been specified
			[[nodiscard]] inline auto MaxLines() const noexcept
			{
				return max_lines_;
			}


			//Returns the default foreground color for the displayed text
			[[nodiscard]] inline auto& DefaultForegroundColor() const noexcept
			{
				return default_foreground_color_;
			}

			//Returns the default background color behind the displayed text
			[[nodiscard]] inline auto& DefaultBackgroundColor() const noexcept
			{
				return default_background_color_;
			}

			//Returns the default font style for the displayed text
			//Returns nullopt if no default decoration has been specified
			[[nodiscard]] inline auto DefaultFontStyle() const noexcept
			{
				return default_font_style_;
			}

			//Returns the default decoration for the displayed text
			//Returns nullopt if no default decoration has been specified
			[[nodiscard]] inline auto DefaultDecoration() const noexcept
			{
				return default_decoration_;
			}

			//Returns the default decoration color for the displayed text decoration
			//Returns nullopt if no default decoration color has been specified
			[[nodiscard]] inline auto DefaultDecorationColor() const noexcept
			{
				return default_decoration_color_;
			}


			//Returns a pointer to the mutable type face in this text (used for lettering)
			//Returns nullptr if this text does not have a type face
			[[nodiscard]] TypeFace* Lettering() noexcept;

			//Returns a pointer to the immutable type face in this text (used for lettering)
			//Returns nullptr if this text does not have a type face
			[[nodiscard]] const TypeFace* Lettering() const noexcept;


			/*
				Content
			*/

			//Insert the given content to the back of the (raw) content used by this text
			//This will only parse and format the added content (unlike Text::Content)
			void AppendContent(std::string_view content);

			//Insert the given content to the front of the (raw) content used by this text
			//This will only parse and format the added content (unlike Text::Content)
			void PrependContent(std::string_view content);


			//Insert the given content as a new line to the back of the (raw) content used by this text
			//This is an optimization of Text::AppendContent (no changes to existing lines or blocks)
			void AppendLine(std::string_view content);

			//Insert the given content as a new line to the front of the (raw) content used by this text
			//This is an optimization of Text::PrependContent (no changes to existing lines or blocks)
			void PrependLine(std::string_view content);


			//Clears all content in this text
			void Clear() noexcept;


			/*
				Formatted
			*/

			//Returns all of the formatted blocks in this text, with associated styles
			//Text styles has been parsed from HTML tags and CSS code
			[[nodiscard]] inline auto& FormattedBlocks() const noexcept
			{
				return formatted_blocks_;
			}

			//Returns an immutable range of all formatted lines in this text
			//Each line contains formatted blocks of text, with associated styles
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto FormattedLines() const noexcept
			{
				return adaptors::ranges::Iterable<const text::MeasuredTextLines&>{formatted_lines_};
			}


			/*
				Unformatted
			*/

			//Returns the (plain) unformatted content
			//All valid HTML tags are removed if TextFormatting is HTML
			[[nodiscard]] std::string UnformattedContent() const;

			//Returns the (plain) unformatted content as displayed
			//All valid HTML tags are removed if TextFormatting is HTML
			//If area size has been specified, content may be word wrapped
			[[nodiscard]] std::string UnformattedDisplayedContent() const;
	};
} //ion::graphics::fonts

#endif