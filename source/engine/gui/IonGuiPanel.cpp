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
				node_ = panel->Node()->CreateChildNode(Position(), vector2::UnitY, panel->Node()->Visible());

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
	Modifiers
*/

void GridCell::Alignment(GridCellAlignment alignment) noexcept
{
	if (alignment_ != alignment)
	{
		alignment_ = alignment;

		if (node_)
			node_->Position(Position());
	}
}

void GridCell::VerticalAlignment(GridCellVerticalAlignment vertical_alignment) noexcept
{
	if (vertical_alignment_ != vertical_alignment)
	{
		vertical_alignment_ = vertical_alignment;

		if (node_)
			node_->Position(Position());
	}
}


/*
	Observers
*/

Vector2 GridCell::Position() const noexcept
{
	if (auto grid = Owner(); grid)
	{
		if (auto panel = grid->Owner(); panel)
		{
			auto [grid_width, grid_height] = grid->Size().XY();
			auto cell_size = Size();
			auto [row, column] = Offset();

			auto position = panel->Node()->Position() -
				Vector2{grid_width * 0.5_r, -grid_height * 0.5_r} - cell_size * 0.5_r +
				cell_size * Vector2{static_cast<real>(row + 1), static_cast<real>(column + 1)};

			switch (alignment_)
			{
				case GridCellAlignment::Left:
				position.X(position.X() - cell_size.X() * 0.5_r);
				break;

				case GridCellAlignment::Right:
				position.X(position.X() + cell_size.X() * 0.5_r);
				break;
			}

			switch (vertical_alignment_)
			{
				case GridCellVerticalAlignment::Top:
				position.Y(position.Y() + cell_size.Y() * 0.5_r);
				break;

				case GridCellVerticalAlignment::Bottom:
				position.Y(position.Y() - cell_size.Y() * 0.5_r);
				break;
			}

			return position;
		}
	}

	return vector2::Zero;
}

Vector2 GridCell::Size() const noexcept
{
	if (auto grid = Owner(); grid)
		return grid->Size() /
			Vector2{static_cast<real>(grid->Rows()), static_cast<real>(grid->Columns())};
	else
		return vector2::Zero;
}

std::pair<int, int> GridCell::Offset() const noexcept
{
	if (auto grid = Owner(); grid)
	{
		for (auto &[off, cell] : grid->Cells())
		{
			if (this == &cell)
				return off;
		}
	}

	return std::pair{0, 0};
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