/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/fonts
File:	IonTextManager.cpp
-------------------------------------------
*/

#include "IonTextManager.h"
#include <type_traits>

namespace ion::graphics::fonts
{

using namespace text_manager;

namespace text_manager::detail
{

} //text_manager::detail


//Public


/*
	Texts
	Creating
*/

Text& TextManager::CreateText(std::string name, TypeFace &type_face)
{
	return Create(std::move(name), std::ref(type_face));
}


Text& TextManager::CreateText(const Text &text)
{
	return Create(text);
}

Text& TextManager::CreateText(Text &&text)
{
	return Create(std::move(text));
}


/*
	Texts
	Retrieving
*/

Text* TextManager::GetText(std::string_view name) noexcept
{
	return Get(name);
}

const Text* TextManager::GetText(std::string_view name) const noexcept
{
	return Get(name);
}


/*
	Texts
	Removing
*/

void TextManager::ClearTexts() noexcept
{
	Clear();
}

bool TextManager::RemoveText(Text &text) noexcept
{
	return Remove(text);
}

bool TextManager::RemoveText(std::string_view name) noexcept
{
	return Remove(name);
}

} //ion::graphics::fonts