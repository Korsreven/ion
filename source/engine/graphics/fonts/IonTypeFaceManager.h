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


	///@brief A class that manages and stores type faces
	class TypeFaceManager final :
		public managed::ObjectManager<TypeFace, TypeFaceManager>
	{
		public:

			///@brief Default constructor
			TypeFaceManager() = default;

			///@brief Deleted copy constructor
			TypeFaceManager(const TypeFaceManager&) = delete;

			///@brief Default move constructor
			TypeFaceManager(TypeFaceManager&&) = default;

			///@brief Destructor
			~TypeFaceManager() = default;


			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			TypeFaceManager& operator=(const TypeFaceManager&) = delete;

			///@brief Default move assignment
			TypeFaceManager& operator=(TypeFaceManager&&) = default;

			///@}

			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all type faces in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto TypeFaces() noexcept
			{
				return Objects();
			}

			///@brief Returns an immutable range of all type faces in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto TypeFaces() const noexcept
			{
				return Objects();
			}

			///@}

			/**
				@name Type faces - Creating
				@{
			*/

			///@brief Creates a type face with the given name and a regular font
			NonOwningPtr<TypeFace> CreateTypeFace(std::string name, NonOwningPtr<Font> regular);

			///@brief Creates a type face with the given name, a regular, bold and italic font
			NonOwningPtr<TypeFace> CreateTypeFace(std::string name, NonOwningPtr<Font> regular, NonOwningPtr<Font> bold, NonOwningPtr<Font> italic);

			///@brief Creates a type face with the given name, a regular, bold, italic and bold italic font
			NonOwningPtr<TypeFace> CreateTypeFace(std::string name, NonOwningPtr<Font> regular, NonOwningPtr<Font> bold, NonOwningPtr<Font> italic,
				NonOwningPtr<Font> bold_italic);


			///@brief Creates a type face as a copy of the given type face
			NonOwningPtr<TypeFace> CreateTypeFace(const TypeFace &type_face);

			///@brief Creates a type face by moving the given type face
			NonOwningPtr<TypeFace> CreateTypeFace(TypeFace &&type_face);

			///@}

			/**
				@name Type faces - Retrieving
				@{
			*/

			///@brief Gets a pointer to a mutable type face with the given name
			///@details Returns nullptr if type face could not be found
			[[nodiscard]] NonOwningPtr<TypeFace> GetTypeFace(std::string_view name) noexcept;

			///@brief Gets a pointer to an immutable type face with the given name
			///@details Returns nullptr if type face could not be found
			[[nodiscard]] NonOwningPtr<const TypeFace> GetTypeFace(std::string_view name) const noexcept;

			///@}

			/**
				@name Type faces - Removing
				@{
			*/

			///@brief Clears all removable type faces from this manager
			void ClearTypeFaces() noexcept;

			///@brief Removes a removable type face from this manager
			bool RemoveTypeFace(TypeFace &type_face) noexcept;

			///@brief Removes a removable type face with the given name from this manager
			bool RemoveTypeFace(std::string_view name) noexcept;

			///@}
	};
} //ion::graphics::fonts

#endif