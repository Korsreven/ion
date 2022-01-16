/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiTextBox.cpp
-------------------------------------------
*/

#include "IonGuiTextBox.h"

#include "graphics/render/IonViewport.h"
#include "graphics/scene/IonDrawableText.h"
#include "graphics/scene/IonModel.h"
#include "graphics/scene/IonSceneManager.h"
#include "graphics/scene/graph/IonSceneNode.h"
#include "graphics/scene/shapes/IonSprite.h"
#include "utilities/IonStringUtility.h"

namespace ion::gui::controls
{

using namespace gui_text_box;

namespace gui_text_box::detail
{

std::string trim_content(std::string content, TextBoxTextMode text_mode) noexcept
{
	using namespace utilities;
	string::RemoveNonPrintable(content);

	switch (text_mode)
	{
		case TextBoxTextMode::Alpha:
		return string::RemoveNumericCopy(std::move(content));

		case TextBoxTextMode::Numeric:
		return string::RemoveAlphaCopy(std::move(content));

		case TextBoxTextMode::AlphaNumeric:
		default:
		return content;
	}
}

std::string truncate_content(std::string content, int max_characters) noexcept
{
	if (std::ssize(content) > max_characters)
		return content.substr(0, max_characters);
	else
		return content;
}

std::string mask_content(std::string content, char mask) noexcept
{
	for (auto &c : content)
		c = mask;

	return content;
}

} //gui_text_box::detail


//Private

void GuiTextBox::DefaultSetup() noexcept
{
	CaptionLayout(gui_control::ControlCaptionLayout::OutsideTopCenter);
}


//Protected

/*
	Events
*/

void GuiTextBox::Resized(Vector2 from_size, Vector2 to_size) noexcept
{
	if (skin_)
		UpdateText();

	GuiControl::Resized(from_size, to_size); //Use base functionality
}


void GuiTextBox::Scrolled(int delta) noexcept
{
	static auto do_scroll = true;

	if (auto skin = static_cast<TextBoxSkin*>(skin_.get());
		skin && skin->Text && do_scroll)
	{
		//Text
		if (auto &text = skin->Text->Get(); text)
		{
			auto from_line = text->FromLine();

			//Scrolled up
			if (delta < 0)
			{
				text->FromLine(
					from_line > -delta ?
					from_line + delta : 0);
			}
			//Scrolled down
			else if (delta > 0)
			{
				auto line_count = text->LineCount();
				auto displayed_line_count = text->DisplayedLineCount();
				auto max_from_line = 
					line_count > displayed_line_count ?
					line_count - displayed_line_count : 0;

				text->FromLine(
					from_line + delta < max_from_line ?
					from_line + delta : max_from_line);
			}

			do_scroll = false;
			UpdateScrollBar();
			do_scroll = true;
		}
	}
}

int GuiTextBox::TotalElements() noexcept
{
	if (auto skin = static_cast<TextBoxSkin*>(skin_.get()); skin && skin->Text)
		return skin->Text->GetImmutable()->LineCount();
	else
		return 0;
}

int GuiTextBox::ElementsInView() noexcept
{
	if (auto skin = static_cast<TextBoxSkin*>(skin_.get()); skin && skin->Text)
		return skin->Text->GetImmutable()->DisplayedLineCount();
	else
		return 0;
}


int GuiTextBox::ScrollPosition() noexcept
{
	if (auto skin = static_cast<TextBoxSkin*>(skin_.get()); skin && skin->Text)
		return skin->Text->GetImmutable()->FromLine();
	else
		return 0;
}


/*
	States
*/

void GuiTextBox::SetSkinState(gui_control::ControlState state, TextBoxSkin &skin) noexcept
{
	if (skin.Text)
		SetCaptionState(state, skin.Text);
}

void GuiTextBox::SetState(gui_control::ControlState state) noexcept
{
	GuiControl::SetState(state); //Use base functionality

	if (visible_ && skin_)
		SetSkinState(state, static_cast<TextBoxSkin&>(*skin_));
}


/*
	Skins
*/

void GuiTextBox::AttachSkin()
{
	GuiControl::AttachSkin(); //Use base functionality

	if (auto skin = static_cast<TextBoxSkin*>(skin_.get()); skin && skin_node_)
	{
		if (skin->Text)
		{
			//Detach from previous parent (if any)
			if (auto node = skin->Text->ParentNode(); node)
				node->DetachObject(*skin->Text.TextObject);
		
			//Attach text
			skin_node_->AttachObject(*skin->Text.TextObject);
		}
	}
}

void GuiTextBox::DetachSkin() noexcept
{
	GuiControl::DetachSkin(); //Use base functionality
}

void GuiTextBox::RemoveSkin() noexcept
{
	if (auto skin = static_cast<TextBoxSkin*>(skin_.get()); skin)
	{
		DetachSkin();

		if (skin->Text && skin->Text->Owner())
			skin->Text->Owner()->RemoveText(*skin->Text.TextObject); //Remove text
	}

	GuiControl::RemoveSkin(); //Use base functionality
}


void GuiTextBox::UpdateText() noexcept
{
	if (auto skin = static_cast<TextBoxSkin*>(skin_.get());
		skin && skin->Text)
	{
		//Text
		if (auto &text = skin->Text->Get(); text)
		{
			if (auto size = InnerSize(); size)
			{
				auto center = CenterArea().value_or(aabb::Zero).Center();

				auto ortho_viewport_ratio =
					[&]() noexcept
					{
						//Adjust area size from ortho to viewport space
						if (auto scene_manager = skin->Text->Owner(); scene_manager)
						{
							if (auto viewport = scene_manager->ConnectedViewport(); viewport)
								return viewport->OrthoToViewportRatio();
						}

						return vector2::UnitScale;
					}();

				text->Formatting(graphics::fonts::text::TextFormatting::None);
				text->Overflow(graphics::fonts::text::TextOverflow::Truncate);
				text->AreaSize(*size * ortho_viewport_ratio);
				text->Padding(text_padding_.value_or(detail::default_text_padding_size));
				text->Alignment(detail::text_layout_to_text_alignment(text_layout_));

				if (mask_)
					text->Content(detail::mask_content(content_, *mask_));
				else
					text->Content(content_);

				skin->Text->Position(center);
			}
		}
	}

	UpdateCursor();
}

void GuiTextBox::UpdateCursor() noexcept
{
	//Todo
}


//Public

GuiTextBox::GuiTextBox(std::string name, std::optional<std::string> caption,
	OwningPtr<TextBoxSkin> skin, gui_control::BoundingBoxes hit_boxes) :
	GuiScrollable{std::move(name), std::move(caption), {}, std::move(skin), std::move(hit_boxes)}
{
	DefaultSetup();
}

GuiTextBox::GuiTextBox(std::string name, std::optional<std::string> caption,
	OwningPtr<TextBoxSkin> skin, const Vector2 &size, gui_control::BoundingBoxes hit_boxes) :
	GuiScrollable{std::move(name), std::move(caption), {}, std::move(skin), size, std::move(hit_boxes)}
{
	DefaultSetup();
}


/*
	Key events
*/

bool GuiTextBox::KeyPressed(KeyButton button) noexcept
{
	return false;
}

bool GuiTextBox::KeyReleased(KeyButton button) noexcept
{
	return GuiScrollable::KeyReleased(button); //Use base functionality
}

bool GuiTextBox::CharacterPressed(char character) noexcept
{
	return false;
}


/*
	Mouse events
*/

bool GuiTextBox::MouseReleased(MouseButton button, Vector2 position) noexcept
{
	return false;
}

} //ion::gui::controls