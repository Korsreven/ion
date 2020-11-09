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

		enum class TextFormatting
		{
			HTML,
			CSS,
			Both
		};

		enum class TextDecoration
		{
			Underline,
			LineThrough,
			Overline
		};

		enum class FontStyle
		{
			Bold,
			Italic,
			BoldItalic
		};


		namespace detail
		{
			inline const auto jet_black = Color::RGB(52, 52, 52);


			struct formatted_element
			{
				std::string str;
				std::optional<Color> color;
				std::optional<TextDecoration> decoration;
				std::optional<Color> decoration_color;
				std::optional<FontStyle> font_style;
			};

			using formatted_elements = std::vector<formatted_element>;

			struct formatted_line
			{
				formatted_elements elements;
				int width = 0;
			};

			using formatted_lines = std::vector<formatted_line>;
		} //detail
	} //text


	//A text class that contains a typeface and some content
	class Text final : public managed::ManagedObject<TextManager>
	{
		private:

			std::string unformatted_str_;
			text::TextAlignment alignment_ = text::TextAlignment::Left;
			text::TextVerticalAlignment vertical_alignment_ = text::TextVerticalAlignment::Top;
			std::optional<text::TextFormatting> formatting_ = text::TextFormatting::Both;

			std::optional<Vector2> area_size_;
			std::optional<int> padding_;
			std::optional<int> line_spacing_;

			int from_line_ = 0;				//Render lines in range:
			std::optional<int> max_lines_;	//[from_line, from_line + max_lines)

			Color default_color_ = text::detail::jet_black;	
			std::optional<text::TextDecoration> default_decoration_;
			std::optional<Color> default_decoration_color_;
			std::optional<text::FontStyle> default_font_style_;

			managed::ObservedObject<TypeFace> type_face_;
			text::detail::formatted_lines formatted_lines_;

		public:

			//Construct a new text with the given name, string and a type face
			Text(std::string name, std::string str, TypeFace &type_face);
			

			/*
				Static text conversions
			*/




			/*
				Modifiers
			*/

			//Sets the unformatted (raw) string used by this text to the given string
			inline void UnformattedStr(std::string str)
			{
				unformatted_str_ = std::move(str);
			}

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
			//If nullopt is passed, all formatting will be turned off
			inline void Formatting(std::optional<text::TextFormatting> formatting) noexcept
			{
				formatting_ = formatting;
			}


			//Sets the area size of the text to the given size
			//If nullopt is passed, no area size will be used
			inline void AreaSize(const std::optional<Vector2> &area_size) noexcept
			{
				area_size_ = area_size;
			}

			//Sets the padding size of the text area to the given padding (in pixels)
			//Padding size is the space between the area and the displayed text
			//If nullopt is passed, default padding is used (could vary based on type face)
			inline void Padding(std::optional<int> padding) noexcept
			{
				padding_ = padding;
			}

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


			//Sets the default color for the displayed text to the given color
			inline void DefaultColor(const Color &color) noexcept
			{
				default_color_ = color;
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

			//Sets the default font style for the displayed text to the given style
			//If nullopt is passed, no default font style will be used
			inline void DefaultFontStyle(std::optional<text::FontStyle> font_style) noexcept
			{
				default_font_style_ = font_style;
			}


			//Attach the given type face to the text (used for lettering)
			void Lettering(TypeFace &type_face);

			//Detach the type face from the text
			void Lettering(std::nullptr_t) noexcept;


			/*
				Observers
			*/

			//Returns the unformatted (raw) string used by this text
			[[nodiscard]] inline auto& UnformattedStr() const noexcept
			{
				return unformatted_str_;
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
			//Returns nullopt if all formatting is turned off
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
			[[nodiscard]] inline auto Padding() const noexcept
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


			//Returns the default color for the displayed text
			[[nodiscard]] inline auto& DefaultColor() const noexcept
			{
				return default_color_;
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

			//Returns the default font style for the displayed text
			//Returns nullopt if no default decoration has been specified
			[[nodiscard]] inline auto DefaultFontStyle() const noexcept
			{
				return default_font_style_;
			}


			//Returns a pointer to the mutable type face in this text (used for lettering)
			//Returns nullptr if this text does not have a type face
			[[nodiscard]] TypeFace* Lettering() noexcept;

			//Returns a pointer to the immutable type face in this text (used for lettering)
			//Returns nullptr if this text does not have a type face
			[[nodiscard]] const TypeFace* Lettering() const noexcept;


			/*
				Unformatted
			*/

			//Append the given string to the front of the unformatted (raw) string used by this text
			//This will only parse and format the appended string (unlike UnformattedStr)
			void AppendFront(std::string_view str);

			//Append the given string to the back of the unformatted (raw) string used by this text
			//This will only parse and format the appended string (unlike UnformattedStr)
			void AppendBack(std::string_view str);


			/*
				Formatted
			*/

			//Returns the formatted string, meaning all formatting tags removed
			//Which tags are removed and kept, is based on TextFormatting
			[[nodiscard]] std::string FormattedStr() const noexcept;

			//Returns an immutable range of all formatted lines in this text
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto FormattedLines() const noexcept
			{
				return adaptors::ranges::Iterable<const text::detail::formatted_lines&>{formatted_lines_};
			}
	};
} //ion::graphics::fonts

#endif