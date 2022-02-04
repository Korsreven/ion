/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/skins
File:	IonGuiSkin.cpp
-------------------------------------------
*/

#include "IonGuiSkin.h"

#include "IonGuiTheme.h"
#include "graphics/scene/IonDrawableText.h"
#include "graphics/scene/IonModel.h"
#include "graphics/scene/IonSceneManager.h"
#include "graphics/scene/shapes/IonSprite.h"

namespace ion::gui::skins
{

using namespace gui_skin;

namespace gui_skin
{

namespace detail
{

controls::gui_control::ControlSkin make_control_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager)
{
	return {};
}

OwningPtr<controls::gui_control::ControlSkin> make_button_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager)
{
	return nullptr;
}

OwningPtr<controls::gui_control::ControlSkin> make_check_box_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager)
{
	return nullptr;
}

OwningPtr<controls::gui_control::ControlSkin> make_group_box_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager)
{
	return nullptr;
}

OwningPtr<controls::gui_control::ControlSkin> make_label_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager)
{
	return nullptr;
}

OwningPtr<controls::gui_control::ControlSkin> make_list_box_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager)
{
	return nullptr;
}

OwningPtr<controls::gui_control::ControlSkin> make_progress_bar_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager)
{
	return nullptr;
}

OwningPtr<controls::gui_control::ControlSkin> make_radio_button_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager)
{
	return nullptr;
}

OwningPtr<controls::gui_control::ControlSkin> make_scroll_bar_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager)
{
	return nullptr;
}

OwningPtr<controls::gui_control::ControlSkin> make_slider_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager)
{
	return nullptr;
}

OwningPtr<controls::gui_control::ControlSkin> make_text_box_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager)
{
	return nullptr;
}

OwningPtr<controls::gui_control::ControlSkin> make_tooltip_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager)
{
	return nullptr;
}

} //detail


void SkinFactory::Register(std::string name, SkinMaker skin_maker)
{
	//Instance().skin_makers_[std::move(name)] = skin_maker;
}

OwningPtr<controls::gui_control::ControlSkin> SkinFactory::Make(std::string_view name,
	const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager)
{
	if (auto iter = Instance().skin_makers_.find(name); iter != std::end(Instance().skin_makers_))
		return iter->second(parts, text_parts, scene_manager);
	else
		return nullptr;
}

} //gui_skin


GuiSkin::GuiSkin(std::string name) :
	managed::ManagedObject<GuiTheme>{std::move(name)}
{
	//Empty
}

GuiSkin::GuiSkin(std::string name, const SkinParts &parts, const SkinTextPart &caption) :
	managed::ManagedObject<GuiTheme>{std::move(name)}
{
	if (parts.Center)
		parts_["center"] = parts.Center;


	if (parts.Border.Sides.Top)
		parts_["top"] = parts.Border.Sides.Top;

	if (parts.Border.Sides.Bottom)
		parts_["bottom"] = parts.Border.Sides.Bottom;

	if (parts.Border.Sides.Left)
		parts_["left"] = parts.Border.Sides.Left;

	if (parts.Border.Sides.Right)
		parts_["right"] = parts.Border.Sides.Right;


	if (parts.Border.Corners.TopLeft)
		parts_["top-left"] = parts.Border.Corners.TopLeft;

	if (parts.Border.Corners.TopRight)
		parts_["top-right"] = parts.Border.Corners.TopRight;

	if (parts.Border.Corners.BottomLeft)
		parts_["bottom-left"] = parts.Border.Corners.BottomLeft;

	if (parts.Border.Corners.BottomRight)
		parts_["bottom-right"] = parts.Border.Corners.BottomRight;


	if (caption)
		text_parts_["caption"] = caption;
}

GuiSkin::GuiSkin(std::string name, const SkinBorderParts &border_parts, const SkinTextPart &caption) :
	GuiSkin{std::move(name), SkinParts{{}, border_parts}, caption}
{
	//Empty
}

GuiSkin::GuiSkin(std::string name, const SkinSideParts &side_parts, const SkinTextPart &caption) :
	GuiSkin{std::move(name), SkinParts{{}, {side_parts, {}}}, caption}
{
	//Empty
}

GuiSkin::GuiSkin(std::string name, const SkinPart &center, const SkinTextPart &caption) :
	GuiSkin{std::move(name), SkinParts{center}, caption}
{
	//Empty
}

GuiSkin::GuiSkin(std::string name, const SkinTextPart &caption) :
	GuiSkin{std::move(name), SkinParts{}, caption}
{
	//Empty
}


/*
	Instantiating
*/

OwningPtr<controls::gui_control::ControlSkin> GuiSkin::Instantiate() const
{
	if (owner_)
	{
		if (auto scene_manager = owner_->ConnectedSceneManager(); scene_manager)
			return SkinFactory::Make(*name_, parts_, text_parts_, *scene_manager);
	}

	return nullptr;
}


/*
	Parts
	Adding
*/

void GuiSkin::AddPart(std::string name, const SkinPart &part)
{
	parts_[std::move(name)] = part;
}

void GuiSkin::AddTextPart(std::string name, const SkinTextPart &text_part)
{
	text_parts_[std::move(name)] = text_part;
}


/*
	Parts
	Retrieving
*/

gui_skin::SkinPart GuiSkin::GetPart(std::string_view name) const
{
	if (auto iter = parts_.find(name); iter != std::end(parts_))
		return iter->second;
	else
		return {};
}

gui_skin::SkinTextPart GuiSkin::GetTextPart(std::string_view name) const
{
	if (auto iter = text_parts_.find(name); iter != std::end(text_parts_))
		return iter->second;
	else
		return {};
}


/*
	Parts
	Removing
*/

void GuiSkin::ClearParts() noexcept
{
	parts_.clear();
	parts_.shrink_to_fit();
}

bool GuiSkin::RemovePart(std::string_view name) noexcept
{
	return parts_.erase(name);
}


void GuiSkin::ClearTextParts() noexcept
{
	text_parts_.clear();
	text_parts_.shrink_to_fit();
}

bool GuiSkin::RemoveTextPart(std::string_view name) noexcept
{
	return text_parts_.erase(name);
}

} //ion::gui::skins