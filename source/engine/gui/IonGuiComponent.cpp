/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui
File:	IonGuiComponent.cpp
-------------------------------------------
*/

#include "IonGuiComponent.h"

#include "IonGuiContainer.h"
#include "graphics/scene/graph/IonSceneNode.h"

namespace ion::gui
{

using namespace gui_component;

namespace gui_component::detail
{

bool is_descendant_of(const GuiContainer &owner, const GuiComponent &component) noexcept
{
	if (auto component_owner = component.Owner(); component_owner == &owner)
		return true;
	else if (component_owner)
		return is_descendant_of(owner, *component_owner); //Recursive
	else
		return false;
}

} //gui_component::detail


//Private

/*
	Helper functions
*/

void GuiComponent::Detach() noexcept
{
	parent_ = nullptr;

	if (node_)
		node_->ParentNode()->RemoveChildNode(*node_);
}


//Protected

/*
	Events
*/

void GuiComponent::Created() noexcept
{
	//Optional to override
}

void GuiComponent::Removed() noexcept
{
	//Optional to override
}


void GuiComponent::Enabled() noexcept
{
	//User callback
	if (on_enable_)
		(*on_enable_)(*this);
}

void GuiComponent::Disabled() noexcept
{
	//User callback
	if (on_disable_)
		(*on_disable_)(*this);
}


void GuiComponent::Shown() noexcept
{
	//User callback
	if (on_show_)
		(*on_show_)(*this);
}

void GuiComponent::Hidden() noexcept
{
	//User callback
	if (on_hide_)
		(*on_hide_)(*this);
}


//Public

GuiComponent::GuiComponent(const GuiComponent &rhs) :

	managed::ManagedObject<GuiContainer>{rhs},
	parent_{nullptr} //A copy constructed component has no parent
{
	//Empty
}

GuiComponent::~GuiComponent() noexcept
{
	Detach();
}


/*
	Modifiers
*/

void GuiComponent::Enable() noexcept
{
	if (!enabled_)
	{
		enabled_ = true;
		Enabled();
	}
}

void GuiComponent::Disable() noexcept
{
	if (enabled_)
	{
		enabled_ = false;
		Disabled();
	}
}


void GuiComponent::Show() noexcept
{
	if (node_)
		node_->Visible(node_->ParentNode()->Visible(), false);

	if (!visible_)
	{
		visible_ = true;
		Shown();
	}
}

void GuiComponent::Hide() noexcept
{
	if (node_)
		node_->Visible(false);

	if (visible_)
	{
		visible_ = false;
		Hidden();
	}
}


void GuiComponent::Parent(GuiComponent &parent) noexcept
{
	if (owner_ == parent.Owner())
	{
		if (node_)
			parent.Node()->Adopt(node_->ParentNode()->Orphan(*node_));

		parent_ = &parent;
	}
}

void GuiComponent::Owner(GuiContainer &owner) noexcept
{
	managed::ManagedObject<GuiContainer>::Owner(owner);
	Detach();

	parent_ = &owner;
	node_ = parent_->Node() ?
		parent_->Node()->CreateChildNode(visible_ && parent_->Node()->Visible()) :
		nullptr;
}

void GuiComponent::Owner(std::nullptr_t) noexcept
{
	managed::ManagedObject<GuiContainer>::Owner(nullptr);
	Detach();
}

void GuiComponent::ZOrder(real z) noexcept
{
	if (node_)
	{
		auto [x, y, _] = node_->Position().XYZ();
		node_->Position({x, y, z});
	}
}


/*
	Observers
*/

real GuiComponent::ZOrder() const noexcept
{
	return node_ ? node_->Position().Z() : 0.0_r;
}

bool GuiComponent::IsDescendantOf(const GuiContainer &owner) const noexcept
{
	return detail::is_descendant_of(owner, *this);
}

} //ion::gui