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

namespace gui_component::detail
{

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
	node_ = parent_->Node() ? parent_->Node()->CreateChildNode() : nullptr;
}

void GuiComponent::Release() noexcept
{
	managed::ManagedObject<GuiContainer>::Release();
	Detach();
}

} //ion::gui