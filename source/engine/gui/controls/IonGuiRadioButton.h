/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiRadioButton.h
-------------------------------------------
*/

#ifndef ION_GUI_RADIO_BUTTON_H
#define ION_GUI_RADIO_BUTTON_H

#include <optional>
#include <string>

#include "IonGuiCheckBox.h"
#include "graphics/utilities/IonVector2.h"
#include "memory/IonOwningPtr.h"

namespace ion::gui::controls
{
	using namespace graphics::utilities;

	namespace gui_radio_button
	{
		struct RadioButtonSkin : gui_check_box::CheckBoxSkin
		{
			//Empty
		};


		namespace detail
		{
		} //detail
	} //gui_radio_button


	class GuiRadioButton : public GuiCheckBox
	{
		protected:

			int tag_ = 0;


			/*
				Events
			*/

			//See GuiCheckBox::Checked for more details
			virtual void Checked() noexcept override;

			//See GuiCheckBox::Unchecked for more details
			virtual void Unchecked() noexcept override;


			/*
				Tags
			*/

			void Unselect() noexcept;
			void UnselectEqualTag(int tag) noexcept;
			bool UniqueTag(int tag) const noexcept;

		public:

			//Construct a radio button with the given name, caption, tooltip and skin
			GuiRadioButton(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
				OwningPtr<gui_radio_button::RadioButtonSkin> skin);

			//Construct a radio button with the given name, caption, tooltip, skin and size
			GuiRadioButton(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
				OwningPtr<gui_radio_button::RadioButtonSkin> skin, const Vector2 &size);

			//Construct a radio button with the given name, caption, tooltip, skin and hit areas
			GuiRadioButton(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
				OwningPtr<gui_radio_button::RadioButtonSkin> skin, gui_control::Areas areas);


			/*
				Modifiers
			*/

			//Select this radio button
			void Select() noexcept;

			//Sets the tag used for this radio button to the given tag
			void Tag(int tag) noexcept;


			/*
				Observers
			*/

			//Returns true if this radio button is selected
			[[nodiscard]] inline auto IsSelected() const noexcept
			{
				return IsChecked();
			}

			//Returns the tag used for this radio button
			[[nodiscard]] inline auto Tag() const noexcept
			{
				return tag_;
			}
	};

} //ion::gui::controls

#endif