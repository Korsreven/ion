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

Text& TextManager::CreateText(std::string name, std::string content, text::TextAlignment alignment, TypeFace &type_face)
{
	return Create(std::move(name), std::move(content), alignment, std::ref(type_face));
}

Text& TextManager::CreateText(std::string name, std::string content, TypeFace &type_face)
{
	return Create(std::move(name), std::move(content), std::ref(type_face));
}

Text& TextManager::CreateText(std::string name, std::string content, text::TextFormatting formatting,
	text::TextAlignment alignment, text::TextVerticalAlignment vertical_alignment,
	const std::optional<Vector2> &area_size, const std::optional<Vector2> &padding,
	std::optional<int> line_spacing, TypeFace &type_face)
{
	return Create(std::move(name), std::move(content), formatting,
				  alignment, vertical_alignment,
				  area_size, padding, line_spacing, std::ref(type_face));
}

Text& TextManager::CreateText(std::string name, std::string content,
	text::TextAlignment alignment, text::TextVerticalAlignment vertical_alignment,
	const std::optional<Vector2> &area_size, const std::optional<Vector2> &padding,
	std::optional<int> line_spacing, TypeFace &type_face)
{
	return Create(std::move(name), std::move(content),
				  alignment, vertical_alignment,
				  area_size, padding, line_spacing, std::ref(type_face));
}

Text& TextManager::CreateText(std::string name, std::string content,
	const std::optional<Vector2> &area_size, const std::optional<Vector2> &padding,
	std::optional<int> line_spacing, TypeFace &type_face)
{
	return Create(std::move(name), std::move(content),
				  area_size, padding, line_spacing, std::ref(type_face));
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