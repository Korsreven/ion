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
	Texts - Creating
*/

NonOwningPtr<Text> TextManager::CreateText(std::string name, std::string content, text::TextAlignment alignment,
	NonOwningPtr<TypeFace> type_face)
{
	return Create(std::move(name), std::move(content), alignment, type_face);
}

NonOwningPtr<Text> TextManager::CreateText(std::string name, std::string content, NonOwningPtr<TypeFace> type_face)
{
	return Create(std::move(name), std::move(content), type_face);
}

NonOwningPtr<Text> TextManager::CreateText(std::string name, std::string content, text::TextFormatting formatting,
	text::TextAlignment alignment, text::TextVerticalAlignment vertical_alignment,
	const std::optional<Vector2> &area_size, const Vector2 &padding,
	std::optional<real> line_height_factor, NonOwningPtr<TypeFace> type_face)
{
	return Create(std::move(name), std::move(content), formatting,
				  alignment, vertical_alignment,
				  area_size, padding, line_height_factor, type_face);
}

NonOwningPtr<Text> TextManager::CreateText(std::string name, std::string content,
	text::TextAlignment alignment, text::TextVerticalAlignment vertical_alignment,
	const std::optional<Vector2> &area_size, const Vector2 &padding,
	std::optional<real> line_height_factor, NonOwningPtr<TypeFace> type_face)
{
	return Create(std::move(name), std::move(content),
				  alignment, vertical_alignment,
				  area_size, padding, line_height_factor, type_face);
}

NonOwningPtr<Text> TextManager::CreateText(std::string name, std::string content,
	const std::optional<Vector2> &area_size, const Vector2 &padding,
	std::optional<real> line_height_factor, NonOwningPtr<TypeFace> type_face)
{
	return Create(std::move(name), std::move(content),
				  area_size, padding, line_height_factor, type_face);
}


NonOwningPtr<Text> TextManager::CreateText(const Text &text)
{
	return Create(text);
}

NonOwningPtr<Text> TextManager::CreateText(Text &&text)
{
	return Create(std::move(text));
}


/*
	Texts - Retrieving
*/

NonOwningPtr<Text> TextManager::GetText(std::string_view name) noexcept
{
	return Get(name);
}

NonOwningPtr<const Text> TextManager::GetText(std::string_view name) const noexcept
{
	return Get(name);
}


/*
	Texts - Removing
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