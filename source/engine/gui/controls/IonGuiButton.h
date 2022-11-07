/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiButton.h
-------------------------------------------
*/

#ifndef ION_GUI_BUTTON_H
#define ION_GUI_BUTTON_H

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "IonGuiControl.h"
#include "adaptors/ranges/IonIterable.h"
#include "memory/IonOwningPtr.h"

namespace ion::gui::controls
{
	namespace gui_button
	{
		enum class ButtonActionType
		{
			//Controller actions
			ShowGui,
			HideGui,
			EnableGui,
			DisableGui,

			//Frame actions
			ShowFrame,
			ShowFrameModal,
			HideFrame,
			EnableFrame,
			DisableFrame,
			FocusFrame,
			DefocusFrame,

			//Panel actions
			ShowPanel,
			HidePanel,
			EnablePanel,
			DisablePanel,

			//Control actions
			ShowControl,
			HideControl,
			EnableControl,
			DisableControl,
			FocusControl,
			DefocusControl
		};

		using ButtonAction = std::pair<ButtonActionType, std::string>;
		using ButtonActions = std::vector<ButtonAction>;


		struct ButtonSkin : gui_control::ControlSkin
		{
			//Empty
		};


		namespace detail
		{
		} //detail
	} //gui_button


	///@brief A class representing a GUI button
	///@details A button can have predefined actions executed when clicked as well as custom callbacks
	class GuiButton : public GuiControl
	{
		private:

			void ExecuteActions() noexcept;

		protected:

			gui_button::ButtonActions actions_;


			/**
				@name Events
				@{
			*/

			///@brief See GuiControl::Clicked for more details
			virtual void Clicked() noexcept override;

			///@}

			/**
				@name Skins
				@{
			*/

			virtual OwningPtr<gui_control::ControlSkin> AttuneSkin(OwningPtr<gui_control::ControlSkin> skin) const override;

			///@}

		public:

			///@brief Constructs a button with the given name, size, caption, tooltip and hit boxes
			GuiButton(std::string name, const std::optional<Vector2> &size,
				std::optional<std::string> caption, std::optional<std::string> tooltip, gui_control::BoundingBoxes hit_boxes = {}) noexcept;

			///@brief Constructs a button with the given name, skin, size, caption, tooltip and hit boxes
			GuiButton(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
				std::optional<std::string> caption, std::optional<std::string> tooltip, gui_control::BoundingBoxes hit_boxes = {});


			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all actions that are executed when clicking on this button
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Actions() noexcept
			{
				return adaptors::ranges::Iterable<gui_button::ButtonActions&>{actions_};
			}

			///@brief Returns an immutable range of all actions that are executed when clicking on this button
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Actions() const noexcept
			{
				return adaptors::ranges::Iterable<const gui_button::ButtonActions&>{actions_};
			}

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Adds the given action to this button
			void AddAction(gui_button::ButtonAction action);

			///@brief Adds the given actions to this button
			void AddActions(gui_button::ButtonActions actions);


			///@brief Clears all of the added actions on this button
			void ClearActions() noexcept;

			///@}
	};

} //ion::gui::controls

#endif