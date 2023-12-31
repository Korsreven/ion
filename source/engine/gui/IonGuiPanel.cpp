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

#include "graphics/render/IonViewport.h"
#include "graphics/scene/IonCamera.h"
#include "graphics/scene/graph/IonSceneNode.h"
#include "gui/IonGuiController.h"
#include "gui/IonGuiFrame.h"

namespace ion::gui
{

using namespace gui_panel;

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
				node_ = panel->Node()->CreateChildNode({}, Position(), vector2::UnitY);

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


void GridCell::Align(controls::GuiControl &control) noexcept
{
	if (auto size = control.Size(); size && node_)
	{	
		auto position = vector2::Zero;

		switch (alignment_)
		{
			case GridCellAlignment::Left:
			position.X(size->X() * 0.5_r);
			break;

			case GridCellAlignment::Right:
			position.X(-size->X() * 0.5_r);
			break;
		}

		switch (vertical_alignment_)
		{
			case GridCellVerticalAlignment::Top:
			position.Y(-size->Y() * 0.5_r);
			break;

			case GridCellVerticalAlignment::Bottom:
			position.Y(size->Y() * 0.5_r);
			break;
		}

		control.Node()->Position(position);
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
	DetachAllControls();
}


/*
	Modifiers
*/

void GridCell::Show() noexcept
{
	if (node_ && node_->ParentNode())
		node_->Visible(node_->ParentNode()->Visible(), false);
}

void GridCell::Realign() noexcept
{
	for (auto &control : Controls())
	{
		if (control)
			Align(*control);
	}
}

void GridCell::Reposition() noexcept
{
	if (node_)
		node_->Position(Position());
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
			auto [cell_width, cell_height] = Size().XY();
			auto [row, column] = Offset();

			auto position = panel->Node()->Position() +
				Vector2{-grid_width * 0.5_r + cell_width * 0.5_r, grid_height * 0.5_r - cell_height * 0.5_r} +
				Vector2{cell_width, -cell_height} * Vector2{static_cast<real>(column), static_cast<real>(row)};

			switch (alignment_)
			{
				case GridCellAlignment::Left:
				position.X(position.X() - cell_width * 0.5_r);
				break;

				case GridCellAlignment::Right:
				position.X(position.X() + cell_width * 0.5_r);
				break;
			}

			switch (vertical_alignment_)
			{
				case GridCellVerticalAlignment::Top:
				position.Y(position.Y() + cell_height * 0.5_r);
				break;

				case GridCellVerticalAlignment::Bottom:
				position.Y(position.Y() - cell_height * 0.5_r);
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
	Controls - Attaching/detaching
*/

bool GridCell::AttachControl(NonOwningPtr<controls::GuiControl> control)
{
	if (auto grid = Owner(); grid && control && control->Node() &&
		control->Owner() == grid->Owner() && //Control and grid has same owner
		control->Parent() == grid->Owner() && //Parent of the control is the owner of the grid
		control->Node()->ParentNode() == grid->Owner()->Node().get())
			//Parent node of the control is the node of the owner of the grid
	{
		Adopt(*control->Node()); //Adopt
		Align(*control);
		controls_.push_back(control);
		return true;
	}
	else
		return false;
}

bool GridCell::DetachControl(controls::GuiControl &control) noexcept
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

void GridCell::DetachAllControls() noexcept
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
//Private

GuiController* PanelGrid::GetController() const noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = Owner()->ParentFrame(); frame)
			return frame->Owner();
	}
	
	return nullptr;
}

Vector2 PanelGrid::ViewSize() const noexcept
{
	//Get ortho size from the default viewport camera
	if (auto controller = GetController(); controller)
	{
		if (auto viewport = controller->DefaultViewport(); viewport)
		{
			if (auto camera = viewport->ConnectedCamera(); camera)
			{
				auto frustum = camera->ViewFrustum(vector2::UnitScale); //No scaling
				auto [left, right, bottom, top, z_near, z_far] = frustum.ToOrthoBounds(viewport->Bounds().ToSize());
				return {right - left, top - bottom};
			}
		}
	}

	return vector2::Zero;
}


//Public

PanelGrid::PanelGrid(GuiPanel &owner, int rows, int columns) noexcept :

	rows_{rows},
	columns_{columns},
	size_percentage_{vector2::UnitScale},
	owner_{&owner}
{
	size_ = ViewSize();
}

PanelGrid::PanelGrid(GuiPanel &owner, int rows, int columns, const Vector2 &size) noexcept :

	rows_{rows},
	columns_{columns},
	size_{size},
	owner_{&owner}
{
	//Empty
}


/*
	Modifiers
*/

void PanelGrid::Show() noexcept
{
	for (auto &[off, cell] : Cells())
		cell.Show();
}

void PanelGrid::Realign() noexcept
{
	for (auto &[off, cell] : Cells())
		cell.Realign();
}

void PanelGrid::Reposition() noexcept
{
	for (auto &[off, cell] : Cells())
		cell.Reposition();
}

void PanelGrid::Resize() noexcept
{
	if (size_percentage_)
	{
		size_ = ViewSize() * *size_percentage_;
		Reposition();
	}
}

} //gui_panel


GuiPanel::GuiPanel(std::string name) noexcept :
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

	if (grid_)
		grid_->Show();

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


gui_panel::PanelGrid& GuiPanel::GridLayout(int rows, int columns)
{
	if (grid_)
		GridLayout(std::nullopt);

	grid_.emplace(*this, rows < 1 ? 1 : rows, columns < 1 ? 1 : columns);
	return *grid_;
}

PanelGrid& GuiPanel::GridLayout(int rows, int columns, const Vector2 &size)
{
	if (grid_)
		GridLayout(std::nullopt);

	grid_.emplace(*this, rows < 1 ? 1 : rows, columns < 1 ? 1 : columns, size);
	return *grid_;
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


/*
	Viewport events
*/

void GuiPanel::ViewportResized(graphics::render::Viewport &viewport) noexcept
{
	for (auto &panel : Panels())
		panel.ViewportResized(viewport);

	if (grid_)
		grid_->Resize();
}

} //ion::gui