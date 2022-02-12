/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui
File:	IonGuiPanelContainer.cpp
-------------------------------------------
*/

#include "IonGuiPanelContainer.h"

#include <algorithm>
#include <utility>

#include "IonGuiController.h"
#include "IonGuiFrame.h"
#include "IonGuiPanel.h"
#include "controls/IonGuiControl.h"
#include "skins/IonGuiTheme.h"

namespace ion::gui
{

namespace gui_panel_container::detail
{

} //gui_panel_container::detail


//Private

const skins::GuiTheme* GuiPanelContainer::GetTheme() const noexcept
{
	if (auto frame = ParentFrame(); frame)
	{
		if (auto theme = frame->ActiveTheme(); theme)
			return theme;
		else if (auto owner = frame->Owner(); owner)
			return owner->ActiveTheme();
	}

	return nullptr;
}

const skins::GuiSkin* GuiPanelContainer::GetSkin(std::string_view name) const noexcept
{
	if (auto theme = GetTheme(); theme)
		return theme->GetSkin(name).get();
	else
		return nullptr;
}


//Protected

/*
	Events
*/

void GuiPanelContainer::Created(GuiComponent &component) noexcept
{
	GuiContainer::Created(component); //Use base functionality

	ordered_components_.push_back(&component);

	if (auto control = dynamic_cast<controls::GuiControl*>(&component); control)
		Created(*control);
	else if (auto panel = dynamic_cast<GuiPanel*>(&component); panel)
		Created(*panel);
}

void GuiPanelContainer::Created(controls::GuiControl &control) noexcept
{
	controls_.push_back(&control);
}

void GuiPanelContainer::Created(GuiPanel &panel) noexcept
{
	panels_.push_back(&panel);
}


void GuiPanelContainer::Removed(GuiComponent &component) noexcept
{
	auto iter =
		std::find_if(std::begin(ordered_components_), std::end(ordered_components_),
			[&](auto &x) noexcept
			{
				return x == &component;
			});

	//Component found
	if (iter != std::end(ordered_components_))
		ordered_components_.erase(iter);

	if (auto control = dynamic_cast<controls::GuiControl*>(&component); control)
		Removed(*control);
	else if (auto panel = dynamic_cast<GuiPanel*>(&component); panel)
		Removed(*panel);

	GuiContainer::Removed(component); //Use base functionality
}

void GuiPanelContainer::Removed(controls::GuiControl &control) noexcept
{
	auto iter =
		std::find_if(std::begin(controls_), std::end(controls_),
			[&](auto &x) noexcept
			{
				return x == &control;
			});

	//Control found
	if (iter != std::end(controls_))
		controls_.erase(iter);
}

void GuiPanelContainer::Removed(GuiPanel &panel) noexcept
{
	auto iter =
		std::find_if(std::begin(panels_), std::end(panels_),
			[&](auto &x) noexcept
			{
				return x == &panel;
			});

	//Panel found
	if (iter != std::end(panels_))
		panels_.erase(iter);
}


void GuiPanelContainer::TabOrderChanged() noexcept
{
	//Optional to override
}


void GuiPanelContainer::Enabled() noexcept
{
	GuiComponent::Enabled(); //Use base functionality
}

void GuiPanelContainer::Disabled() noexcept
{
	if (auto frame = ParentFrame(); frame)
	{
		if (auto control = frame->FocusedControl();
			control && control->IsDescendantOf(*this))

			control->Reset();
	}

	GuiComponent::Disabled(); //Use base functionality
}


void GuiPanelContainer::Shown() noexcept
{
	GuiComponent::Shown(); //Use base functionality
}

void GuiPanelContainer::Hidden() noexcept
{
	if (auto frame = ParentFrame(); frame)
	{
		if (auto control = frame->FocusedControl();
			control && control->IsDescendantOf(*this))

			control->Reset();
	}

	GuiComponent::Hidden(); //Use base functionality
}


//Public

/*
	Observers
*/

bool GuiPanelContainer::IsFocusable() const noexcept
{
	if (auto frame = dynamic_cast<const GuiFrame*>(this); frame)
		return frame->IsFocusable();
	else if (owner_)
		return IsEnabled() && IsVisible() && static_cast<const GuiPanelContainer*>(owner_)->IsFocusable(); //Recursive
	else
		return IsEnabled() && IsVisible();
}


GuiFrame* GuiPanelContainer::ParentFrame() const noexcept
{
	if (auto frame = dynamic_cast<const GuiFrame*>(this); frame)
		return const_cast<GuiFrame*>(frame);
	else if (owner_)
		return static_cast<GuiPanelContainer*>(owner_)->ParentFrame(); //Recursive
	else
		return nullptr;
}


/*
	Tabulating
*/

void GuiPanelContainer::TabOrder(GuiComponent &component, int order) noexcept
{
	//Component found
	if (auto iter = std::find(std::begin(ordered_components_), std::end(ordered_components_), &component);
		iter != std::end(ordered_components_))
	{
		ordered_components_.erase(iter);
		ordered_components_.insert(std::begin(ordered_components_) +
			std::clamp(order, 0, std::ssize(ordered_components_)), &component);
		TabOrderChanged(); //Notify that tab order has been changed
	}
}

std::optional<int> GuiPanelContainer::TabOrder(const GuiComponent &component) const noexcept
{
	for (auto order = 0; auto &ordered_component : ordered_components_)
	{
		if (ordered_component == &component)
			return order;
		else
			++order;
	}

	return {};
}


/*
	Buttons
	Creating
*/

NonOwningPtr<controls::GuiButton> GuiPanelContainer::CreateButton(std::string name, const std::optional<Vector2> &size,
	std::optional<std::string> caption, std::optional<std::string> tooltip, controls::gui_control::BoundingBoxes hit_boxes)
{
	return CreateControl<controls::GuiButton>(std::move(name), size, std::move(caption), std::move(tooltip), std::move(hit_boxes));
}

NonOwningPtr<controls::GuiButton> GuiPanelContainer::CreateButton(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
	std::optional<std::string> caption, std::optional<std::string> tooltip, controls::gui_control::BoundingBoxes hit_boxes)
{
	return CreateControl<controls::GuiButton>(std::move(name), skin, size, std::move(caption), std::move(tooltip), std::move(hit_boxes));
}

NonOwningPtr<controls::GuiButton> GuiPanelContainer::CreateButton(controls::GuiButton &&button)
{
	return CreateControl<controls::GuiButton>(std::move(button));
}


/*
	Check boxes
	Creating
*/

NonOwningPtr<controls::GuiCheckBox> GuiPanelContainer::CreateCheckBox(std::string name, const std::optional<Vector2> &size,
	std::optional<std::string> caption, std::optional<std::string> tooltip, controls::gui_control::BoundingBoxes hit_boxes)
{
	return CreateControl<controls::GuiCheckBox>(std::move(name), size, std::move(caption), std::move(tooltip), std::move(hit_boxes));
}

NonOwningPtr<controls::GuiCheckBox> GuiPanelContainer::CreateCheckBox(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
	std::optional<std::string> caption, std::optional<std::string> tooltip, controls::gui_control::BoundingBoxes hit_boxes)
{
	return CreateControl<controls::GuiCheckBox>(std::move(name), skin, size, std::move(caption), std::move(tooltip), std::move(hit_boxes));
}

NonOwningPtr<controls::GuiCheckBox> GuiPanelContainer::CreateCheckBox(controls::GuiCheckBox &&check_box)
{
	return CreateControl<controls::GuiCheckBox>(std::move(check_box));
}


/*
	Group boxes
	Creating
*/

NonOwningPtr<controls::GuiGroupBox> GuiPanelContainer::CreateGroupBox(std::string name, const std::optional<Vector2> &size,
	std::optional<std::string> caption, controls::gui_control::BoundingBoxes hit_boxes)
{
	return CreateControl<controls::GuiGroupBox>(std::move(name), size, std::move(caption), std::move(hit_boxes));
}

NonOwningPtr<controls::GuiGroupBox> GuiPanelContainer::CreateGroupBox(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
	std::optional<std::string> caption, controls::gui_control::BoundingBoxes hit_boxes)
{
	return CreateControl<controls::GuiGroupBox>(std::move(name), skin, size, std::move(caption), std::move(hit_boxes));
}

NonOwningPtr<controls::GuiGroupBox> GuiPanelContainer::CreateGroupBox(controls::GuiGroupBox &&group_box)
{
	return CreateControl<controls::GuiGroupBox>(std::move(group_box));
}


/*
	Labels
	Creating
*/

NonOwningPtr<controls::GuiLabel> GuiPanelContainer::CreateLabel(std::string name, const std::optional<Vector2> &size,
	std::optional<std::string> caption, controls::gui_control::BoundingBoxes hit_boxes)
{
	return CreateControl<controls::GuiLabel>(std::move(name), size, std::move(caption), std::move(hit_boxes));
}

NonOwningPtr<controls::GuiLabel> GuiPanelContainer::CreateLabel(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
	std::optional<std::string> caption, controls::gui_control::BoundingBoxes hit_boxes)
{
	return CreateControl<controls::GuiLabel>(std::move(name), skin, size, std::move(caption), std::move(hit_boxes));
}

NonOwningPtr<controls::GuiLabel> GuiPanelContainer::CreateLabel(controls::GuiLabel &&label)
{
	return CreateControl<controls::GuiLabel>(std::move(label));
}


/*
	List boxes
	Creating
*/

NonOwningPtr<controls::GuiListBox> GuiPanelContainer::CreateListBox(std::string name, const std::optional<Vector2> &size,
	std::optional<std::string> caption, controls::gui_control::BoundingBoxes hit_boxes)
{
	return CreateControl<controls::GuiListBox>(std::move(name), size, std::move(caption), std::move(hit_boxes));
}

NonOwningPtr<controls::GuiListBox> GuiPanelContainer::CreateListBox(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
	std::optional<std::string> caption, controls::gui_control::BoundingBoxes hit_boxes)
{
	return CreateControl<controls::GuiListBox>(std::move(name), skin, size, std::move(caption), std::move(hit_boxes));
}

NonOwningPtr<controls::GuiListBox> GuiPanelContainer::CreateListBox(controls::GuiListBox &&list_box)
{
	return CreateControl<controls::GuiListBox>(std::move(list_box));
}


/*
	Progress bars
	Creating
*/

NonOwningPtr<controls::GuiProgressBar> GuiPanelContainer::CreateProgressBar(std::string name, const std::optional<Vector2> &size,
	std::optional<std::string> caption, controls::gui_progress_bar::ProgressBarType type)
{
	return CreateControl<controls::GuiProgressBar>(std::move(name), size, std::move(caption), type);
}

NonOwningPtr<controls::GuiProgressBar> GuiPanelContainer::CreateProgressBar(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
	std::optional<std::string> caption, controls::gui_progress_bar::ProgressBarType type)
{
	return CreateControl<controls::GuiProgressBar>(std::move(name), skin, size, std::move(caption), type);
}

NonOwningPtr<controls::GuiProgressBar> GuiPanelContainer::CreateProgressBar(controls::GuiProgressBar &&progress_bar)
{
	return CreateControl<controls::GuiProgressBar>(std::move(progress_bar));
}


/*
	Radio buttons
	Creating
*/

NonOwningPtr<controls::GuiRadioButton> GuiPanelContainer::CreateRadioButton(std::string name, const std::optional<Vector2> &size,
	std::optional<std::string> caption, std::optional<std::string> tooltip, controls::gui_control::BoundingBoxes hit_boxes)
{
	return CreateControl<controls::GuiRadioButton>(std::move(name), size, std::move(caption), std::move(tooltip), std::move(hit_boxes));
}

NonOwningPtr<controls::GuiRadioButton> GuiPanelContainer::CreateRadioButton(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
	std::optional<std::string> caption, std::optional<std::string> tooltip, controls::gui_control::BoundingBoxes hit_boxes)
{
	return CreateControl<controls::GuiRadioButton>(std::move(name), skin, size, std::move(caption), std::move(tooltip), std::move(hit_boxes));
}

NonOwningPtr<controls::GuiRadioButton> GuiPanelContainer::CreateRadioButton(controls::GuiRadioButton &&radio_button)
{
	return CreateControl<controls::GuiRadioButton>(std::move(radio_button));
}


/*
	Scroll bars
	Creating
*/

NonOwningPtr<controls::GuiScrollBar> GuiPanelContainer::CreateScrollBar(std::string name, const std::optional<Vector2> &size,
	std::optional<std::string> caption, controls::gui_slider::SliderType type, controls::gui_control::BoundingBoxes hit_boxes)
{
	return CreateControl<controls::GuiScrollBar>(std::move(name), size, std::move(caption), type, std::move(hit_boxes));
}

NonOwningPtr<controls::GuiScrollBar> GuiPanelContainer::CreateScrollBar(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
	std::optional<std::string> caption, controls::gui_slider::SliderType type, controls::gui_control::BoundingBoxes hit_boxes)
{
	return CreateControl<controls::GuiScrollBar>(std::move(name), skin, size, std::move(caption), type, std::move(hit_boxes));
}

NonOwningPtr<controls::GuiScrollBar> GuiPanelContainer::CreateScrollBar(controls::GuiScrollBar &&scroll_bar)
{
	return CreateControl<controls::GuiScrollBar>(std::move(scroll_bar));
}


/*
	Sliders
	Creating
*/

NonOwningPtr<controls::GuiSlider> GuiPanelContainer::CreateSlider(std::string name, const std::optional<Vector2> &size,
	std::optional<std::string> caption, std::optional<std::string> tooltip, controls::gui_slider::SliderType type,
	controls::gui_control::BoundingBoxes hit_boxes)
{
	return CreateControl<controls::GuiSlider>(std::move(name), size, std::move(caption), std::move(tooltip), type, std::move(hit_boxes));
}

NonOwningPtr<controls::GuiSlider> GuiPanelContainer::CreateSlider(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
	std::optional<std::string> caption, std::optional<std::string> tooltip, controls::gui_slider::SliderType type,
	controls::gui_control::BoundingBoxes hit_boxes)
{
	return CreateControl<controls::GuiSlider>(std::move(name), skin, size, std::move(caption), std::move(tooltip), type, std::move(hit_boxes));
}

NonOwningPtr<controls::GuiSlider> GuiPanelContainer::CreateSlider(controls::GuiSlider &&slider)
{
	return CreateControl<controls::GuiSlider>(std::move(slider));
}


/*
	Text boxes
	Creating
*/

NonOwningPtr<controls::GuiTextBox> GuiPanelContainer::CreateTextBox(std::string name, const std::optional<Vector2> &size,
	std::optional<std::string> caption, controls::gui_control::BoundingBoxes hit_boxes)
{
	return CreateControl<controls::GuiTextBox>(std::move(name), size, std::move(caption), std::move(hit_boxes));
}

NonOwningPtr<controls::GuiTextBox> GuiPanelContainer::CreateTextBox(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
	std::optional<std::string> caption, controls::gui_control::BoundingBoxes hit_boxes)
{
	return CreateControl<controls::GuiTextBox>(std::move(name), skin, size, std::move(caption), std::move(hit_boxes));
}

NonOwningPtr<controls::GuiTextBox> GuiPanelContainer::CreateTextBox(controls::GuiTextBox &&text_box)
{
	return CreateControl<controls::GuiTextBox>(std::move(text_box));
}


/*
	Controls
	Retrieving
*/

NonOwningPtr<controls::GuiControl> GuiPanelContainer::GetControl(std::string_view name) noexcept
{
	return dynamic_pointer_cast<controls::GuiControl>(GetComponent(name));
}

NonOwningPtr<const controls::GuiControl> GuiPanelContainer::GetControl(std::string_view name) const noexcept
{
	return dynamic_pointer_cast<const controls::GuiControl>(GetComponent(name));
}


NonOwningPtr<controls::GuiControl> GuiPanelContainer::SearchControl(std::string_view name) noexcept
{
	if (auto control = GetControl(name); control)
		return control;

	for (auto &panel : panels_)
	{
		if (auto c = panel->SearchControl(name); c) //Recursive
			return c;
	}

	return nullptr;
}

NonOwningPtr<const controls::GuiControl> GuiPanelContainer::SearchControl(std::string_view name) const noexcept
{
	if (auto control = GetControl(name); control)
		return control;

	for (auto &panel : panels_)
	{
		if (auto c = panel->SearchControl(name); c) //Recursive
			return c;
	}

	return nullptr;
}


/*
	Controls
	Removing
*/

void GuiPanelContainer::ClearControls() noexcept
{
	auto controls = std::move(controls_);

	for (auto &control : controls)
	{
		if (RemoveControl(*control))
			control = nullptr;
	}

	controls.erase(
		std::remove(std::begin(controls), std::end(controls), nullptr),
		std::end(controls));

	controls_ = std::move(controls);
	controls_.shrink_to_fit();
}

bool GuiPanelContainer::RemoveControl(controls::GuiControl &control) noexcept
{
	return RemoveComponent(control);
}

bool GuiPanelContainer::RemoveControl(std::string_view name) noexcept
{
	return RemoveComponent(name);
}


/*
	Panels
	Creating
*/

NonOwningPtr<GuiPanel> GuiPanelContainer::CreatePanel(std::string name)
{
	return CreateComponent<GuiPanel>(std::move(name));
}

NonOwningPtr<GuiPanel> GuiPanelContainer::CreatePanel(GuiPanel &&panel)
{
	return CreateComponent<GuiPanel>(std::move(panel));
}


/*
	Panels
	Retrieving
*/

NonOwningPtr<GuiPanel> GuiPanelContainer::GetPanel(std::string_view name) noexcept
{
	return dynamic_pointer_cast<GuiPanel>(GetComponent(name));
}

NonOwningPtr<const GuiPanel> GuiPanelContainer::GetPanel(std::string_view name) const noexcept
{
	return dynamic_pointer_cast<const GuiPanel>(GetComponent(name));
}


NonOwningPtr<GuiPanel> GuiPanelContainer::SearchPanel(std::string_view name) noexcept
{
	if (auto panel = GetPanel(name); panel)
		return panel;

	for (auto &panel : panels_)
	{
		if (auto p = panel->SearchPanel(name); p) //Recursive
			return p;
	}

	return nullptr;
}

NonOwningPtr<const GuiPanel> GuiPanelContainer::SearchPanel(std::string_view name) const noexcept
{
	if (auto panel = GetPanel(name); panel)
		return panel;

	for (auto &panel : panels_)
	{
		if (auto p = panel->SearchPanel(name); p) //Recursive
			return p;
	}

	return nullptr;
}


/*
	Panels
	Removing
*/

void GuiPanelContainer::ClearPanels() noexcept
{
	auto panels = std::move(panels_);

	for (auto &panel : panels)
	{
		if (RemovePanel(*panel))
			panel = nullptr;
	}

	panels.erase(
		std::remove(std::begin(panels), std::end(panels), nullptr),
		std::end(panels));

	panels_ = std::move(panels);
	panels_.shrink_to_fit();
}

bool GuiPanelContainer::RemovePanel(GuiPanel &panel) noexcept
{
	return RemoveComponent(panel);
}

bool GuiPanelContainer::RemovePanel(std::string_view name) noexcept
{
	return RemoveComponent(name);
}


/*
	Components
	Removing (optimization)
*/

void GuiPanelContainer::ClearComponents() noexcept
{
	controls_.clear();
	panels_.clear();
	ordered_components_.clear();
	GuiContainer::ClearComponents();
		//This will go much faster because controls and panels are pre-cleared
	
	//Non-removable components will still be present
	//Add them back to the controls/panels containers
	for (auto &component : Components())
		Created(component);

	controls_.shrink_to_fit();
	panels_.shrink_to_fit();
	ordered_components_.shrink_to_fit();
}

} //ion::gui