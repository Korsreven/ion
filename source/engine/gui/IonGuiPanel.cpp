/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui
File:	IonGuiPanel.cpp
-------------------------------------------
*/

#include "IonGuiPanel.h"

#include "graphics/scene/graph/IonSceneNode.h"

namespace ion::gui
{

namespace gui_panel
{

//Grid cell
//Private

void GridCell::Adopt(SceneNode &node)
{
	if (auto grid = Owner(); grid)
	{
		if (auto panel = grid->Owner(); panel)
		{
			if (!node_)
				node_ = panel->Node()->CreateChildNode(panel->Node()->Visible());

			node_->Adopt(panel->Node()->Orphan(node));
		}
	}
}

void GridCell::Orphan(SceneNode &node)
{
	if (auto grid = Owner(); grid)
	{
		if (auto panel = grid->Owner(); panel)
		{
			panel->Node()->Adopt(node_->Orphan(node));

			if (std::empty(node_->ChildNodes()))
			{
				node_->ParentNode()->RemoveChildNode(*node_);
				node_ = nullptr;
			}
		}
	}
}


//Public

GridCell::GridCell(PanelGrid &owner) noexcept :
	owner_{&owner}
{
	//Empty
}

GridCell::~GridCell() noexcept
{
	DetachAll();
}


/*
	Controls
	Attaching/detaching
*/

bool GridCell::Attach(NonOwningPtr<controls::GuiControl> control)
{
	if (auto owner = Owner();
		owner && control && control->Node() &&
		control->Owner() == owner->Owner() && //Control has same owner
		control->Parent() == owner->Owner() && //Control can be attached
		control->Node()->ParentNode() == owner->Owner()->Node().get()) //Cannot attach same control twice
	{
		Adopt(*control->Node()); //Adopt
		controls_.push_back(control);
		return true;
	}
	else
		return false;
}

bool GridCell::Detach(controls::GuiControl &control) noexcept
{
	auto iter =
		std::find_if(std::begin(controls_), std::end(controls_),
			[&](auto &x) noexcept
			{
				return x.get() == &control;
			});

	//Control found
	if (iter != std::end(controls_))
	{
		Orphan(*control.Node()); //Orphan
		controls_.erase(iter);
		return true;
	}
	else
		return false;
}

void GridCell::DetachAll() noexcept
{
	for (auto &control : controls_)
	{
		if (control)
			Orphan(*control->Node()); //Orphan
	}

	controls_.clear();
	controls_.shrink_to_fit();
}


//Panel grid

PanelGrid::PanelGrid(GuiPanel &owner, const Vector2 &size, int rows, int columns) noexcept :

	size_{size},
	rows_{rows},
	columns_{columns},
	owner_{&owner}
{
	//Empty
}

} //gui_panel


GuiPanel::GuiPanel(std::string name) :
	GuiPanelContainer{std::move(name)}
{
	//Empty
}


/*
	Modifiers
*/

void GuiPanel::Show() noexcept
{
	GuiPanelContainer::Show();

	//Show all controls that should be visible
	for (auto &control : Controls())
	{
		if (control.IsVisible())
			control.Show();
	}

	//Show all panels that should be visible
	for (auto &panel : Panels())
	{
		if (panel.IsVisible())
			panel.Show();
	}
}


void GuiPanel::GridLayout(const Vector2 &size, int rows, int columns)
{
	if (grid_)
		GridLayout(std::nullopt);

	grid_ = {*this, size, rows, columns};
}

void GuiPanel::GridLayout(std::nullopt_t) noexcept
{
	grid_ = {};
}


/*
	Tabulating
*/

void GuiPanel::TabOrder(int order) noexcept
{
	if (auto owner = Owner(); owner)
		owner->TabOrder(*this, order);
}

std::optional<int> GuiPanel::TabOrder() const noexcept
{
	if (auto owner = Owner(); owner)
		return owner->TabOrder(*this);
	else
		return {};
}


/*
	Frame events
*/

void GuiPanel::FrameStarted(duration time) noexcept
{
	for (auto &control : Controls())
		control.FrameStarted(time);

	for (auto &panel : Panels())
		panel.FrameStarted(time);
}

void GuiPanel::FrameEnded(duration time) noexcept
{
	for (auto &control : Controls())
		control.FrameEnded(time);

	for (auto &panel : Panels())
		panel.FrameEnded(time);
}

} //ion::gui