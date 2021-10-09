/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiControl.h
-------------------------------------------
*/

#ifndef ION_GUI_CONTROL_H
#define ION_GUI_CONTROL_H

#include <optional>
#include <string>

#include "gui/IonGuiComponent.h"

namespace ion::gui
{
	class GuiPanelContainer; //Forward declaration
}

namespace ion::gui::controls
{
	namespace gui_control::detail
	{
	} //gui_control::detail


	class GuiControl : public GuiComponent
	{
		protected:

			bool focused_ = false;
			bool focusable_ = true;


			/*
				Events
			*/

			//See GuiComponent::Enabled for more details
			virtual void Enabled() noexcept override;

			//See GuiComponent::Disabled for more details
			virtual void Disabled() noexcept override;


			//Called right after a control has been focused
			virtual void Focused() noexcept;

			//Called right after a control has been defocused
			virtual void Defocused() noexcept;

		public:

			//Construct a control with the given name
			GuiControl(std::string name);


			/*
				Modifiers
			*/

			//Sets whether or not this control is enabled
			inline void Enabled(bool enabled) noexcept
			{
				return GuiComponent::Enabled(enabled);
			}

			//Sets whether or not this control is focused
			inline void Focused(bool focused) noexcept
			{
				if (focused_ != focused &&
					enabled_ && focusable_)
				{
					if ((focused_ = focused) == true) //Suppress W4706
						Focused();
					else
						Defocused();
				}
			}

			//Sets whether or not this control is focusable
			inline void Focusable(bool focusable) noexcept
			{
				if (focusable_ != focusable)
				{
					if (!(focusable_ = focusable) && focused_)
					{
						focused_ = false;
						Defocused();
					}
				}
			}


			/*
				Observers
			*/

			//Returns true if this control is focused
			[[nodiscard]] inline auto IsFocused() const noexcept
			{
				return focused_;
			}

			//Returns true if this control is focusable
			[[nodiscard]] inline auto IsFocusable() const noexcept
			{
				return focusable_;
			}


			//Returns a pointer to the owner of this control
			[[nodiscard]] GuiPanelContainer* Owner() const noexcept;


			/*
				Tabulating
			*/

			//Sets the tab order of this control to the given order
			void TabOrder(int order) noexcept;

			//Returns the tab order of this control
			//Returns nullopt if this control has no owner
			[[nodiscard]] std::optional<int> TabOrder() const noexcept;
	};
} //ion::gui::controls

#endif