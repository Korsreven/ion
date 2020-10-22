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

#include "IonFontManager.h"
#include "managed/IonManagedObject.h"
#include "managed/IonObservedObject.h"

namespace ion::graphics::fonts
{
	struct TypeFaceManager; //Forward declaration

	namespace type_face::detail
	{
		inline auto is_font_attachable(Font &regular_font, Font &font) noexcept
		{
			return regular_font.Size() == font.Size();
		}
	} //type_face::detail


	class TypeFace final :
		public managed::ManagedObject<TypeFaceManager>
	{
		private:

			managed::ObservedObject<Font> regular_font_;
			managed::ObservedObject<Font> bold_font_;
			managed::ObservedObject<Font> italic_font_;
			managed::ObservedObject<Font> bold_italic_font_;

		public:

			//Constructs a new type face with the given regular font
			explicit TypeFace(Font &regular);

			//Constructs a new type face with the given regular and bold font
			TypeFace(Font &regular, Font &bold, std::nullptr_t /*italic*/);

			//Constructs a new type face with the given regular and italic font
			TypeFace(Font &regular, std::nullptr_t /*bold*/, Font &italic);

			//Constructs a new type face with the given regular, bold and italic font
			TypeFace(Font &regular, Font &bold, Font &italic);

			//Constructs a new type face with the given regular, bold, italic and bold italic font
			TypeFace(Font &regular, Font &bold, Font &italic, Font &bold_italic);


			/*
				Modifiers
			*/

			//Attach the given regular font to the type face
			void RegularFont(Font &font);

			//Detach the regular font from the type face
			void RegularFont(std::nullptr_t) noexcept;


			//Attach the given bold font to the type face
			void BoldFont(Font &font);

			//Detach the bold font from the type face
			void BoldFont(std::nullptr_t) noexcept;


			//Attach the given italic font to the type face
			void ItalicFont(Font &font);

			//Detach the italic font from the type face
			void ItalicFont(std::nullptr_t) noexcept;


			//Attach the given bold italic font to the type face
			void BoldItalicFont(Font &font);

			//Detach the bold italic font from the type face
			void BoldItalicFont(std::nullptr_t) noexcept;


			/*
				Observers
			*/

			//Returns the attached regular font
			//Returns nullptr if no regular font is attached
			[[nodiscard]] inline auto RegularFont() const noexcept
			{
				return regular_font_.Object();
			}

			//Returns the attached bold font
			//Returns nullptr if no bold font is attached
			[[nodiscard]] inline auto BoldFont() const noexcept
			{
				return bold_font_.Object();
			}

			//Returns the attached italic font
			//Returns nullptr if no italic font is attached
			[[nodiscard]] inline auto ItalicFont() const noexcept
			{
				return italic_font_.Object();
			}

			//Returns the attached bold italic font
			//Returns nullptr if no bold italic font is attached
			[[nodiscard]] inline auto BoldItalicFont() const noexcept
			{
				return bold_italic_font_.Object();
			}


			//Returns true if this type face has at least a regular font
			[[nodiscard]] inline auto HasRegularFont() const noexcept
			{
				return !!regular_font_;
			}

			//Returns true if this type face is empty (has no fonts)
			[[nodiscard]] inline auto IsEmpty() const noexcept
			{
				return !regular_font_ && !bold_font_ && !italic_font_ && !bold_italic_font_;
			}
	};
} //ion::graphics::fonts

#endif