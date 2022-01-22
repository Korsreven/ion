/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiTextBox.h
-------------------------------------------
*/

#ifndef ION_GUI_TEXT_BOX_H
#define ION_GUI_TEXT_BOX_H

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "IonGuiScrollable.h"
#include "events/listeners/IonKeyListener.h"
#include "events/listeners/IonMouseListener.h"
#include "graphics/fonts/IonText.h"
#include "graphics/utilities/IonVector2.h"
#include "memory/IonOwningPtr.h"
#include "types/IonCumulative.h"
#include "types/IonTypes.h"

//Forward declarations
namespace ion
{
	namespace graphics
	{
		namespace materials
		{
			class Material;
		}

		namespace scene::shapes
		{
			class Sprite;
		}
	}
}

namespace ion::gui::controls
{
	using namespace events::listeners;
	using namespace graphics::utilities;
	using namespace types::type_literals;

	namespace gui_text_box
	{
		enum class TextBoxTextMode
		{
			AlphaNumeric,
			Alpha,
			Numeric
		};

		enum class TextBoxTextLayout
		{
			Left,
			Center,
			Right
		};


		struct TextBoxSkin : gui_control::ControlSkin
		{
			gui_control::ControlCaptionPart Text;
			gui_control::ControlVisualPart Cursor;
		};


		namespace detail
		{
			enum class cursor_blink_phase
			{
				FadeIn,
				Hold,
				FadeOut
			};

			enum class key_repeat_phase : bool
			{
				PreRepeat,
				Repeat
			};


			constexpr auto default_text_padding_size = 2.0_r;
			constexpr auto default_reveal_count = 10;

			constexpr auto default_cursor_blink_rate = 1.0_sec;
			constexpr auto default_cursor_hold_percent = 0.7_r;
			constexpr auto default_key_repeat_rate = 0.03_sec;
			constexpr auto default_key_repeat_delay = 0.5_sec;


			inline auto text_layout_to_text_alignment(TextBoxTextLayout text_layout) noexcept
			{
				switch (text_layout)
				{
					case TextBoxTextLayout::Left:
					return graphics::fonts::text::TextAlignment::Left;

					case TextBoxTextLayout::Right:
					return graphics::fonts::text::TextAlignment::Right;

					default:
					return graphics::fonts::text::TextAlignment::Center;
				}
			}

			Vector2 cursor_offset(real width, real line_width, real cursor_distance, TextBoxTextLayout text_layout) noexcept;


			/*
				Content
			*/

			std::string trim_content(std::string content, TextBoxTextMode text_mode) noexcept;
			std::string truncate_content(std::string content, int max_characters) noexcept;
			std::string mask_content(std::string content, char mask) noexcept;

			real char_width(char c, graphics::fonts::Font &font) noexcept;
			real string_width(std::string_view str, graphics::fonts::Font &font) noexcept;
			bool reveal_character(char c, real &width, int max_width, graphics::fonts::Font &font) noexcept;
			bool trim_character(char c, real &width, int max_width, graphics::fonts::Font &font) noexcept;

			graphics::fonts::Font* get_font(const graphics::fonts::Text &text) noexcept;
			std::pair<int, int> get_content_view(std::string_view content, int cursor_position, std::pair<int, int> content_view,
				std::optional<char> mask, int reveal_count, const graphics::fonts::Text &text) noexcept;
			std::string get_viewed_content(const std::string &content, std::pair<int, int> content_view, std::optional<char> mask);
		} //detail
	} //gui_text_box


	class GuiTextBox : public GuiScrollable
	{
		private:

			void DefaultSetup() noexcept;

		protected:

			std::string content_;
			std::optional<std::string> placeholder_content_;
			std::optional<int> max_characters_;
			std::optional<char> mask_;

			std::optional<Vector2> text_padding_;
			gui_text_box::TextBoxTextMode text_mode_ = gui_text_box::TextBoxTextMode::AlphaNumeric;
			gui_text_box::TextBoxTextLayout text_layout_ = gui_text_box::TextBoxTextLayout::Left;

			int cursor_position_ = 0;
			std::optional<int> reveal_count_;
			std::pair<int, int> content_view_;
			
			duration cursor_blink_rate_ = gui_text_box::detail::default_cursor_blink_rate;
			real cursor_hold_percent_ = gui_text_box::detail::default_cursor_hold_percent;
			duration key_repeat_rate_ = gui_text_box::detail::default_key_repeat_rate;
			duration key_repeat_delay_ = gui_text_box::detail::default_key_repeat_delay;

			gui_text_box::detail::cursor_blink_phase blink_phase_ = gui_text_box::detail::cursor_blink_phase::Hold;
			gui_text_box::detail::key_repeat_phase repeat_phase_ = gui_text_box::detail::key_repeat_phase::PreRepeat;
			types::Cumulative<duration> blink_phase_duration_{cursor_blink_rate_ * cursor_hold_percent_};
			types::Cumulative<duration> repeat_phase_duration_{key_repeat_delay_};

			real cursor_opacity_ = 1.0_r;
			std::optional<KeyButton> repeat_key_;
			std::optional<char> repeat_char_;


			/*
				Events
			*/

			//See GuiControl::Focused for more details
			virtual void Focused() noexcept override;

			//See GuiControl::Defocused for more details
			virtual void Defocused() noexcept override;

			//See GuiControl::Resized for more details
			virtual void Resized(Vector2 from_size, Vector2 to_size) noexcept override;


			//See GuiScrollable::Scrolled for more details
			virtual void Scrolled(int delta) noexcept override;

			//See GuiScrollable::TotalElements for more details
			virtual int TotalElements() noexcept override;

			//See GuiScrollable::ElementsInView for more details
			virtual int ElementsInView() noexcept override;

			//See GuiScrollable::ScrollPosition for more details
			virtual int ScrollPosition() noexcept override;


			/*
				States
			*/

			void SetSkinState(gui_control::ControlState state, gui_text_box::TextBoxSkin &skin) noexcept;
			virtual void SetState(gui_control::ControlState state) noexcept override;


			/*
				Skins
			*/

			virtual void AttachSkin() override;
			virtual void DetachSkin() noexcept override;
			virtual void RemoveSkin() noexcept override;
			
			virtual void UpdateText() noexcept;
			virtual void UpdateCursor() noexcept;

			void SetCursorOpacity(real percent) noexcept;


			/*
				Phase
			*/

			void SetBlinkPhase(gui_text_box::detail::cursor_blink_phase phase) noexcept;
			void SetRepeatPhase(gui_text_box::detail::key_repeat_phase phase) noexcept;
			void UpdateBlinkPhaseDuration() noexcept;
			void UpdateRepeatPhaseDuration() noexcept;


			/*
				Content
			*/

			void InsertTextContent(int off, std::string content);
			void ReplaceTextContent(int first, int last, std::string content);
			void RemoveTextContent(int first, int last) noexcept;
			void ClearTextContent() noexcept;

			void MoveContentView(int delta) noexcept;

		public:

			//Construct a text box with the given name, caption, tooltip, skin and hit boxes
			GuiTextBox(std::string name, std::optional<std::string> caption,
				OwningPtr<gui_text_box::TextBoxSkin> skin, gui_control::BoundingBoxes hit_boxes = {});

			//Construct a text box with the given name, caption, tooltip, skin, size and hit boxes
			GuiTextBox(std::string name, std::optional<std::string> caption,
				OwningPtr<gui_text_box::TextBoxSkin> skin, const Vector2 &size, gui_control::BoundingBoxes hit_boxes = {});


			/*
				Modifiers
			*/

			//Sets the content for this text box to the given content
			inline void Content(std::string content) noexcept
			{
				content = gui_text_box::detail::trim_content(std::move(content), text_mode_);

				if (max_characters_)
					content = gui_text_box::detail::truncate_content(std::move(content), *max_characters_);

				if (content_ != content)
				{
					content_ = content;
					UpdateText();
				}
			}

			//Sets the placeholder content for this text box to the given content
			inline void PlaceholderContent(std::optional<std::string> content) noexcept
			{
				if (content)
					content = gui_text_box::detail::trim_content(std::move(*content), text_mode_);

				if (placeholder_content_ != content)
				{
					placeholder_content_ = content;

					if (std::empty(content_) && !focused_)
						UpdateText();
				}
			}

			//Sets the max characters for this text box to the given max
			inline void MaxCharacters(std::optional<int> max) noexcept
			{
				if (max && *max < 0)
					max = 0;

				if (max_characters_ != max)
				{
					max_characters_ = max;

					if (max && *max < std::ssize(content_))
					{
						content_ = gui_text_box::detail::truncate_content(std::move(content_), *max);
						UpdateText();
					}
				}
			}

			//Sets the mask for this text box to the given character
			//The character given must be a graphical character
			inline void Mask(std::optional<char> character) noexcept
			{
				if (!character || std::isgraph(*character))
				{
					if (mask_ != character)
					{
						mask_ = character;
						UpdateText();
					}
				}
			}


			//Sets the text padding for this text box to the given padding
			inline void TextPadding(const std::optional<Vector2> &padding) noexcept
			{
				if (text_padding_ != padding)
				{
					text_padding_ = padding;
					UpdateText();
				}
			}

			//Sets the text mode for this text box to the given mode
			inline void TextMode(gui_text_box::TextBoxTextMode mode) noexcept
			{
				if (text_mode_ != mode)
				{
					text_mode_ = mode;
					UpdateText();
				}
			}

			//Sets the text layout for this text box to the given layout
			inline void TextLayout(gui_text_box::TextBoxTextLayout layout) noexcept
			{
				if (text_layout_ != layout)
				{
					text_layout_ = layout;
					UpdateText();
				}
			}


			//Sets the cursor position for this text box to the given position
			inline void CursorPosition(int position) noexcept
			{
				position = std::clamp(position, 0, std::ssize(content_));

				if (cursor_position_ != position)
				{
					cursor_position_ = position;

					//Out of view
					if (position < content_view_.first ||
						position > content_view_.second)
						UpdateText();
					else
						UpdateCursor();
				}
			}

			//Sets the reveal count for this text box to the given count
			inline void RevealCount(std::optional<int> count) noexcept
			{
				reveal_count_ = count;
			}


			//Sets the cursor blink rate for this text box to the given time
			inline void CursorBlinkRate(duration time) noexcept
			{
				if (cursor_blink_rate_ != time && time >= 0.0_sec)
				{
					cursor_blink_rate_ = time;
					UpdateBlinkPhaseDuration();
				}
			}

			//Sets the cursor hold percent for this text box to the given percentage
			inline void CursorHoldPercent(real percent) noexcept
			{
				percent = std::clamp(percent, 0.0_r, 1.0_r);

				if (cursor_hold_percent_ != percent)
				{
					cursor_hold_percent_ = percent;
					UpdateBlinkPhaseDuration();
				}
			}

			//Sets the key repeat rate for this text box to the given time
			inline void KeyRepeatRate(duration time) noexcept
			{
				if (key_repeat_rate_ != time && time >= 0.0_sec)
				{
					key_repeat_rate_ = time;
					UpdateRepeatPhaseDuration();
				}
			}

			//Sets the key repeat delay for this text box to the given time
			inline void KeyRepeatDelay(duration time) noexcept
			{
				if (key_repeat_delay_ != time && time >= 0.0_sec)
				{
					key_repeat_delay_ = time;
					UpdateRepeatPhaseDuration();
				}
			}


			/*
				Observers
			*/

			//Returns the content for this text box
			[[nodiscard]] inline auto Content() const noexcept
			{
				return content_;
			}

			//Returns the placeholder content for this text box
			//Returns nullopt if no custom placeholder content has been set
			[[nodiscard]] inline auto PlaceholderContent() const noexcept
			{
				return placeholder_content_;
			}

			//Returns the max characters for this text box
			//Returns nullopt if there is no character limit
			[[nodiscard]] inline auto MaxCharacters() const noexcept
			{
				return max_characters_;
			}

			//Returns the mask for this text box
			//Returns nullopt if there is no mask
			[[nodiscard]] inline auto Mask() const noexcept
			{
				return mask_;
			}


			//Returns the text padding for this text box
			//Returns nullopt if no custom text padding has been set
			[[nodiscard]] inline auto TextPadding() const noexcept
			{
				return text_padding_;
			}

			//Returns the text mode for this text box
			[[nodiscard]] inline auto TextMode() const noexcept
			{
				return text_mode_;
			}

			//Returns the text layout for this text box
			[[nodiscard]] inline auto TextLayout() const noexcept
			{
				return text_layout_;
			}


			//Returns the cursor position for this text box
			[[nodiscard]] inline auto CursorPosition() const noexcept
			{
				return cursor_position_;
			}

			//Returns the reveal count for this text box
			//Returns nullopt if no custom reveal count has been set
			[[nodiscard]] inline auto RevealCount() const noexcept
			{
				return reveal_count_;
			}


			//Returns the cursor blink rate for this text box
			[[nodiscard]] inline auto CursorBlinkRate() const noexcept
			{
				return cursor_blink_rate_;
			}

			//Returns the cursor hold percent for this text box
			[[nodiscard]] inline auto CursorHoldPercent() const noexcept
			{
				return cursor_hold_percent_;
			}

			//Returns the key repeat rate for this text box
			[[nodiscard]] inline auto KeyRepeatRate() const noexcept
			{
				return key_repeat_rate_;
			}

			//Returns the key repeat delay for this text box
			[[nodiscard]] inline auto KeyRepeatDelay() const noexcept
			{
				return key_repeat_delay_;
			}


			/*
				Content
				Adding/inserting
			*/

			//Adds the given content in the text box
			void AddContent(std::string content);

			//Inserts the given content, at the given offset in the text box
			void InsertContent(int off, std::string content);


			/*
				Content
				Replacing
			*/

			//Replaces the content at the given offset, with the given content
			void ReplaceContent(int off, std::string content);

			//Replaces the content in range [first, last), with the given content
			void ReplaceContent(int first, int last, std::string content);


			/*
				Content
				Removing
			*/

			//Clears all content in this text box
			void ClearContent() noexcept;

			//Removes content at the given offset in this text box
			void RemoveContent(int off) noexcept;

			//Removes all content in range [first, last) in this text box
			void RemoveContent(int first, int last) noexcept;


			/*
				Frame events
			*/

			//Called from gui control when a frame has started
			virtual void FrameStarted(duration time) noexcept override;


			/*
				Key events
			*/

			//Called from gui control when a key button has been pressed
			//Returns true if the key press event has been consumed by the control
			virtual bool KeyPressed(KeyButton button) noexcept override;

			//Called from gui control when a key button has been released
			//Returns true if the key release event has been consumed by the control
			virtual bool KeyReleased(KeyButton button) noexcept override;

			//Called from gui control when a character has been pressed
			//Returns true if the character press event has been consumed by the control
			virtual bool CharacterPressed(char character) noexcept override;


			/*
				Mouse events
			*/

			//Called from gui control when the mouse button has been released
			//Returns true if the mouse release event has been consumed by the control
			virtual bool MouseReleased(MouseButton button, Vector2 position) noexcept override;
	};

} //ion::gui::controls

#endif