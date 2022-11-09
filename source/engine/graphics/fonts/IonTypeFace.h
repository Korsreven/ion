/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/fonts
File:	IonTypeFace.h
-------------------------------------------
*/

#ifndef ION_TYPE_FACE_H
#define ION_TYPE_FACE_H

#include "IonFont.h"
#include "managed/IonManagedObject.h"
#include "memory/IonNonOwningPtr.h"

namespace ion::graphics::fonts
{
	class TypeFaceManager; //Forward declaration

	namespace type_face::detail
	{
		inline auto is_font_attachable(Font &regular_font, Font &font) noexcept
		{
			return regular_font.Size() == font.Size();
		}
	} //type_face::detail


	///@brief A class representing a type face that can contain a regular, bold, italic and bold italic font
	class TypeFace final :
		public managed::ManagedObject<TypeFaceManager>
	{
		private:

			NonOwningPtr<Font> regular_font_;
			NonOwningPtr<Font> bold_font_;
			NonOwningPtr<Font> italic_font_;
			NonOwningPtr<Font> bold_italic_font_;

		public:

			///@brief Constructs a new type face with the given name and a regular font
			TypeFace(std::string name, NonOwningPtr<Font> regular) noexcept;

			///@brief Constructs a new type face with the given name, a regular, bold and italic font
			TypeFace(std::string name, NonOwningPtr<Font> regular, NonOwningPtr<Font> bold, NonOwningPtr<Font> italic) noexcept;

			///@brief Constructs a new type face with the given name, a regular, bold, italic and bold italic font
			TypeFace(std::string name, NonOwningPtr<Font> regular, NonOwningPtr<Font> bold, NonOwningPtr<Font> italic,
				NonOwningPtr<Font> bold_italic) noexcept;


			/**
				@name Modifiers
				@{
			*/

			///@brief Attaches the given regular font to the type face
			void RegularFont(NonOwningPtr<Font> font) noexcept;

			///@brief Attaches the given bold font to the type face
			void BoldFont(NonOwningPtr<Font> font) noexcept;

			///@brief Attaches the given italic font to the type face
			void ItalicFont(NonOwningPtr<Font> font) noexcept;

			///@brief Attaches the given bold italic font to the type face
			void BoldItalicFont(NonOwningPtr<Font> font) noexcept;

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the attached regular font
			///@details Returns nullptr if no regular font is attached
			[[nodiscard]] inline auto RegularFont() const noexcept
			{
				return regular_font_;
			}

			///@brief Returns the attached bold font
			///@details Returns nullptr if no bold font is attached
			[[nodiscard]] inline auto BoldFont() const noexcept
			{
				return bold_font_;
			}

			///@brief Returns the attached italic font
			///@details Returns nullptr if no italic font is attached
			[[nodiscard]] inline auto ItalicFont() const noexcept
			{
				return italic_font_;
			}

			///@brief Returns the attached bold italic font
			///@details Returns nullptr if no bold italic font is attached
			[[nodiscard]] inline auto BoldItalicFont() const noexcept
			{
				return bold_italic_font_;
			}


			///@brief Returns true if this type face has at least a regular font
			///@details A type face is considered invalid if regular font is missing
			[[nodiscard]] inline auto HasRegularFont() const noexcept
			{
				return !!regular_font_;
			}

			///@brief Returns true if this type face is empty (has no fonts)
			[[nodiscard]] inline auto IsEmpty() const noexcept
			{
				return !regular_font_ && !bold_font_ && !italic_font_ && !bold_italic_font_;
			}

			///@}
	};
} //ion::graphics::fonts

#endif