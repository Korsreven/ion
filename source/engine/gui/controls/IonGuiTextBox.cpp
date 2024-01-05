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

#include "IonEngine.h"
#include "graphics/fonts/utilities/IonFontUtility.h"
#include "graphics/scene/IonDrawableText.h"
#include "graphics/scene/IonModel.h"
#include "graphics/scene/IonSceneManager.h"
#include "graphics/scene/graph/IonSceneNode.h"
#include "graphics/scene/shapes/IonSprite.h"
#include "utilities/IonMath.h"
#include "utilities/IonStringUtility.h"
#include "system/IonSystemUtility.h"

namespace ion::gui::controls
{

using namespace gui_text_box;

namespace gui_text_box::detail
{

Vector2 cursor_offset(real width, real line_width, real line_padding, real cursor_distance, TextBoxTextLayout text_layout) noexcept
{
	switch (text_layout)
	{
		case TextBoxTextLayout::Left:
		return {-width * 0.5_r + line_padding + cursor_distance, 0.0_r};

		case TextBoxTextLayout::Right:
		return {width * 0.5_r - line_padding - (line_width - cursor_distance), 0.0_r};
		
		case TextBoxTextLayout::Center:
		default:
		return {-line_width * 0.5_r + cursor_distance, 0.0_r};
	}
}


/*
	Content
*/

std::string trim_content(std::string content, TextBoxTextMode text_mode, TextBoxCharacterSet character_set) noexcept
{
	using namespace utilities;

	if (character_set == TextBoxCharacterSet::ASCII)
		string::RemoveNonAscii(content);

	switch (text_mode)
	{
		case TextBoxTextMode::Alpha:
		return string::RemoveNonAlphaCopy(std::move(content));

		case TextBoxTextMode::Numeric:
		return string::RemoveNonNumericCopy(std::move(content));

		case TextBoxTextMode::AlphaNumeric:
		return string::RemoveNonAlphaNumericCopy(std::move(content));

		case TextBoxTextMode::Printable:
		default:
		return string::RemoveNonPrintableCopy(std::move(content));
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

std::string trim_placeholder_content(std::string content) noexcept
{
	using namespace utilities;
	string::ReplaceAll(content, "<br>", "\n");
	string::RemoveNonPrintable(content);
	return content;
}


real char_width(char c, graphics::fonts::Font &font) noexcept
{
	return graphics::fonts::utilities::MeasureCharacter(c, font).value_or(vector2::Zero).X();
}

real string_width(std::string_view str, graphics::fonts::Font &font) noexcept
{
	return graphics::fonts::utilities::MeasureString(str, font).value_or(vector2::Zero).X();
}

bool reveal_character(char c, real &width, int max_width, graphics::fonts::Font &font) noexcept
{
	if (auto c_width = char_width(c, font);
		static_cast<int>(std::ceil(width + c_width)) > max_width) //Too wide
		return false;
	else
	{
		width += c_width;
		return true;
	}
}

bool trim_character(char c, real &width, int max_width, graphics::fonts::Font &font) noexcept
{
	auto c_width = char_width(c, font);
	width -= c_width;
	return static_cast<int>(std::ceil(width)) < max_width;
}


graphics::fonts::Font* get_font(const graphics::fonts::Text &text) noexcept
{
	if (auto type_face = text.Lettering(); type_face)
		return graphics::fonts::utilities::detail::get_font(*type_face, text.DefaultFontStyle());
	else
		return nullptr;
}

std::pair<int, int> get_content_view(std::string_view content, int cursor_position, std::pair<int, int> content_view,
	std::optional<char> mask, int reveal_count, const graphics::fonts::Text &text) noexcept
{
	if (auto area_size = text.AreaSize(); area_size)
	{
		if (auto font = get_font(text); font)
		{
			auto max_width = static_cast<int>(
				graphics::fonts::text::detail::text_area_max_size(*area_size, text.Padding()).X()
			);
			auto width = 0.0_r;

			//Move left
			if (cursor_position < content_view.first)
			{
				content_view = {cursor_position + 1, cursor_position + 1};

				//Reveal left (by reveal count)
				for (auto off = cursor_position;
					off >= 0 && cursor_position - off <= reveal_count; --off)
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
						content_view.second = off + 1;
					else
						break;
				}
			}
			//Move right
			else if (cursor_position > content_view.second)
			{
				--cursor_position;
				content_view = {cursor_position, cursor_position};

				//Reveal right (by reveal count)
				for (auto off = cursor_position;
					off < std::ssize(content) && off - cursor_position <= reveal_count; ++off)
				{
					if (reveal_character(mask ? *mask : content[off], width, max_width, *font))
						content_view.second = off + 1;
					else
						break;
				}

				//Reveal left
				for (auto off = cursor_position - 1; off >= 0; --off)
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
				if (content_view.first == content_view.second)
					content_view = {0, std::ssize(content)};

				width = mask ?
					char_width(*mask, *font) * (content_view.second - content_view.first) :
					string_width(content.substr(content_view.first, content_view.second - content_view.first), *font);

				//Too wide, trim from left, then right
				if (static_cast<int>(std::ceil(width)) > max_width)
				{
					auto fits = false;

					//Trim from left
					for (auto off = content_view.first; !fits && off < cursor_position; ++off)
					{
						fits = trim_character(mask ? *mask : content[off], width, max_width, *font);
						content_view.first = off + 1;
					}

					//Trim from right
					for (auto off = content_view.second - 1; !fits && off > cursor_position; --off)
					{
						fits = trim_character(mask ? *mask : content[off], width, max_width, *font);
						content_view.second = off;
					}
				}

				//Could be more space, reveal right, then left
				if (static_cast<int>(std::ceil(width)) < max_width)
				{
					//Reveal right
					for (auto off = content_view.second; off < std::ssize(content); ++off)
					{
						if (reveal_character(mask ? *mask : content[off], width, max_width, *font))
							content_view.second = off + 1;
						else
							break;
					}

					//Reveal left
					for (auto off = content_view.first - 1; off >= 0; --off)
					{
						if (reveal_character(mask ? *mask : content[off], width, max_width, *font))
							content_view.first = off;
						else
							break;
					}
				}
			}

			return content_view;
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


int get_cursor_position(const Vector2 &position, const Vector2 &scaling, std::string_view str, graphics::fonts::Font &font) noexcept
{
	using namespace utilities;
	auto cursor_position = 0;

	if (auto x = position.X(); x > 0.0_r)
	{
		for (auto width = 0.0_r; auto &c : str)
		{
			if (auto c_width = char_width(c, font) * scaling.X(); width + c_width >= x)
				//x is in range (width, width + c_width]
				return cursor_position + static_cast<int>(math::Round((x - width) / c_width));
			else
			{
				width += c_width;
				++cursor_position;
			}
		}
	}

	return cursor_position;
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

	if (auto skin = static_cast<TextBoxSkin*>(skin_.get()); skin && skin->Cursor)
		skin->Cursor->Visible(!read_only_);

	GuiControl::Focused(); //Use base functionality
}

void GuiTextBox::Defocused() noexcept
{
	if (repeat_key_ || repeat_char_)
	{
		SetRepeatPhase(detail::key_repeat_phase::PreRepeat);
		repeat_phase_duration_.Total(0.0_sec);
		repeat_key_ = {};
		repeat_char_ = {};
	}

	ctrl_pressed_ = false;

	if (auto skin = static_cast<TextBoxSkin*>(skin_.get()); skin && skin->Cursor)
	{
		SetBlinkPhase(detail::cursor_blink_phase::Hold);
		blink_phase_duration_.Total(0.0_sec);
		SetCursorOpacity(1.0_r);
		skin->Cursor->Visible(false);
	}

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

int GuiTextBox::ScrollValue() noexcept
{
	if (auto skin = static_cast<TextBoxSkin*>(skin_.get());
		skin && skin->Text && skin->Text->GetImmutable())
		return skin->Text->GetImmutable()->FromLine();
	else
		return 0;
}


void GuiTextBox::CursorMoved() noexcept
{
	//Optional to override
}


/*
	States
*/

void GuiTextBox::SetSkinState(gui_control::ControlState state, TextBoxSkin &skin) noexcept
{
	if (skin.Text)
		SetCaptionState(state, skin.Text);

	if (skin.PlaceholderText)
		SetCaptionState(state, skin.PlaceholderText);

	if (skin.Cursor)
	{
		SetPartState(state, skin.Cursor);
		skin.Cursor->Visible(focused_ && !read_only_);
	}
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

OwningPtr<gui_control::ControlSkin> GuiTextBox::AttuneSkin(OwningPtr<gui_control::ControlSkin> skin) const
{
	//Not fully compatible
	if (skin && !dynamic_cast<TextBoxSkin*>(skin.get()))
	{
		auto text_box_skin = make_owning<TextBoxSkin>();
		text_box_skin->Assign(*skin);
		return text_box_skin;
	}
	else
		return skin;
}


void GuiTextBox::AttachSkin()
{
	GuiControl::AttachSkin(); //Use base functionality

	if (auto skin = static_cast<TextBoxSkin*>(skin_.get()); skin && skin_node_)
	{
		if (skin->Text)
		{
			//Detach from previous parent (if any)
			if (auto node = skin->Text->ParentNode(); node)
				node->DetachObject(*skin->Text.Object);
		
			//Attach text
			skin_node_->AttachObject(*skin->Text.Object);
		}

		if (skin->PlaceholderText)
		{
			//Detach from previous parent (if any)
			if (auto node = skin->PlaceholderText->ParentNode(); node)
				node->DetachObject(*skin->PlaceholderText.Object);
		
			//Attach placeholder text
			skin_node_->AttachObject(*skin->PlaceholderText.Object);
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
			skin->Text->Owner()->RemoveText(*skin->Text.Object); //Remove text

		if (skin->PlaceholderText && skin->PlaceholderText->Owner())
			skin->PlaceholderText->Owner()->RemoveText(*skin->PlaceholderText.Object); //Remove placeholder text
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
			if (auto size = ContentSize(); size)
			{
				auto center = ContentArea().value_or(aabb::Zero).Center();

				auto ppu = Engine::PixelsPerUnit();
				text->Formatting(graphics::fonts::text::TextFormatting::None);
				text->Overflow(graphics::fonts::text::TextOverflow::Truncate);
				text->AreaSize(*size * ppu);
				text->Padding(text_padding_.value_or(detail::default_text_padding_size));
				text->Alignment(detail::text_layout_to_text_alignment(text_layout_));
				text->VerticalAlignment(graphics::fonts::text::TextVerticalAlignment::Middle);

				if (skin->PlaceholderText)
					skin->PlaceholderText->Visible(false);

				if (!std::empty(content_))
				{
					//Clamp
					content_view_ = {
						std::clamp(content_view_.first, 0, std::ssize(content_)),
						std::clamp(content_view_.second, content_view_.first, std::ssize(content_))
					};

					content_view_ =
						detail::get_content_view(content_, cursor_position_, content_view_,
							mask_, reveal_count_.value_or(detail::default_reveal_count), *text);
					text->Content(detail::get_viewed_content(content_, content_view_, mask_));
				}
				else
				{
					content_view_ = {};
					text->Clear();

					if (skin->PlaceholderText && placeholder_content_ &&
						(!focused_ || read_only_))
					{
						//Placeholder text
						if (auto &placeholder_text = skin->PlaceholderText->Get(); placeholder_text)
						{
							placeholder_text->Overflow(graphics::fonts::text::TextOverflow::TruncateEllipsis);
							placeholder_text->AreaSize(*size * ppu);
							placeholder_text->Padding(text_padding_.value_or(detail::default_text_padding_size));
							placeholder_text->Alignment(detail::text_layout_to_text_alignment(text_layout_));
							placeholder_text->VerticalAlignment(graphics::fonts::text::TextVerticalAlignment::Middle);
							placeholder_text->Content(*placeholder_content_);
							skin->PlaceholderText->Visible(true);
						}

						skin->PlaceholderText->Position(center);
					}
				}

				skin->Text->Position(center);
			}
		}
	}

	UpdateCursor();
}

void GuiTextBox::UpdateCursor() noexcept
{
	if (read_only_)
		return;

	if (auto skin = static_cast<TextBoxSkin*>(skin_.get());
		skin && skin->Cursor && skin->Text)
	{
		//Lines text
		if (auto &text = skin->Text->GetImmutable();
			text && text->LineHeight())
		{
			auto font = detail::get_font(*text);

			if (auto size = ContentSize(); size && font)
			{
				auto [width, height] = size->XY();
				auto center = ContentArea().value_or(aabb::Zero).Center();

				auto ppu = Engine::PixelsPerUnit();
				auto line_height = *text->LineHeight() / ppu;
				auto line_padding = text->Padding().X() / ppu;

				auto [cursor_width, cursor_height] = skin->Cursor->Size().XY();
				auto aspect_ratio = cursor_width / cursor_height;

				//Cursor should keep proportions when resized
				skin->Cursor->Size(
					{line_height * aspect_ratio, line_height}
				);

				auto cursor_distance = detail::string_width(
					detail::get_viewed_content(content_, {content_view_.first, cursor_position_}, mask_), *font) / ppu;
				auto line_width = cursor_distance + detail::string_width(
					detail::get_viewed_content(content_, {cursor_position_, content_view_.second}, mask_), *font) / ppu;

				skin->Cursor->Position(
					Vector3{center.X(), center.Y(), skin->Cursor->Position().Z()} +
					detail::cursor_offset(width, line_width, line_padding, cursor_distance, text_layout_)
				);
			}
		}
	}
}


void GuiTextBox::SetCursorOpacity(real percent) noexcept
{
	if (auto skin = static_cast<TextBoxSkin*>(skin_.get()); skin && skin->Cursor)
		skin->Cursor->FillOpacity(percent);

	cursor_opacity_ = percent;
}


/*
	Phase
*/

void GuiTextBox::SetBlinkPhase(detail::cursor_blink_phase phase) noexcept
{
	blink_phase_ = phase;
	UpdateBlinkPhaseDuration();
}

void GuiTextBox::SetRepeatPhase(detail::key_repeat_phase phase) noexcept
{
	repeat_phase_ = phase;
	UpdateRepeatPhaseDuration();
}

void GuiTextBox::UpdateBlinkPhaseDuration() noexcept
{
	auto limit =
		[&]() noexcept
		{
			switch (blink_phase_)
			{
				case detail::cursor_blink_phase::FadeIn:
				case detail::cursor_blink_phase::FadeOut:
				return cursor_blink_rate_ * (1.0_r - cursor_hold_percent_) * 0.5_r;

				case detail::cursor_blink_phase::Hold:
				default:
				return cursor_blink_rate_ * cursor_hold_percent_ * 0.5_r;
			}
		}();

	blink_phase_duration_.Limit(limit);
}

void GuiTextBox::UpdateRepeatPhaseDuration() noexcept
{
	auto limit =
		[&]() noexcept
		{
			switch (repeat_phase_)
			{
				case detail::key_repeat_phase::PreRepeat:
				return key_repeat_delay_;

				case detail::key_repeat_phase::Repeat:
				default:
				return key_repeat_rate_;
			}
		}();

	repeat_phase_duration_.Limit(limit);
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

GuiTextBox::GuiTextBox(std::string name, const std::optional<Vector2> &size,
	std::optional<std::string> caption, gui_control::BoundingBoxes hit_boxes) noexcept :
	GuiScrollable{std::move(name), size, std::move(caption), {}, std::move(hit_boxes)}
{
	DefaultSetup();
}

GuiTextBox::GuiTextBox(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
	std::optional<std::string> caption, gui_control::BoundingBoxes hit_boxes) :
	GuiScrollable{std::move(name), skin, size, std::move(caption), {}, std::move(hit_boxes)}
{
	DefaultSetup();
}


/*
	Content - Adding/inserting
*/

void GuiTextBox::AddContent(std::string content)
{
	InsertContent(std::ssize(content_), std::move(content));
}

void GuiTextBox::InsertContent(int off, std::string content)
{
	if (off >= 0)
	{
		content = gui_text_box::detail::trim_content(std::move(content), text_mode_, character_set_);

		if (max_characters_)
			content = gui_text_box::detail::truncate_content(std::move(content),
				*max_characters_ - std::ssize(content_));

		if (!std::empty(content))
		{
			off = std::clamp(off, 0, std::ssize(content_));
			content_.insert(std::begin(content_) + off, std::begin(content), std::end(content));

			//Adjust cursor position
			if (cursor_position_ >= off)
				CursorPosition(cursor_position_ + std::ssize(content));

			InsertTextContent(off, std::move(content));
			UpdateText();
			Changed();
		}
	}
}


/*
	Content - Replacing
*/

void GuiTextBox::ReplaceContent(int off, std::string content)
{
	ReplaceContent(off, off + 1, std::move(content));
}

void GuiTextBox::ReplaceContent(int first, int last, std::string content)
{
	if (first >= 0 && first < last)
	{
		content = gui_text_box::detail::trim_content(std::move(content), text_mode_, character_set_);

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
		Changed();
	}
}


/*
	Content - Removing
*/

void GuiTextBox::ClearContent() noexcept
{
	if (!std::empty(content_))
	{
		content_.clear();

		CursorPosition(0);
		ClearTextContent();
		UpdateText();
		Changed();
	}

	content_.shrink_to_fit();
}

void GuiTextBox::RemoveContent(int off) noexcept
{
	RemoveContent(off, off + 1);
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
		Changed();
	}
}


/*
	Content - Clipboard
*/

bool GuiTextBox::CopyContent() noexcept
{
	return system::utilities::Clipboard(
		detail::get_viewed_content(content_, {0, std::ssize(content_)}, mask_)
	);
}

bool GuiTextBox::CopyContent(int first, int last) noexcept
{
	first = std::clamp(first, 0, std::ssize(content_));
	last = std::clamp(last, first, std::ssize(content_));

	return system::utilities::Clipboard(
		detail::get_viewed_content(content_, {first, last}, mask_)
	);
}


bool GuiTextBox::CutContent() noexcept
{
	if (CopyContent())
	{
		ClearContent();
		return true;
	}
	else
		return false;
}

bool GuiTextBox::CutContent(int first, int last) noexcept
{
	if (CopyContent(first, last))
	{
		RemoveContent(first, last);
		return true;
	}
	else
		return false;
}


bool GuiTextBox::PasteContent()
{
	if (auto str = system::utilities::Clipboard(); str)
	{
		AddContent(std::move(*str));
		return true;
	}
	else
		return false;
}

bool GuiTextBox::PasteContent(int off)
{
	if (auto str = system::utilities::Clipboard(); str)
	{
		InsertContent(off, std::move(*str));
		return true;
	}
	else
		return false;
}

bool GuiTextBox::PasteContent(int first, int last)
{
	if (auto str = system::utilities::Clipboard(); str)
	{
		ReplaceContent(first, last, std::move(*str));
		return true;
	}
	else
		return false;
}


/*
	Frame events
*/

void GuiTextBox::FrameStarted(duration time) noexcept
{
	if (visible_ && focused_)
	{
		//Key repeat
		if (repeat_key_ || repeat_char_)
		{
			if (repeat_phase_duration_ += time)
			{
				repeat_phase_duration_.ResetWithCarry();

				//Switch to next phase
				SetRepeatPhase(
					[&]() noexcept
					{
						switch (repeat_phase_)
						{
							case detail::key_repeat_phase::PreRepeat:
							case detail::key_repeat_phase::Repeat:
							default:
							return detail::key_repeat_phase::Repeat;
						}
					}());

				if (repeat_key_)
					KeyPressed(*repeat_key_);
				else if (repeat_char_)
				{
					auto c = *repeat_char_;
					repeat_char_ = {};
					CharacterPressed(c);
				}
			}

			SetBlinkPhase(detail::cursor_blink_phase::Hold);
			blink_phase_duration_.Total(0.0_sec);
			SetCursorOpacity(1.0_r);
		}

		//Cursor blink
		else if (blink_phase_duration_ += time)
		{
			blink_phase_duration_.ResetWithCarry();

			switch (blink_phase_)
			{
				case detail::cursor_blink_phase::FadeIn:
				SetCursorOpacity(1.0_r);
				break;

				case detail::cursor_blink_phase::FadeOut:
				SetCursorOpacity(0.0_r);
				break;
			}

			//Switch to next phase
			SetBlinkPhase(
				[&]() noexcept
				{
					switch (blink_phase_)
					{
						case detail::cursor_blink_phase::FadeIn:
						case detail::cursor_blink_phase::FadeOut:
						return detail::cursor_blink_phase::Hold;

						case detail::cursor_blink_phase::Hold:
						default:
						return cursor_opacity_ == 0.0_r ?
							detail::cursor_blink_phase::FadeIn :
							detail::cursor_blink_phase::FadeOut;
					}
				}());
		}

		switch (blink_phase_)
		{
			case detail::cursor_blink_phase::FadeIn:
			SetCursorOpacity(blink_phase_duration_.Percent());
			break;

			case detail::cursor_blink_phase::FadeOut:
			SetCursorOpacity(1.0_r - blink_phase_duration_.Percent());
			break;
		}
	}
}


/*
	Key events
*/

bool GuiTextBox::KeyPressed(KeyButton button) noexcept
{
	switch (button)
	{
		//Move cursor left	
		case KeyButton::LeftArrow:
		case KeyButton::DownArrow:
		{
			if (read_only_)
				break;

			CursorPosition(cursor_position_ - 1);
			repeat_char_ = {};
			repeat_key_ = button;
			return true;
		}

		//Move cursor right
		case KeyButton::RightArrow:
		case KeyButton::UpArrow:
		{
			if (read_only_)
				break;

			CursorPosition(cursor_position_ + 1);
			repeat_char_ = {};
			repeat_key_ = button;
			return true;
		}

		case KeyButton::Backspace:
		{
			if (read_only_)
				break;

			RemoveContent(cursor_position_ - 1);
			repeat_char_ = {};
			repeat_key_ = button;
			return true;
		}

		case KeyButton::Delete:
		{
			if (read_only_)
				break;

			RemoveContent(cursor_position_);
			repeat_char_ = {};
			repeat_key_ = button;
			return true;
		}


		case KeyButton::Ctrl:
		ctrl_pressed_ = true;
		return true;

		case KeyButton::Alt:
		ctrl_pressed_ = false;
		return true;

		case KeyButton::C:
		{
			if (ctrl_pressed_)
				CopyContent();

			return true;
		}

		case KeyButton::X:
		{
			if (read_only_)
				break;

			if (ctrl_pressed_)
				CutContent();

			return true;
		}

		case KeyButton::V:
		{
			if (read_only_)
				break;

			if (ctrl_pressed_)
				PasteContent(cursor_position_);

			return true;
		}
	}

	return false;
}

bool GuiTextBox::KeyReleased(KeyButton button) noexcept
{
	if (repeat_key_ || repeat_char_)
	{
		SetRepeatPhase(detail::key_repeat_phase::PreRepeat);
		repeat_phase_duration_.Total(0.0_sec);
		repeat_key_ = {};
		repeat_char_ = {};
	}

	switch (button)
	{
		case KeyButton::Home:
		{
			if (read_only_)
				break;

			CursorPosition(0);
			return true;
		}

		case KeyButton::End:
		{
			if (read_only_)
				break;

			CursorPosition(std::ssize(content_));
			return true;
		}

		case KeyButton::LeftArrow:
		case KeyButton::DownArrow:
		case KeyButton::RightArrow:
		case KeyButton::UpArrow:
		case KeyButton::Backspace:
		case KeyButton::Delete:
		{
			if (read_only_)
				break;

			return true;
		}

		case KeyButton::Ctrl:
		ctrl_pressed_ = false;
		return true;
	}

	return GuiScrollable::KeyReleased(button); //Use base functionality
}

bool GuiTextBox::CharacterPressed(char character) noexcept
{
	if (!read_only_ && character != '\b' && character != '\x7f')
	{
		if (!repeat_char_ || *repeat_char_ != character)
		{
			InsertContent(cursor_position_, std::string(1, character));
			repeat_key_ = {};
			repeat_char_ = character;
		}

		return true;
	}
	else
		return false;
}


/*
	Mouse events
*/

bool GuiTextBox::MouseReleased(MouseButton button, Vector2 position) noexcept
{
	using namespace utilities;

	if (!read_only_ && button == MouseButton::Left)
	{
		if (auto skin = static_cast<TextBoxSkin*>(skin_.get());
			skin && skin->Cursor && skin->Text)
		{
			//Text
			if (auto &text = skin->Text->GetImmutable(); text)
			{
				auto font = detail::get_font(*text);

				if (auto size = ContentSize(); size && font)
				{
					auto [width, height] = size->XY();
					auto center = ContentArea().value_or(aabb::Zero).Center();

					auto ppu = Engine::PixelsPerUnit();
					auto line_width = detail::string_width(
						detail::get_viewed_content(content_, {content_view_.first, content_view_.second}, mask_), *font) / ppu;
					auto line_padding = text->Padding().X() / ppu;
					auto scaling = vector2::UnitScale;
					
					if (skin_node_)
					{
						position = //Make position relative to text
							(position - (skin_node_->DerivedPosition() + center)).
							RotateCopy(-skin_node_->DerivedRotation(), vector2::Zero);
						
						width *= skin_node_->DerivedScaling().X();
						line_width *= skin_node_->DerivedScaling().X();
						line_padding *= skin_node_->DerivedScaling().X();
						scaling *= skin_node_->DerivedScaling();
					}

					CursorPosition(
						content_view_.first +
						detail::get_cursor_position((position -
							detail::cursor_offset(width, line_width, line_padding, 0.0_r, text_layout_)) * ppu,
							scaling, detail::get_viewed_content(content_, content_view_, mask_), *font)
						);
				}
			}
		}
	}

	return false;
}

} //ion::gui::controls