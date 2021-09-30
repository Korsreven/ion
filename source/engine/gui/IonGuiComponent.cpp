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


GuiComponent::GuiComponent(std::string name) :
	managed::ManagedObject<GuiContainer>{std::move(name)}
{
	//Empty
}

GuiComponent::GuiComponent(GuiContainer &owner, std::string name) :
	managed::ManagedObject<GuiContainer>{std::move(name)},
	node_{owner.Node() ? owner.Node()->CreateChildNode() : nullptr}
{
	//Empty
}

GuiComponent::~GuiComponent() noexcept
{
	if (node_)
		node_->ParentNode()->RemoveChildNode(*node_);
}

} //ion::gui