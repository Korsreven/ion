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
#include "graphics/render/IonPass.h"
#include "graphics/scene/IonDrawableText.h"
#include "graphics/scene/IonModel.h"
#include "graphics/scene/IonSceneManager.h"
#include "graphics/scene/shapes/IonSprite.h"
#include "graphics/utilities/IonVector2.h"
#include "gui/controls/IonGuiButton.h"
#include "gui/controls/IonGuiCheckBox.h"
#include "gui/controls/IonGuiGroupBox.h"
#include "gui/controls/IonGuiLabel.h"
#include "gui/controls/IonGuiListBox.h"
#include "gui/controls/IonGuiProgressBar.h"
#include "gui/controls/IonGuiRadioButton.h"
#include "gui/controls/IonGuiScrollBar.h"
#include "gui/controls/IonGuiSlider.h"
#include "gui/controls/IonGuiTextBox.h"
#include "gui/controls/IonGuiTooltip.h"
#include "types/IonTypes.h"

namespace ion::gui::skins
{

using namespace gui_skin;
using namespace graphics::utilities;
using namespace types::type_literals;

namespace gui_skin::detail
{

controls::gui_control::ControlSkin make_control_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager)
{
	controls::gui_control::ControlSkin control_skin;

	if (!std::empty(parts))
	{
		auto iter = parts.find("center");
		auto center_part = iter != std::end(parts) ? &iter->second : nullptr;

		//Sides
		iter = parts.find("top");
		auto top_part = iter != std::end(parts) ? &iter->second : nullptr;

		iter = parts.find("bottom");
		auto bottom_part = iter != std::end(parts) ? &iter->second : nullptr;

		iter = parts.find("left");
		auto left_part = iter != std::end(parts) ? &iter->second : nullptr;

		iter = parts.find("right");
		auto right_part = iter != std::end(parts) ? &iter->second : nullptr;

		//Corners
		iter = parts.find("top-left");
		auto top_left_part = iter != std::end(parts) ? &iter->second : nullptr;

		iter = parts.find("top-right");
		auto top_right_part = iter != std::end(parts) ? &iter->second : nullptr;

		iter = parts.find("bottom-left");
		auto bottom_left_part = iter != std::end(parts) ? &iter->second : nullptr;

		iter = parts.find("bottom-right");
		auto bottom_right_part = iter != std::end(parts) ? &iter->second : nullptr;


		//Minimum required parts
		if (center_part ||
			(top_part && bottom_part) || (left_part && right_part) ||
			(top_left_part && bottom_right_part) || (bottom_left_part && top_right_part))
		{
			auto model = scene_manager.CreateModel();
			model->AddPass(graphics::render::Pass{});
			control_skin.Parts.ModelObject = model;

			//Center part
			if (center_part && *center_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, center_part->Enabled});
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);

				control_skin.Parts.Center.SpriteObject = sprite;
				control_skin.Parts.Center.EnabledMaterial = center_part->Enabled;
				control_skin.Parts.Center.DisabledMaterial = center_part->Disabled;
				control_skin.Parts.Center.FocusedMaterial = center_part->Focused;
				control_skin.Parts.Center.PressedMaterial = center_part->Pressed;
				control_skin.Parts.Center.HoveredMaterial = center_part->Hovered;
			}

			//Sides
			//Top part
			if (top_part && *top_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, top_part->Enabled});
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);

				control_skin.Parts.Top.SpriteObject = sprite;
				control_skin.Parts.Top.EnabledMaterial = top_part->Enabled;
				control_skin.Parts.Top.DisabledMaterial = top_part->Disabled;
				control_skin.Parts.Top.FocusedMaterial = top_part->Focused;
				control_skin.Parts.Top.PressedMaterial = top_part->Pressed;
				control_skin.Parts.Top.HoveredMaterial = top_part->Hovered;
			}

			//Bottom part
			if (bottom_part && *bottom_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, bottom_part->Enabled});
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);

				control_skin.Parts.Bottom.SpriteObject = sprite;
				control_skin.Parts.Bottom.EnabledMaterial = bottom_part->Enabled;
				control_skin.Parts.Bottom.DisabledMaterial = bottom_part->Disabled;
				control_skin.Parts.Bottom.FocusedMaterial = bottom_part->Focused;
				control_skin.Parts.Bottom.PressedMaterial = bottom_part->Pressed;
				control_skin.Parts.Bottom.HoveredMaterial = bottom_part->Hovered;
			}

			//Left part
			if (left_part && *left_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, left_part->Enabled});
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);

				control_skin.Parts.Left.SpriteObject = sprite;
				control_skin.Parts.Left.EnabledMaterial = left_part->Enabled;
				control_skin.Parts.Left.DisabledMaterial = left_part->Disabled;
				control_skin.Parts.Left.FocusedMaterial = left_part->Focused;
				control_skin.Parts.Left.PressedMaterial = left_part->Pressed;
				control_skin.Parts.Left.HoveredMaterial = left_part->Hovered;
			}

			//Right part
			if (right_part && *right_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, right_part->Enabled});
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);

				control_skin.Parts.Right.SpriteObject = sprite;
				control_skin.Parts.Right.EnabledMaterial = right_part->Enabled;
				control_skin.Parts.Right.DisabledMaterial = right_part->Disabled;
				control_skin.Parts.Right.FocusedMaterial = right_part->Focused;
				control_skin.Parts.Right.PressedMaterial = right_part->Pressed;
				control_skin.Parts.Right.HoveredMaterial = right_part->Hovered;
			}

			//Corners
			//Top-left part
			if (top_left_part && *top_left_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, top_left_part->Enabled});
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);

				control_skin.Parts.TopLeft.SpriteObject = sprite;
				control_skin.Parts.TopLeft.EnabledMaterial = top_left_part->Enabled;
				control_skin.Parts.TopLeft.DisabledMaterial = top_left_part->Disabled;
				control_skin.Parts.TopLeft.FocusedMaterial = top_left_part->Focused;
				control_skin.Parts.TopLeft.PressedMaterial = top_left_part->Pressed;
				control_skin.Parts.TopLeft.HoveredMaterial = top_left_part->Hovered;
			}

			//Top-right part
			if (top_right_part && *top_right_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, top_right_part->Enabled});
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);

				control_skin.Parts.TopRight.SpriteObject = sprite;
				control_skin.Parts.TopRight.EnabledMaterial = top_right_part->Enabled;
				control_skin.Parts.TopRight.DisabledMaterial = top_right_part->Disabled;
				control_skin.Parts.TopRight.FocusedMaterial = top_right_part->Focused;
				control_skin.Parts.TopRight.PressedMaterial = top_right_part->Pressed;
				control_skin.Parts.TopRight.HoveredMaterial = top_right_part->Hovered;
			}

			//Bottom-left part
			if (bottom_left_part && *bottom_left_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, bottom_left_part->Enabled});
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);

				control_skin.Parts.BottomLeft.SpriteObject = sprite;
				control_skin.Parts.BottomLeft.EnabledMaterial = bottom_left_part->Enabled;
				control_skin.Parts.BottomLeft.DisabledMaterial = bottom_left_part->Disabled;
				control_skin.Parts.BottomLeft.FocusedMaterial = bottom_left_part->Focused;
				control_skin.Parts.BottomLeft.PressedMaterial = bottom_left_part->Pressed;
				control_skin.Parts.BottomLeft.HoveredMaterial = bottom_left_part->Hovered;
			}

			//Bottom-right part
			if (bottom_right_part && *bottom_right_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, bottom_right_part->Enabled});
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);

				control_skin.Parts.BottomRight.SpriteObject = sprite;
				control_skin.Parts.BottomRight.EnabledMaterial = bottom_right_part->Enabled;
				control_skin.Parts.BottomRight.DisabledMaterial = bottom_right_part->Disabled;
				control_skin.Parts.BottomRight.FocusedMaterial = bottom_right_part->Focused;
				control_skin.Parts.BottomRight.PressedMaterial = bottom_right_part->Pressed;
				control_skin.Parts.BottomRight.HoveredMaterial = bottom_right_part->Hovered;
			}


			//Sides
			//Top part (position)
			if (control_skin.Parts.Top)
			{
				if (control_skin.Parts.Center)
					control_skin.Parts.Top->Position(
						{0.0_r, control_skin.Parts.Center->Size().Y() * 0.5_r + control_skin.Parts.Top->Size().Y() * 0.5_r, 0.0_r});
			}

			//Bottom part (position)
			if (control_skin.Parts.Bottom)
			{
				control_skin.Parts.Bottom->Position({});
			}

			//Left part (position)
			if (control_skin.Parts.Left)
			{
				control_skin.Parts.Left->Position({});
			}

			//Right part (position)
			if (control_skin.Parts.Right)
			{
				control_skin.Parts.Right->Position({});
			}

			//Corners
			//Top-left part (position)
			if (control_skin.Parts.TopLeft)
			{
				control_skin.Parts.TopLeft->Position({});
			}

			//Top-right part (position)
			if (control_skin.Parts.TopRight)
			{
				control_skin.Parts.TopRight->Position({});
			}

			//Bottom-left part (position)
			if (control_skin.Parts.BottomLeft)
			{
				control_skin.Parts.BottomLeft->Position({});
			}

			//Bottom-right part (position)
			if (control_skin.Parts.BottomRight)
			{
				control_skin.Parts.BottomRight->Position({});
			}
		}
	}

	if (!std::empty(text_parts))
	{
		auto iter = text_parts.find("caption");
		auto caption_part = iter != std::end(text_parts) ? &iter->second : nullptr;

		//Caption part
		if (caption_part && *caption_part)
		{
			auto text = scene_manager.CreateText(caption_part->Template);
			text->AddPass(graphics::render::Pass{});

			control_skin.Caption.TextObject = text;
			control_skin.Caption.EnabledStyle = caption_part->Enabled;
			control_skin.Caption.DisabledStyle = caption_part->Disabled;
			control_skin.Caption.FocusedStyle = caption_part->Focused;
			control_skin.Caption.PressedStyle = caption_part->Pressed;
			control_skin.Caption.HoveredStyle = caption_part->Hovered;
		}
	}

	return control_skin;
}


OwningPtr<controls::gui_control::ControlSkin> make_button_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager)
{
	auto button_skin = make_owning<controls::gui_button::ButtonSkin>();
	static_cast<controls::gui_control::ControlSkin&>(*button_skin) =
		make_control_skin(parts, text_parts, scene_manager);
	return button_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_check_box_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager)
{
	auto check_box_skin = make_owning<controls::gui_check_box::CheckBoxSkin>();
	static_cast<controls::gui_control::ControlSkin&>(*check_box_skin) =
		make_control_skin(parts, text_parts, scene_manager);

	//Todo

	return check_box_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_group_box_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager)
{
	auto group_box_skin = make_owning<controls::gui_group_box::GroupBoxSkin>();
	static_cast<controls::gui_control::ControlSkin&>(*group_box_skin) =
		make_control_skin(parts, text_parts, scene_manager);
	return group_box_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_label_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager)
{
	auto label_skin = make_owning<controls::gui_label::LabelSkin>();
	static_cast<controls::gui_control::ControlSkin&>(*label_skin) =
		make_control_skin(parts, text_parts, scene_manager);
	return label_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_list_box_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager)
{
	auto list_box_skin = make_owning<controls::gui_list_box::ListBoxSkin>();
	static_cast<controls::gui_control::ControlSkin&>(*list_box_skin) =
		make_control_skin(parts, text_parts, scene_manager);

	//Todo

	return list_box_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_progress_bar_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager)
{
	auto progress_bar_skin = make_owning<controls::gui_progress_bar::ProgressBarSkin>();
	static_cast<controls::gui_control::ControlSkin&>(*progress_bar_skin) =
		make_control_skin(parts, text_parts, scene_manager);

	//Todo

	return progress_bar_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_radio_button_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager)
{
	auto radio_button_skin = make_owning<controls::gui_radio_button::RadioButtonSkin>();
	static_cast<controls::gui_control::ControlSkin&>(*radio_button_skin) =
		make_control_skin(parts, text_parts, scene_manager);

	//Todo

	return radio_button_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_scroll_bar_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager)
{
	auto scroll_bar_skin = make_owning<controls::gui_scroll_bar::ScrollBarSkin>();
	static_cast<controls::gui_control::ControlSkin&>(*scroll_bar_skin) =
		make_control_skin(parts, text_parts, scene_manager);

	//Todo

	return scroll_bar_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_slider_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager)
{
	auto slider_skin = make_owning<controls::gui_slider::SliderSkin>();
	static_cast<controls::gui_control::ControlSkin&>(*slider_skin) =
		make_control_skin(parts, text_parts, scene_manager);

	//Todo

	return slider_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_text_box_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager)
{
	auto text_box_skin = make_owning<controls::gui_text_box::TextBoxSkin>();
	static_cast<controls::gui_control::ControlSkin&>(*text_box_skin) =
		make_control_skin(parts, text_parts, scene_manager);

	//Todo

	return text_box_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_tooltip_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager)
{
	auto tooltip_skin = make_owning<controls::gui_tooltip::TooltipSkin>();
	static_cast<controls::gui_control::ControlSkin&>(*tooltip_skin) =
		make_control_skin(parts, text_parts, scene_manager);
	return tooltip_skin;
}

} //gui_skin::detail

//Private

adaptors::FlatMap<std::type_index, std::string> GuiSkin::registered_skins_{};
adaptors::FlatMap<std::string, gui_skin::SkinBuilder> GuiSkin::registered_skin_builders_{};

void GuiSkin::RegisterDefaultSkins()
{
	RegisterSkin<controls::GuiButton>("GuiButton", detail::make_button_skin);
	RegisterSkin<controls::GuiCheckBox>("GuiCheckBox", detail::make_check_box_skin);
	RegisterSkin<controls::GuiGroupBox>("GuiGroupBox", detail::make_group_box_skin);
	RegisterSkin<controls::GuiLabel>("GuiLabel", detail::make_label_skin);
	RegisterSkin<controls::GuiListBox>("GuiListBox", detail::make_list_box_skin);
	RegisterSkin<controls::GuiProgressBar>("GuiProgressBar", detail::make_progress_bar_skin);
	RegisterSkin<controls::GuiRadioButton>("GuiRadioButton", detail::make_radio_button_skin);
	RegisterSkin<controls::GuiScrollBar>("GuiScrollBar", detail::make_scroll_bar_skin);
	RegisterSkin<controls::GuiSlider>("GuiSlider", detail::make_slider_skin);
	RegisterSkin<controls::GuiTextBox>("GuiTextBox", detail::make_text_box_skin);
	RegisterSkin<controls::GuiTooltip>("GuiTooltip", detail::make_tooltip_skin);
}


//Public

GuiSkin::GuiSkin(std::string name) :
	managed::ManagedObject<GuiTheme>{std::move(name)}
{
	RegisterDefaultSkins();
}

GuiSkin::GuiSkin(std::string name, const SkinParts &parts, const SkinTextPart &caption) :
	managed::ManagedObject<GuiTheme>{std::move(name)}
{
	RegisterDefaultSkins();

	if (parts.Center)
		parts_["center"] = parts.Center;

	//Sides
	if (parts.Border.Sides.Top)
		parts_["top"] = parts.Border.Sides.Top;

	if (parts.Border.Sides.Bottom)
		parts_["bottom"] = parts.Border.Sides.Bottom;

	if (parts.Border.Sides.Left)
		parts_["left"] = parts.Border.Sides.Left;

	if (parts.Border.Sides.Right)
		parts_["right"] = parts.Border.Sides.Right;

	//Corners
	if (parts.Border.Corners.TopLeft)
		parts_["top-left"] = parts.Border.Corners.TopLeft;

	if (parts.Border.Corners.TopRight)
		parts_["top-right"] = parts.Border.Corners.TopRight;

	if (parts.Border.Corners.BottomLeft)
		parts_["bottom-left"] = parts.Border.Corners.BottomLeft;

	if (parts.Border.Corners.BottomRight)
		parts_["bottom-right"] = parts.Border.Corners.BottomRight;

	//Caption
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
		{
			if (auto iter = registered_skin_builders_.find(*name_);
				iter != std::end(registered_skin_builders_))
				return iter->second(parts_, text_parts_, *scene_manager);
		}
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