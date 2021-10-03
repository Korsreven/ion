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