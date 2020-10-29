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
	Type faces
	Creating
*/

TypeFace& TypeFaceManager::CreateTypeFace(std::string name, Font &regular)
{
	return Create(std::move(name), std::ref(regular));
}

TypeFace& TypeFaceManager::CreateTypeFace(std::string name, Font &regular, Font &bold, std::nullptr_t)
{
	return Create(std::move(name), std::ref(regular), std::ref(bold), nullptr);
}

TypeFace& TypeFaceManager::CreateTypeFace(std::string name, Font &regular, std::nullptr_t, Font &italic)
{
	return Create(std::move(name), std::ref(regular), nullptr, std::ref(italic));
}

TypeFace& TypeFaceManager::CreateTypeFace(std::string name, Font &regular, Font &bold, Font &italic)
{
	return Create(std::move(name), std::ref(regular), std::ref(bold), std::ref(italic));
}

TypeFace& TypeFaceManager::CreateTypeFace(std::string name, Font &regular, Font &bold, Font &italic, Font &bold_italic)
{
	return Create(std::move(name), std::ref(regular), std::ref(bold), std::ref(italic), std::ref(bold_italic));
}


TypeFace& TypeFaceManager::CreateTypeFace(const TypeFace &type_face)
{
	return Create(type_face);
}

TypeFace& TypeFaceManager::CreateTypeFace(TypeFace &&type_face)
{
	return Create(std::move(type_face));
}


/*
	Type faces
	Retrieving
*/

TypeFace* TypeFaceManager::GetTypeFace(std::string_view name) noexcept
{
	return Get(name);
}

const TypeFace* TypeFaceManager::GetTypeFace(std::string_view name) const noexcept
{
	return Get(name);
}


/*
	Type faces
	Removing
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