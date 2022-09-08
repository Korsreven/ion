/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/fonts
File:	IonTypeFaceManager.h
-------------------------------------------
*/

#ifndef ION_TYPE_FACE_MANAGER_H
#define ION_TYPE_FACE_MANAGER_H

#include <string>
#include <string_view>

#include "IonTypeFace.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"

namespace ion::graphics::fonts
{
	namespace type_face_manager::detail
	{
	} //type_face_manager::detail


	//A class that manages and stores type faces
	struct TypeFaceManager final :
		managed::ObjectManager<TypeFace, TypeFaceManager>
	{
		//Default constructor
		TypeFaceManager() = default;

		//Deleted copy constructor
		TypeFaceManager(const TypeFaceManager&) = delete;

		//Default move constructor
		TypeFaceManager(TypeFaceManager&&) = default;

		//Destructor
		~TypeFaceManager() = default;


		/*
			Operators
		*/

		//Deleted copy assignment
		TypeFaceManager& operator=(const TypeFaceManager&) = delete;

		//Move assignment
		TypeFaceManager& operator=(TypeFaceManager&&) = default;


		/*
			Ranges
		*/

		//Returns a mutable range of all type faces in this manager
		//This can be used directly with a range-based for loop
		[[nodiscard]] inline auto TypeFaces() noexcept
		{
			return Objects();
		}

		//Returns an immutable range of all type faces in this manager
		//This can be used directly with a range-based for loop
		[[nodiscard]] inline auto TypeFaces() const noexcept
		{
			return Objects();
		}


		/*
			Type faces
			Creating
		*/

		//Create a type face with the given name and a regular font
		NonOwningPtr<TypeFace> CreateTypeFace(std::string name, NonOwningPtr<Font> regular);

		//Create a type face with the given name, a regular, bold and italic font
		NonOwningPtr<TypeFace> CreateTypeFace(std::string name, NonOwningPtr<Font> regular, NonOwningPtr<Font> bold, NonOwningPtr<Font> italic);

		//Create a type face with the given name, a regular, bold, italic and bold italic font
		NonOwningPtr<TypeFace> CreateTypeFace(std::string name, NonOwningPtr<Font> regular, NonOwningPtr<Font> bold, NonOwningPtr<Font> italic,
			NonOwningPtr<Font> bold_italic);


		//Create a type face as a copy of the given type face
		NonOwningPtr<TypeFace> CreateTypeFace(const TypeFace &type_face);

		//Create a type face by moving the given type face
		NonOwningPtr<TypeFace> CreateTypeFace(TypeFace &&type_face);


		/*
			Type faces
			Retrieving
		*/

		//Gets a pointer to a mutable type face with the given name
		//Returns nullptr if type face could not be found
		[[nodiscard]] NonOwningPtr<TypeFace> GetTypeFace(std::string_view name) noexcept;

		//Gets a pointer to an immutable type face with the given name
		//Returns nullptr if type face could not be found
		[[nodiscard]] NonOwningPtr<const TypeFace> GetTypeFace(std::string_view name) const noexcept;


		/*
			Type faces
			Removing
		*/

		//Clear all removable type faces from this manager
		void ClearTypeFaces() noexcept;

		//Remove a removable type face from this manager
		bool RemoveTypeFace(TypeFace &type_face) noexcept;

		//Remove a removable type face with the given name from this manager
		bool RemoveTypeFace(std::string_view name) noexcept;
	};
} //ion::graphics::fonts

#endif