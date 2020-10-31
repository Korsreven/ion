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
#include "managed/IonManagedObject.h"
#include "managed/IonObservedObject.h"

namespace ion::graphics::fonts
{
	struct TextManager; //Forward declaration

	namespace text
	{
		namespace detail
		{
		} //detail
	} //text


	//A text class that contains a typeface and formated characters
	class Text final : public managed::ManagedObject<TextManager>
	{
		private:

			managed::ObservedObject<TypeFace> type_face_;

		public:

			//Construct a new text with the given name and a type face
			Text(std::string name, TypeFace &type_face);
			

			/*
				Static text conversions
			*/




			/*
				Modifiers
			*/




			/*
				Observers
			*/




			/*
				Type face
			*/

			//Returns a pointer to the mutable type face in this text
			//Returns nullptr if this text does not have an underlying type face
			[[nodiscard]] TypeFace* UnderlyingTypeFace() noexcept;

			//Returns a pointer to the immutable type face in this text
			//Returns nullptr if this text does not have an underlying type face
			[[nodiscard]] const TypeFace* UnderlyingTypeFace() const noexcept;
	};
} //ion::graphics::fonts

#endif