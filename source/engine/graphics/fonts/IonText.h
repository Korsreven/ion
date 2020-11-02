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

#include "IonTypeFaceManager.h"
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


		namespace detail
		{
			inline const auto jet_black = Color::RGB(52, 52, 52);
		} //detail
	} //text


	//A text class that contains a typeface and some content
	class Text final : public managed::ManagedObject<TextManager>
	{
		private:

			std::string unformatted_;
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
			managed::ObservedObject<TypeFace> type_face_;

		public:

			//Construct a new text with the given name, string and a type face
			Text(std::string name, std::string str, TypeFace &type_face);
			

			/*
				Static text conversions
			*/




			/*
				Modifiers
			*/

			//
			inline void UnformattedStr(std::string str)
			{
				unformatted_ = std::move(str);
			}

			//
			inline void Alignment(text::TextAlignment alignment) noexcept
			{
				alignment_ = alignment;
			}

			//
			inline void VerticalAlignment(text::TextVerticalAlignment vertical_alignment) noexcept
			{
				vertical_alignment_ = vertical_alignment;
			}

			//
			inline void Formatting(std::optional<text::TextFormatting> formatting) noexcept
			{
				formatting_ = formatting;
			}


			//
			inline void AreaSize(const std::optional<Vector2> &area_size) noexcept
			{
				area_size_ = area_size;
			}

			//
			inline void Padding(std::optional<int> padding) noexcept
			{
				padding_ = padding;
			}

			//
			inline void LineSpacing(std::optional<int> line_spacing) noexcept
			{
				line_spacing_ = line_spacing;
			}


			//
			inline void FromLine(int from_line) noexcept
			{
				from_line_ = from_line;
			}

			//
			inline void MaxLines(int max_lines) noexcept
			{
				max_lines_ = max_lines;
			}


			//
			inline void DefaultColor(const Color &color) noexcept
			{
				default_color_ = color;
			}

			//
			inline void DefaultDecoration(std::optional<text::TextDecoration> decoration) noexcept
			{
				default_decoration_ = decoration;
			}

			//Attach the given type face to the text (used for lettering)
			void Lettering(TypeFace &type_face);

			//Detach the type face from the text
			void Lettering(std::nullptr_t) noexcept;


			/*
				Observers
			*/

			//
			[[nodiscard]] inline auto& UnformattedStr() const noexcept
			{
				return unformatted_;
			}

			//
			[[nodiscard]] std::string FormattedStr() const noexcept;

			//
			[[nodiscard]] inline auto Alignment() const noexcept
			{
				return alignment_;
			}

			//
			[[nodiscard]] inline auto VerticalAlignment() const noexcept
			{
				return vertical_alignment_;
			}

			//
			[[nodiscard]] inline auto Formatting() const noexcept
			{
				return formatting_;
			}


			//
			[[nodiscard]] inline auto& AreaSize() const noexcept
			{
				return area_size_;
			}

			//
			[[nodiscard]] inline auto Padding() const noexcept
			{
				return padding_;
			}

			//
			[[nodiscard]] inline auto LineSpacing() const noexcept
			{
				return line_spacing_;
			}


			//
			[[nodiscard]] inline auto FromLine() const noexcept
			{
				return from_line_;
			}

			//
			[[nodiscard]] inline auto MaxLines() const noexcept
			{
				return max_lines_;
			}


			//
			[[nodiscard]] inline auto& DefaultColor() const noexcept
			{
				return default_color_;
			}

			//
			[[nodiscard]] inline auto DefaultDecoration() const noexcept
			{
				return default_decoration_;
			}

			//Returns a pointer to the mutable type face in this text (used for lettering)
			//Returns nullptr if this text does not have a type face
			[[nodiscard]] TypeFace* Lettering() noexcept;

			//Returns a pointer to the immutable type face in this text (used for lettering)
			//Returns nullptr if this text does not have a type face
			[[nodiscard]] const TypeFace* Lettering() const noexcept;
	};
} //ion::graphics::fonts

#endif