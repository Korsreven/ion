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

#include "IonTypeFace.h"
#include "managed/IonObjectManager.h"

namespace ion::graphics::fonts
{
	namespace type_face_manager::detail
	{
	} //type_face_manager::detail


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
			return this->Objects();
		}

		//Returns an immutable range of all type faces in this manager
		//This can be used directly with a range-based for loop
		[[nodiscard]] inline const auto TypeFaces() const noexcept
		{
			return this->Objects();
		}


		/*
			Type faces
			Creating
		*/

		//Create a type face with the given regular font
		TypeFace& CreateTypeFace(Font &regular);

		//Create a type face with the given regular and bold font
		TypeFace& CreateTypeFace(Font &regular, Font &bold, std::nullptr_t /*italic*/);

		//Create a type face with the given regular and italic font
		TypeFace& CreateTypeFace(Font &regular, std::nullptr_t /*bold*/, Font &italic);

		//Create a type face with the given regular, bold and italic font
		TypeFace& CreateTypeFace(Font &regular, Font &bold, Font &italic);

		//Create a type face with the given regular, bold, italic and bold italic font
		TypeFace& CreateTypeFace(Font &regular, Font &bold, Font &italic, Font &bold_italic);


		/*
			Type faces
			Removing
		*/

		//Clear all removable type faces from this manager
		void ClearTypeFaces() noexcept;

		//Remove a removable type faces from this manager
		bool RemoveTypeFace(TypeFace &type_face) noexcept;
	};
} //ion::graphics::fonts

#endif