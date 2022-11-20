/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/fonts
File:	IonTypeFaceManager.cpp
-------------------------------------------
*/

#include "IonTypeFaceManager.h"
#include <type_traits>

namespace ion::graphics::fonts
{

using namespace type_face_manager;

namespace type_face_manager::detail
{

} //type_face_manager::detail


//Public


/*
	Type faces - Creating
*/

NonOwningPtr<TypeFace> TypeFaceManager::CreateTypeFace(std::string name, NonOwningPtr<Font> regular)
{
	return Create(std::move(name), regular);
}

NonOwningPtr<TypeFace> TypeFaceManager::CreateTypeFace(std::string name, NonOwningPtr<Font> regular, NonOwningPtr<Font> bold, NonOwningPtr<Font> italic)
{
	return Create(std::move(name), regular, bold, italic);
}

NonOwningPtr<TypeFace> TypeFaceManager::CreateTypeFace(std::string name, NonOwningPtr<Font> regular, NonOwningPtr<Font> bold, NonOwningPtr<Font> italic,
	NonOwningPtr<Font> bold_italic)
{
	return Create(std::move(name), regular, bold, italic, bold_italic);
}


NonOwningPtr<TypeFace> TypeFaceManager::CreateTypeFace(const TypeFace &type_face)
{
	return Create(type_face);
}

NonOwningPtr<TypeFace> TypeFaceManager::CreateTypeFace(TypeFace &&type_face)
{
	return Create(std::move(type_face));
}


/*
	Type faces - Retrieving
*/

NonOwningPtr<TypeFace> TypeFaceManager::GetTypeFace(std::string_view name) noexcept
{
	return Get(name);
}

NonOwningPtr<const TypeFace> TypeFaceManager::GetTypeFace(std::string_view name) const noexcept
{
	return Get(name);
}


/*
	Type faces - Removing
*/

void TypeFaceManager::ClearTypeFaces() noexcept
{
	Clear();
}

bool TypeFaceManager::RemoveTypeFace(TypeFace &type_face) noexcept
{
	return Remove(type_face);
}

bool TypeFaceManager::RemoveTypeFace(std::string_view name) noexcept
{
	return Remove(name);
}

} //ion::graphics::fonts