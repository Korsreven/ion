/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui
File:	IonGuiPanelContainer.h
-------------------------------------------
*/

#ifndef ION_GUI_PANEL_CONTAINER_H
#define ION_GUI_PANEL_CONTAINER_H

#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "IonGuiContainer.h"
#include "adaptors/ranges/IonDereferenceIterable.h"
#include "controls/IonGuiButton.h"
#include "controls/IonGuiCheckBox.h"
#include "controls/IonGuiGroupBox.h"
#include "controls/IonGuiLabel.h"
#include "controls/IonGuiRadioButton.h"
#include "controls/IonGuiScrollBar.h"
#include "controls/IonGuiSlider.h"
#include "graphics/utilities/IonVector2.h"
#include "memory/IonNonOwningPtr.h"

namespace ion::gui
{
	using graphics::utilities::Vector2;

	//Forward declarations
	class GuiFrame;
	class GuiPanel;

	namespace controls
	{
		class GuiControl; //Forward declaration
	}

	namespace gui_panel_container
	{
		namespace detail
		{
			using control_pointers = std::vector<controls::GuiControl*>;
			using panel_pointers = std::vector<GuiPanel*>;
			using component_pointers = std::vector<GuiComponent*>;
		} //detail
	} //gui_panel_container


	class GuiPanelContainer : public GuiContainer
	{
		private:

			gui_panel_container::detail::control_pointers controls_;
			gui_panel_container::detail::panel_pointers panels_;
			gui_panel_container::detail::component_pointers ordered_components_;

		protected:

			/*
				Events
			*/

			//See ObjectManager::Created for more details
			virtual void Created(GuiComponent &component) noexcept override;
			virtual void Created(controls::GuiControl &control) noexcept;
			virtual void Created(GuiPanel &panel) noexcept;

			//See ObjectManager::Removed for more details
			virtual void Removed(GuiComponent &component) noexcept override;
			virtual void Removed(controls::GuiControl &control) noexcept;
			virtual void Removed(GuiPanel &panel) noexcept;

			//Called right after the tab order has been changed
			virtual void TabOrderChanged() noexcept;


			//See GuiComponent::Enabled for more details
			virtual void Enabled() noexcept override;

			//See GuiComponent::Disabled for more details
			virtual void Disabled() noexcept override;


			//See GuiComponent::Shown for more details
			virtual void Shown() noexcept override;

			//See GuiComponent::Hidden for more details
			virtual void Hidden() noexcept override;

		public:

			using GuiContainer::GuiContainer;


			/*
				Ranges
			*/

			//Returns a mutable range of all controls in this container
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Controls() noexcept
			{
				return adaptors::ranges::DereferenceIterable<gui_panel_container::detail::control_pointers&>{controls_};
			}

			//Returns an immutable range of all controls in this container
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Controls() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const gui_panel_container::detail::control_pointers&>{controls_};
			}


			//Returns a mutable range of all panels in this container
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Panels() noexcept
			{
				return adaptors::ranges::DereferenceIterable<gui_panel_container::detail::panel_pointers&>{panels_};
			}

			//Returns an immutable range of all panels in this container
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Panels() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const gui_panel_container::detail::panel_pointers&>{panels_};
			}


			//Returns a mutable range of all components ordered for tabulating
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto OrderedComponents() noexcept
			{
				return adaptors::ranges::DereferenceIterable<gui_panel_container::detail::component_pointers&>{ordered_components_};
			}

			//Returns an immutable range of all components ordered for tabulating
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto OrderedComponents() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const gui_panel_container::detail::component_pointers&>{ordered_components_};
			}


			/*
				Modifiers
			*/




			/*
				Observers
			*/

			//Returns true if this container is focusable
			[[nodiscard]] bool IsFocusable() const noexcept;


			//Returns a pointer to the parent frame of this container
			[[nodiscard]] GuiFrame* ParentFrame() const noexcept;


			/*
				Tabulating
			*/

			//Sets the tab order of the given component to the given order
			void TabOrder(GuiComponent &component, int order) noexcept;

			//Returns the tab order of the given component
			//Returns nullopt if the given component could not be found
			[[nodiscard]] std::optional<int> TabOrder(const GuiComponent &component) const noexcept;


			/*
				Controls
				Creating
			*/

			//Create an control of type T with the given name and arguments
			template <typename T, typename... Args>
			auto CreateControl(std::string name, Args &&...args)
			{
				static_assert(std::is_base_of_v<controls::GuiControl, T>);

				auto ptr = CreateComponent<T>(std::move(name), std::forward<Args>(args)...);
				return static_pointer_cast<T>(ptr);
			}


			//Create an control of type T as a copy of the given control
			template <typename T>
			auto CreateControl(const T &control_t)
			{
				static_assert(std::is_base_of_v<controls::GuiControl, T>);

				auto ptr = CreateComponent(control_t);
				return static_pointer_cast<T>(ptr);
			}

			//Create an control of type T by moving the given control
			template <typename T>
			auto CreateControl(T &&control_t)
			{
				static_assert(std::is_base_of_v<controls::GuiControl, T>);

				auto ptr = CreateComponent(std::move(control_t));
				return static_pointer_cast<T>(ptr);
			}


			/*
				Buttons
				Creating
			*/

			//Create a button with the given name, caption, tooltip, skin and hit boxes
			NonOwningPtr<controls::GuiButton> CreateButton(std::string name, std::optional<std::string> caption,
				std::optional<std::string> tooltip, controls::gui_button::ButtonSkin skin,
				controls::gui_control::BoundingBoxes hit_boxes = {});

			//Create a button with the given name, caption, tooltip, skin, size and hit boxes
			NonOwningPtr<controls::GuiButton> CreateButton(std::string name, std::optional<std::string> caption,
				std::optional<std::string> tooltip, controls::gui_button::ButtonSkin skin, const Vector2 &size,
				controls::gui_control::BoundingBoxes hit_boxes = {});

			//Create a button by moving the given button
			NonOwningPtr<controls::GuiButton> CreateButton(controls::GuiButton &&button);


			/*
				Check boxes
				Creating
			*/

			//Create a check box with the given name, caption, tooltip, skin and hit boxes
			NonOwningPtr<controls::GuiCheckBox> CreateCheckBox(std::string name, std::optional<std::string> caption,
				std::optional<std::string> tooltip, controls::gui_check_box::CheckBoxSkin skin,
				controls::gui_control::BoundingBoxes hit_boxes = {});

			//Create a check box with the given name, caption, tooltip, skin, size and hit boxes
			NonOwningPtr<controls::GuiCheckBox> CreateCheckBox(std::string name, std::optional<std::string> caption,
				std::optional<std::string> tooltip, controls::gui_check_box::CheckBoxSkin skin, const Vector2 &size,
				controls::gui_control::BoundingBoxes hit_boxes = {});

			//Create a check box by moving the given check box
			NonOwningPtr<controls::GuiCheckBox> CreateCheckBox(controls::GuiCheckBox &&check_box);


			/*
				Group boxes
				Creating
			*/

			//Create a group box with the given name, caption, skin and hit boxes
			NonOwningPtr<controls::GuiGroupBox> CreateGroupBox(std::string name, std::optional<std::string> caption,
				controls::gui_group_box::GroupBoxSkin skin, controls::gui_control::BoundingBoxes hit_boxes = {});

			//Create a group box with the given name, caption, skin, size and hit boxes
			NonOwningPtr<controls::GuiGroupBox> CreateGroupBox(std::string name, std::optional<std::string> caption,
				controls::gui_group_box::GroupBoxSkin skin, const Vector2 &size, controls::gui_control::BoundingBoxes hit_boxes = {});

			//Create a group box by moving the given group box
			NonOwningPtr<controls::GuiGroupBox> CreateGroupBox(controls::GuiGroupBox &&group_box);


			/*
				Labels
				Creating
			*/

			//Create a label with the given name, caption, skin and hit boxes
			NonOwningPtr<controls::GuiLabel> CreateLabel(std::string name, std::optional<std::string> caption,
				controls::gui_label::LabelSkin skin, controls::gui_control::BoundingBoxes hit_boxes = {});

			//Create a label with the given name, caption, skin, size and hit boxes
			NonOwningPtr<controls::GuiLabel> CreateLabel(std::string name, std::optional<std::string> caption,
				controls::gui_label::LabelSkin skin, const Vector2 &size, controls::gui_control::BoundingBoxes hit_boxes = {});

			//Create a label by moving the given label
			NonOwningPtr<controls::GuiLabel> CreateLabel(controls::GuiLabel &&label);


			/*
				Radio buttons
				Creating
			*/

			//Create a radio button with the given name, caption, tooltip, skin and hit boxes
			NonOwningPtr<controls::GuiRadioButton> CreateRadioButton(std::string name, std::optional<std::string> caption,
				std::optional<std::string> tooltip, controls::gui_radio_button::RadioButtonSkin skin,
				controls::gui_control::BoundingBoxes hit_boxes = {});

			//Create a radio button with the given name, caption, tooltip, skin, size and hit boxes
			NonOwningPtr<controls::GuiRadioButton> CreateRadioButton(std::string name, std::optional<std::string> caption,
				std::optional<std::string> tooltip, controls::gui_radio_button::RadioButtonSkin skin, const Vector2 &size,
				controls::gui_control::BoundingBoxes hit_boxes = {});

			//Create a radio button by moving the given radio button
			NonOwningPtr<controls::GuiRadioButton> CreateRadioButton(controls::GuiRadioButton &&radio_button);


			/*
				Scroll bars
				Creating
			*/

			//Create a scroll bar with the given name, caption, tooltip, skin, type and hit boxes
			NonOwningPtr<controls::GuiScrollBar> CreateScrollBar(std::string name, std::optional<std::string> caption,
				std::optional<std::string> tooltip, controls::gui_scroll_bar::ScrollBarSkin skin,
				controls::gui_slider::SliderType type = controls::gui_slider::SliderType::Vertical,
				controls::gui_control::BoundingBoxes hit_boxes = {});

			//Create a scroll bar with the given name, caption, tooltip, skin, size, type and hit boxes
			NonOwningPtr<controls::GuiScrollBar> CreateScrollBar(std::string name, std::optional<std::string> caption,
				std::optional<std::string> tooltip, controls::gui_scroll_bar::ScrollBarSkin skin, const Vector2 &size,
				controls::gui_slider::SliderType type = controls::gui_slider::SliderType::Vertical,
				controls::gui_control::BoundingBoxes hit_boxes = {});

			//Create a scroll bar by moving the given scroll bar
			NonOwningPtr<controls::GuiScrollBar> CreateScrollBar(controls::GuiScrollBar &&scroll_bar);


			/*
				Sliders
				Creating
			*/

			//Create a slider with the given name, caption, tooltip, skin, type and hit boxes
			NonOwningPtr<controls::GuiSlider> CreateSlider(std::string name, std::optional<std::string> caption,
				std::optional<std::string> tooltip, controls::gui_slider::SliderSkin skin,
				controls::gui_slider::SliderType type = controls::gui_slider::SliderType::Horizontal,
				controls::gui_control::BoundingBoxes hit_boxes = {});

			//Create a slider with the given name, caption, tooltip, skin, size, type and hit boxes
			NonOwningPtr<controls::GuiSlider> CreateSlider(std::string name, std::optional<std::string> caption,
				std::optional<std::string> tooltip, controls::gui_slider::SliderSkin skin, const Vector2 &size,
				controls::gui_slider::SliderType type = controls::gui_slider::SliderType::Horizontal,
				controls::gui_control::BoundingBoxes hit_boxes = {});

			//Create a slider by moving the given slider
			NonOwningPtr<controls::GuiSlider> CreateSlider(controls::GuiSlider &&slider);


			/*
				Controls
				Retrieving
			*/

			//Gets a pointer to a mutable control with the given name
			//Returns nullptr if control could not be found
			[[nodiscard]] NonOwningPtr<controls::GuiControl> GetControl(std::string_view name) noexcept;

			//Gets a pointer to an immutable control with the given name
			//Returns nullptr if control could not be found
			[[nodiscard]] NonOwningPtr<const controls::GuiControl> GetControl(std::string_view name) const noexcept;


			//Searches for a pointer to a mutable control (all child controls) with the given name
			//Returns nullptr if control could not be found
			[[nodiscard]] NonOwningPtr<controls::GuiControl> SearchControl(std::string_view name) noexcept;

			//Searches for a pointer to an immutable control (all child controls) with the given name
			//Returns nullptr if control could not be found
			[[nodiscard]] NonOwningPtr<const controls::GuiControl> SearchControl(std::string_view name) const noexcept;


			//Gets a pointer to a mutable control of type T with the given name
			//Returns nullptr if a control of type T could not be found
			template <typename T>
			[[nodiscard]] auto GetControlAs(std::string_view name) noexcept
			{
				static_assert(std::is_base_of_v<controls::GuiControl, T>);
				return GetComponentAs<T>(name);
			}

			//Gets a pointer to an immutable control of type T with the given name
			//Returns nullptr if a control of type T could not be found
			template <typename T>
			[[nodiscard]] auto GetControlAs(std::string_view name) const noexcept
			{
				static_assert(std::is_base_of_v<controls::GuiControl, T>);
				return GetComponentAs<T>(name);
			}


			/*
				Controls
				Removing
			*/

			//Clear all removable controls from this container
			void ClearControls() noexcept;

			//Remove a removable control from this container
			bool RemoveControl(controls::GuiControl &control) noexcept;

			//Remove a removable control with the given name from this container
			bool RemoveControl(std::string_view name) noexcept;


			/*
				Panels
				Creating
			*/

			//Create a panel with the given name
			NonOwningPtr<GuiPanel> CreatePanel(std::string name);

			//Create a panel by moving the given panel
			NonOwningPtr<GuiPanel> CreatePanel(GuiPanel &&panel);


			/*
				Panels
				Retrieving
			*/

			//Gets a pointer to a mutable panel with the given name
			//Returns nullptr if panel could not be found
			[[nodiscard]] NonOwningPtr<GuiPanel> GetPanel(std::string_view name) noexcept;

			//Gets a pointer to an immutable panel with the given name
			//Returns nullptr if panel could not be found
			[[nodiscard]] NonOwningPtr<const GuiPanel> GetPanel(std::string_view name) const noexcept;


			//Searches for a pointer to a mutable panel (all child panels) with the given name
			//Returns nullptr if panel could not be found
			[[nodiscard]] NonOwningPtr<GuiPanel> SearchPanel(std::string_view name) noexcept;

			//Searches for a pointer to an immutable panel (all child panels) with the given name
			//Returns nullptr if panel could not be found
			[[nodiscard]] NonOwningPtr<const GuiPanel> SearchPanel(std::string_view name) const noexcept;


			/*
				Panels
				Removing
			*/

			//Clear all removable panels from this container
			void ClearPanels() noexcept;

			//Remove a removable panel from this container
			bool RemovePanel(GuiPanel &panel) noexcept;

			//Remove a removable panel with the given name from this container
			bool RemovePanel(std::string_view name) noexcept;


			/*
				Components
				Removing (optimization)
			*/

			//Clear all removable components from this container
			void ClearComponents() noexcept;
	};
} //ion::gui

#endif