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


	///@brief A class representing a GUI radio button
	///@details A radio button can only be selected (checked) never directly unselected (unchecked).
	///When selected, all other radio buttons in the same group (same tag value) gets unselected
	class GuiRadioButton : public GuiCheckBox
	{
		protected:

			int tag_ = 0;


			/**
				@name Events
				@{
			*/

			///@brief See GuiCheckBox::Checked for more details
			virtual void Checked() noexcept override;

			///@brief See GuiCheckBox::Unchecked for more details
			virtual void Unchecked() noexcept override;

			///@}

			/**
				@name Skins
				@{
			*/

			virtual OwningPtr<gui_control::ControlSkin> AttuneSkin(OwningPtr<gui_control::ControlSkin> skin) const override;

			///@}

			/**
				@name Tags
				@{
			*/
			
			void UnselectEqualTag(int tag) noexcept;
			bool UniqueTag(int tag) const noexcept;

			///@}

		public:

			///@brief Constructs a radio button with the given name, size, caption, tooltip and hit boxes
			GuiRadioButton(std::string name, const std::optional<Vector2> &size,
				std::optional<std::string> caption, std::optional<std::string> tooltip, gui_control::BoundingBoxes hit_boxes = {}) noexcept;

			///@brief Constructs a radio button with the given name, skin, size, caption, tooltip and hit boxes
			GuiRadioButton(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
				std::optional<std::string> caption, std::optional<std::string> tooltip, gui_control::BoundingBoxes hit_boxes = {});


			/**
				@name Modifiers
				@{
			*/

			///@brief Selects this radio button
			void Select() noexcept;

			///@brief Unselects this radio button
			void Unselect() noexcept;

			///@brief Sets the tag used for this radio button to the given tag
			void Tag(int tag) noexcept;

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns true if this radio button is selected
			[[nodiscard]] inline auto IsSelected() const noexcept
			{
				return IsChecked();
			}

			///@brief Returns the tag used for this radio button
			[[nodiscard]] inline auto Tag() const noexcept
			{
				return tag_;
			}

			///@}
	};

} //ion::gui::controls

#endif