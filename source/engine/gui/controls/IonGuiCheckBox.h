/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiCheckBox.h
-------------------------------------------
*/

#ifndef ION_GUI_CHECK_BOX_H
#define ION_GUI_CHECK_BOX_H

#include <optional>
#include <string>

#include "IonGuiControl.h"
#include "events/IonCallback.h"
#include "graphics/utilities/IonVector2.h"
#include "memory/IonOwningPtr.h"

namespace ion::gui::controls
{
	using namespace graphics::utilities;

	namespace gui_check_box
	{
		struct CheckBoxSkin : gui_control::ControlSkin
		{
			gui_control::ControlVisualPart CheckMark;
		};


		namespace detail
		{
			void resize_skin(CheckBoxSkin &skin, const Vector2 &from_size, const Vector2 &to_size) noexcept;
		} //detail
	} //gui_check_box


	class GuiCheckBox : public GuiControl
	{
		private:

			void DefaultSetup() noexcept;

		protected:

			bool checked_ = false;

			std::optional<events::Callback<void, GuiCheckBox&>> on_check_;
			std::optional<events::Callback<void, GuiCheckBox&>> on_uncheck_;


			/*
				Events
			*/

			//See GuiControl::Clicked for more details
			virtual void Clicked() noexcept override;

			//See GuiControl::Resized for more details
			virtual void Resized(const Vector2 &from_size, const Vector2 &to_size) noexcept override;


			//Called right after a check box has been checked
			virtual void Checked() noexcept;

			//Called right after a check box has been unchecked
			virtual void Unchecked() noexcept;


			/*
				States
			*/

			void SetSkinState(gui_control::ControlState state, gui_check_box::CheckBoxSkin &skin) noexcept;
			virtual void SetState(gui_control::ControlState state) noexcept override;

		public:

			//Construct a check box with the given name, caption, tooltip and skin
			GuiCheckBox(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
				OwningPtr<gui_check_box::CheckBoxSkin> skin);

			//Construct a check box with the given name, caption, tooltip, skin and size
			GuiCheckBox(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
				OwningPtr<gui_check_box::CheckBoxSkin> skin, const Vector2 &size);

			//Construct a check box with the given name, caption, tooltip, skin and hit areas
			GuiCheckBox(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
				OwningPtr<gui_check_box::CheckBoxSkin> skin, gui_control::Areas areas);


			/*
				Modifiers
			*/

			//Check this check box
			void Check() noexcept;

			//Uncheck this check box
			void Uncheck() noexcept;


			//Sets whether or not this check box is checked
			inline void Checked(bool checked) noexcept
			{
				if (checked)
					Check();
				else
					Uncheck();
			}


			//Sets the on check callback
			inline void OnCheck(events::Callback<void, GuiCheckBox&> on_check) noexcept
			{
				on_check_ = on_check;
			}

			//Sets the on check callback
			inline void OnCheck(std::nullopt_t) noexcept
			{
				on_check_ = {};
			}


			//Sets the on uncheck callback
			inline void OnUncheck(events::Callback<void, GuiCheckBox&> on_uncheck) noexcept
			{
				on_uncheck_ = on_uncheck;
			}

			//Sets the on uncheck callback
			inline void OnUncheck(std::nullopt_t) noexcept
			{
				on_uncheck_ = {};
			}


			/*
				Observers
			*/

			//Returns true if this check box is checked
			[[nodiscard]] inline auto IsChecked() const noexcept
			{
				return checked_;
			}


			//Returns the on check callback
			[[nodiscard]] inline auto OnCheck() const noexcept
			{
				return on_check_;
			}

			//Returns the on uncheck callback
			[[nodiscard]] inline auto OnUncheck() const noexcept
			{
				return on_uncheck_;
			}
	};

} //ion::gui::controls

#endif