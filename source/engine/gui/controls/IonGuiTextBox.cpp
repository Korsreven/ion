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

#include <cmath>

#include "graphics/fonts/utilities/IonFontUtility.h"
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


bool reveal_character(char c, real &width, int max_width, graphics::fonts::Font &font) noexcept
{
	if (auto c_width = graphics::fonts::utilities::MeasureCharacter(c, font).value_or(0.0_r).X();
		static_cast<int>(std::ceil(width + c_width)) > max_width) //Too wide
		return false;
	else
	{
		width += c_width;
		return true;
	}
}

std::pair<int, int> get_content_view(std::string_view content, int cursor_position, std::pair<int, int> content_view,
	std::optional<char> mask, int reveal_count, const graphics::fonts::Text &text) noexcept
{
	if (auto area_size = text.AreaSize(); area_size)
	{
		if (auto type_face = text.Lettering(); type_face)
		{
			if (auto font = graphics::fonts::utilities::detail::get_font(*type_face, text.DefaultFontStyle()); font)
			{
				auto max_width = static_cast<int>(
					graphics::fonts::text::detail::text_area_max_size(*area_size, text.Padding()).X()
				);
				auto width = 0.0_r;

				//Move left
				if (cursor_position < content_view.first)
				{
					content_view = {cursor_position, cursor_position};

					//Reveal left (by reveal count)
					for (auto off = cursor_position;
						off > 0 && cursor_position - off <= reveal_count; --off)
					{
						if (reveal_character(mask ? *mask : content[off], width, max_width, *font))
							content_view.first = off;
						else
							break;
					}

					//Reveal right
					for (auto off = cursor_position + 1; off < std::ssize(content); ++off)
					{
						if (reveal_character(mask ? *mask : content[off], width, max_width, *font))
							content_view.second = off;
						else
							break;
					}
				}
				//Move right
				else if (cursor_position > content_view.second)
				{
					content_view = {cursor_position, cursor_position};

					//Reveal right (by reveal count)
					for (auto off = cursor_position;
						off < std::ssize(content) && off - cursor_position <= reveal_count; ++off)
					{
						if (reveal_character(mask ? *mask : content[off], width, max_width, *font))
							content_view.second = off;
						else
							break;
					}

					//Reveal left
					for (auto off = cursor_position - 1; off > 0; --off)
					{
						if (reveal_character(mask ? *mask : content[off], width, max_width, *font))
							content_view.first = off;
						else
							break;
					}
				}
				//In view, refresh
				else
				{
					//Todo
				}

				return content_view;
			}
		}
	}

	return {};
}

std::string get_viewed_content(const std::string &content, std::pair<int, int> content_view, std::optional<char> mask)
{
	auto str = content.substr(content_view.first, content_view.second - content_view.first);

	if (mask)
		return mask_content(std::move(str), *mask);
	else
		return str;
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

void GuiTextBox::Focused() noexcept
{
	if (std::empty(content_) && placeholder_content_)
		UpdateText();

	GuiControl::Focused(); //Use base functionality
}

void GuiTextBox::Defocused() noexcept
{
	if (std::empty(content_) && placeholder_content_)
		UpdateText();

	GuiControl::Defocused(); //Use base functionality
}

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
	if (auto skin = static_cast<TextBoxSkin*>(skin_.get());
		skin && skin->Text && skin->Text->GetImmutable())
		return skin->Text->GetImmutable()->LineCount();
	else
		return 0;
}

int GuiTextBox::ElementsInView() noexcept
{
	if (auto skin = static_cast<TextBoxSkin*>(skin_.get());
		skin && skin->Text && skin->Text->GetImmutable())
		return skin->Text->GetImmutable()->DisplayedLineCount();
	else
		return 0;
}


int GuiTextBox::ScrollPosition() noexcept
{
	if (auto skin = static_cast<TextBoxSkin*>(skin_.get());
		skin && skin->Text && skin->Text->GetImmutable())
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
				text->Overflow(graphics::fonts::text::TextOverflow::TruncateEllipsis); //For placeholder content (if any)
				text->AreaSize(*size * ortho_viewport_ratio);
				text->Padding(text_padding_.value_or(detail::default_text_padding_size));
				text->Alignment(detail::text_layout_to_text_alignment(text_layout_));

				if (!std::empty(content_))
				{
					content_view_ =
						detail::get_content_view(content_, cursor_position_, content_view_,
							mask_, reveal_count_.value_or(detail::default_reveal_count_), *text);
					text->Content(detail::get_viewed_content(content_, content_view_, mask_));
				}
				else
				{
					content_view_ = {};

					if (!focused_ && placeholder_content_)
						text->Content(*placeholder_content_);
					else
						text->Clear();
				}

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


/*
	Content
*/

void GuiTextBox::InsertTextContent([[maybe_unused]] int off, std::string content)
{
	if (auto skin = static_cast<TextBoxSkin*>(skin_.get());
		skin && skin->Text && skin->Text->Get())
	{
		//Empty
	}
}

void GuiTextBox::ReplaceTextContent([[maybe_unused]] int first, [[maybe_unused]] int last, std::string content)
{
	if (auto skin = static_cast<TextBoxSkin*>(skin_.get());
		skin && skin->Text && skin->Text->Get())
	{
		//Text
		if (auto &text = skin->Text->Get(); text)
		{
			auto count = text->LineCount();
			auto view_count = text->DisplayedLineCount();
			auto view_capacity = text->DisplayedLineCapacity().value_or(0);

			if (count > view_count && view_count < view_capacity)
				Scrolled(view_count - view_capacity);
		}
	}
}

void GuiTextBox::RemoveTextContent([[maybe_unused]] int first, [[maybe_unused]] int last) noexcept
{
	if (auto skin = static_cast<TextBoxSkin*>(skin_.get());
		skin && skin->Text && skin->Text->Get())
	{
		//Text
		if (auto &text = skin->Text->Get(); text)
		{
			auto count = text->LineCount();
			auto view_count = text->DisplayedLineCount();
			auto view_capacity = text->DisplayedLineCapacity().value_or(0);

			if (count > view_count && view_count < view_capacity)
				Scrolled(view_count - view_capacity);
		}
	}
}

void GuiTextBox::ClearTextContent() noexcept
{
	if (auto skin = static_cast<TextBoxSkin*>(skin_.get());
		skin && skin->Text && skin->Text->Get())
	{
		//Text
		if (auto &text = skin->Text->Get(); text)
		{
			text->Clear();
			text->FromLine(0);
		}
	}
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
	Content
	Adding/inserting
*/

void GuiTextBox::AddContent(std::string content)
{
	InsertContent(std::ssize(content_), std::move(content));
}

void GuiTextBox::InsertContent(int off, std::string content)
{
	if (off >= 0)
	{
		content = gui_text_box::detail::trim_content(std::move(content), text_mode_);

		if (max_characters_)
			content = gui_text_box::detail::truncate_content(std::move(content),
				*max_characters_ - std::ssize(content_));

		off = std::clamp(off, 0, std::ssize(content));
		content_.insert(std::begin(content_) + off, std::begin(content), std::end(content));

		//Adjust cursor position
		if (cursor_position_ >= off)
			CursorPosition(cursor_position_ + std::ssize(content));

		InsertTextContent(off, std::move(content));
		UpdateText();
	}
}


/*
	Content
	Replacing
*/

void GuiTextBox::ReplaceContent(int off, std::string content)
{
	ReplaceContent(off, off + 1, std::move(content));
}

void GuiTextBox::ReplaceContent(int first, int last, std::string content)
{
	if (first >= 0 && first < last)
	{
		content = gui_text_box::detail::trim_content(std::move(content), text_mode_);

		if (max_characters_ && std::ssize(content) > last - first)
			content = gui_text_box::detail::truncate_content(std::move(content),
				*max_characters_ - (std::ssize(content_) - (last - first)));

		last = std::clamp(last, first, std::ssize(content_));
		content_.erase(std::begin(content_) + first, std::begin(content_) + last);
		content_.insert(std::begin(content_) + first, std::begin(content), std::end(content));

		//Adjust cursor position
		if (cursor_position_ >= first && cursor_position_ <= last)
			CursorPosition(first + std::ssize(content));
		else if (cursor_position_ > last)
			CursorPosition(cursor_position_ + (std::ssize(content) - (last - first)));

		ReplaceTextContent(first, last, std::move(content));
		UpdateText();
	}
}


/*
	Content
	Removing
*/

void GuiTextBox::ClearContent() noexcept
{
	content_.clear();
	content_.shrink_to_fit();

	CursorPosition(0);

	ClearTextContent();
	UpdateText();
}

void GuiTextBox::RemoveContent(int first, int last) noexcept
{
	if (first >= 0 && first < last)
	{
		last = std::clamp(last, first, std::ssize(content_));
		content_.erase(std::begin(content_) + first, std::begin(content_) + last);

		//Adjust cursor position
		if (cursor_position_ > first && cursor_position_ <= last)
			CursorPosition(first);
		else if (cursor_position_ > last)
			CursorPosition(cursor_position_ - (last - first));

		RemoveTextContent(first, last);
		UpdateText();
	}
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