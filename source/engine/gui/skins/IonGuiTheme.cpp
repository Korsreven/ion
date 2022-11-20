/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/skins
File:	IonGuiTheme.cpp
-------------------------------------------
*/

#include "IonGuiTheme.h"

namespace ion::gui::skins
{

using namespace gui_theme;

namespace gui_theme::detail
{

} //gui_theme::detail


GuiTheme::GuiTheme(std::string name, NonOwningPtr<graphics::scene::SceneManager> scene_manager) noexcept :

	ManagedObject<GuiController>{std::move(name)},
	scene_manager_{scene_manager}
{
	//Empty
}


/*
	Skins - Creating
*/

NonOwningPtr<GuiSkin> GuiTheme::CreateSkin(const GuiSkin &skin)
{
	return Create(skin);
}

NonOwningPtr<GuiSkin> GuiTheme::CreateSkin(GuiSkin &&skin)
{
	return Create(std::move(skin));
}


/*
	Skins - Retrieving
*/

NonOwningPtr<GuiSkin> GuiTheme::GetSkin(std::string_view name) noexcept
{
	return Get(name);
}

NonOwningPtr<const GuiSkin> GuiTheme::GetSkin(std::string_view name) const noexcept
{
	return Get(name);
}


/*
	Skins - Removing
*/

void GuiTheme::ClearSkins() noexcept
{
	Clear();
}

bool GuiTheme::RemoveSkin(GuiSkin &skin) noexcept
{
	return Remove(skin);
}

bool GuiTheme::RemoveSkin(std::string_view name) noexcept
{
	return Remove(name);
}

} //ion::gui::skins