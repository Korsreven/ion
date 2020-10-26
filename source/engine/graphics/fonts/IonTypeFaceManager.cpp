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
	return this->Create(std::move(name), std::ref(regular));
}

TypeFace& TypeFaceManager::CreateTypeFace(std::string name, Font &regular, Font &bold, std::nullptr_t)
{
	return this->Create(std::move(name), std::ref(regular), std::ref(bold), nullptr);
}

TypeFace& TypeFaceManager::CreateTypeFace(std::string name, Font &regular, std::nullptr_t, Font &italic)
{
	return this->Create(std::move(name), std::ref(regular), nullptr, std::ref(italic));
}

TypeFace& TypeFaceManager::CreateTypeFace(std::string name, Font &regular, Font &bold, Font &italic)
{
	return this->Create(std::move(name), std::ref(regular), std::ref(bold), std::ref(italic));
}

TypeFace& TypeFaceManager::CreateTypeFace(std::string name, Font &regular, Font &bold, Font &italic, Font &bold_italic)
{
	return this->Create(std::move(name), std::ref(regular), std::ref(bold), std::ref(italic), std::ref(bold_italic));
}


/*
	Type faces
	Removing
*/

void TypeFaceManager::ClearTypeFaces() noexcept
{
	this->Clear();
}

bool TypeFaceManager::RemoveTypeFace(TypeFace &type_face) noexcept
{
	return this->Remove(type_face);
}

} //ion::graphics::fonts