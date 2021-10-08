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

#include "IonGuiPanel.h"
#include "controls/IonGuiControl.h"

namespace ion::gui
{

namespace gui_panel_container::detail
{

} //gui_panel_container::detail


//Protected

/*
	Events
*/

void GuiPanelContainer::Created(GuiComponent &component) noexcept
{
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
	{
		ordered_components_.erase(iter);

		if (auto control = dynamic_cast<controls::GuiControl*>(&component); control)
			Removed(*control);
		else if (auto panel = dynamic_cast<GuiPanel*>(&component); panel)
			Removed(*panel);
	}
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


//Public

/*
	Tabulation
*/

void GuiPanelContainer::TabOrder(GuiComponent &component, int order) noexcept
{
	auto iter =
		std::find(std::begin(ordered_components_), std::end(ordered_components_), &component);

	//Component found
	if (iter != std::end(ordered_components_))
	{
		ordered_components_.erase(iter);
		ordered_components_.insert(std::begin(ordered_components_) +
			std::clamp(order, 0, std::ssize(ordered_components_)), &component);
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

	return std::nullopt;
}


/*
	Controls
	Retrieving
*/

NonOwningPtr<controls::GuiControl> GuiPanelContainer::GetControl(std::string_view name) noexcept
{
	return static_pointer_cast<controls::GuiControl>(GetComponent(name));
}

NonOwningPtr<const controls::GuiControl> GuiPanelContainer::GetControl(std::string_view name) const noexcept
{
	return static_pointer_cast<const controls::GuiControl>(GetComponent(name));
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
	return static_pointer_cast<GuiPanel>(GetComponent(name));
}

NonOwningPtr<const GuiPanel> GuiPanelContainer::GetPanel(std::string_view name) const noexcept
{
	return static_pointer_cast<const GuiPanel>(GetComponent(name));
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