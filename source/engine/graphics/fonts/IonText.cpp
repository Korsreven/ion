/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/fonts
File:	IonText.cpp
-------------------------------------------
*/

#include "IonText.h"
#include "utilities/IonFontUtility.h"

namespace ion::graphics::fonts
{

using namespace text;

Text::Text(std::string name, std::string content, TypeFace &type_face) :

	managed::ManagedObject<TextManager>{std::move(name)},

	content_{std::move(content)},
	type_face_{type_face}
{
	//Empty
}


/*
	Modifiers
*/

void Text::Lettering(TypeFace &type_face)
{
	type_face_.Observe(type_face);
}

void Text::Lettering(std::nullptr_t) noexcept
{
	type_face_.Release();
}


/*
	Observers
*/

TypeFace* Text::Lettering() noexcept
{
	return type_face_.Object();
}

const TypeFace* Text::Lettering() const noexcept
{
	return type_face_.Object();
}


/*
	Content
*/

void Text::AppendFront(std::string_view content)
{
	content_.insert(0, content);
}

void Text::AppendBack(std::string_view content)
{
	content_ += content;
}


/*
	Unformatted
*/

std::string Text::UnformattedContent() const noexcept
{
	std::string content;

	//First
	if (!std::empty(formatted_lines_))
	{
		content += utilities::detail::text_blocks_to_string(formatted_lines_.front().Blocks);

		//Rest
		for (auto iter = std::begin(formatted_lines_) + 1,
			end = std::end(formatted_lines_); iter != end; ++iter)
			content += "\n" + utilities::detail::text_blocks_to_string(iter->Blocks);
	}

	return content;
}

} //ion::graphics::fonts