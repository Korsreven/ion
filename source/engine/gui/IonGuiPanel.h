/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui
File:	IonGuiPanel.h
-------------------------------------------
*/

#ifndef ION_GUI_PANEL_H
#define ION_GUI_PANEL_H

#include <cassert>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "IonGuiPanelContainer.h"
#include "adaptors/IonFlatMap.h"
#include "adaptors/ranges/IonIterable.h"
#include "controls/IonGuiControl.h"
#include "graphics/utilities/IonVector2.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::gui
{
	using namespace events::listeners;
	using namespace graphics::utilities;

	class GuiPanel; //Forward declaration

	namespace gui_panel
	{
		enum class GridCellAlignment
		{
			Left,
			Center,
			Right
		};

		enum class GridCellVerticalAlignment
		{
			Top,
			Middle,
			Bottom
		};


		namespace detail
		{
			using control_pointers = std::vector<NonOwningPtr<controls::GuiControl>>;
		} //detail


		class PanelGrid; //Forward declaration

		class GridCell final
		{
			private:

				GridCellAlignment alignment_ = GridCellAlignment::Center;
				GridCellVerticalAlignment vertical_alignment_ = GridCellVerticalAlignment::Middle;
				PanelGrid *owner_ = nullptr;

				detail::control_pointers controls_;
				NonOwningPtr<SceneNode> node_;

				void Adopt(SceneNode &node);
				void Orphan(SceneNode &node);

			public:
			
				//Construct a grid cell with the given owner
				explicit GridCell(PanelGrid &owner) noexcept;

				//Destructor
				~GridCell() noexcept;


				/*
					Ranges
				*/

				//Returns a mutable range of all controls in this grid cell
				//This can be used directly with a range-based for loop
				[[nodiscard]] inline auto Controls() noexcept
				{
					return adaptors::ranges::Iterable<detail::control_pointers&>{controls_};
				}

				//Returns an immutable range of all controls in this grid cell
				//This can be used directly with a range-based for loop
				[[nodiscard]] inline auto Controls() const noexcept
				{
					return adaptors::ranges::Iterable<const detail::control_pointers&>{controls_};
				}


				/*
					Modifiers
				*/

				//Sets the horizontal alignment for this grid cells attached controls to the given alignment
				inline void Alignment(GridCellAlignment alignment) noexcept
				{
					if (alignment_ != alignment)
					{
						alignment_ = alignment;
					}
				}

				//Sets the vertical alignment for this grid cells attached controls to the given alignment
				inline void VerticalAlignment(GridCellVerticalAlignment vertical_alignment) noexcept
				{
					if (vertical_alignment_ != vertical_alignment)
					{
						vertical_alignment_ = vertical_alignment;
					}
				}


				/*
					Observers
				*/

				//Returns the horizontal alignment for this grid cells attached controls
				[[nodiscard]] inline auto Alignment() const noexcept
				{
					return alignment_;
				}

				//Returns the vertical alignment for this grid cells attached controls
				[[nodiscard]] inline auto VerticalAlignment() const noexcept
				{
					return vertical_alignment_;
				}

				//Returns a pointer to the owner of this grid cell
				[[nodiscard]] inline auto Owner() const noexcept
				{
					return owner_;
				}


				/*
					Controls
					Attaching/detaching
				*/

				//Attaches the given control to this grid cell
				bool Attach(NonOwningPtr<controls::GuiControl> control);

				//Detaches the given control from this grid cell
				bool Detach(controls::GuiControl &control) noexcept;

				//Detaches all controls attached to this grid cell
				void DetachAll() noexcept;
		};

		class PanelGrid final
		{
			private:

				Vector2 size_;
				int rows_ = 0;
				int columns_ = 0;
				GuiPanel *owner_ = nullptr;

				adaptors::FlatMap<std::pair<int, int>, GridCell> cells_;

			public:

				//Construct a panel grid with the given owner, size, rows and columns
				PanelGrid(GuiPanel &owner, const Vector2 &size, int rows, int columns) noexcept;


				/*
					Ranges
				*/

				//Returns a mutable range of all cells in this panel grid
				//This can be used directly with a range-based for loop
				[[nodiscard]] inline auto Cells() noexcept
				{
					return cells_.Elements();
				}

				//Returns an immutable range of all cells in this panel grid
				//This can be used directly with a range-based for loop
				[[nodiscard]] inline auto Cells() const noexcept
				{
					return cells_.Elements();
				}


				/*
					Operators
				*/

				//Returns a mutable reference to the grid cell at the given offset
				[[nodiscard]] auto& operator[](std::pair<int, int> off) noexcept
				{
					assert(off.first >= 0 && off.first < rows_ &&
						   off.second >= 0 && off.second < columns_);

					if (auto iter = cells_.find(off); iter != std::end(cells_))
						return iter->second;
					else
						return cells_.emplace(off, *this).first->second;
				}
				
				//Returns an immutable reference to the grid cell at the given offset
				[[nodiscard]] auto& operator[](std::pair<int, int> off) const noexcept
				{
					assert(off.first >= 0 && off.first < rows_ &&
						   off.second >= 0 && off.second < columns_);
					return cells_.at(off);
				}


				/*
					Observers
				*/

				//Returns the size of this panel grid
				[[nodiscard]] inline auto Size() const noexcept
				{
					return size_;
				}

				//Returns the number of rows in this panel grid
				[[nodiscard]] inline auto Rows() const noexcept
				{
					return rows_;
				}

				//Returns the number of columns in this panel grid
				[[nodiscard]] inline auto Columns() const noexcept
				{
					return columns_;
				}

				//Returns a pointer to the owner of this panel grid
				[[nodiscard]] inline auto Owner() const noexcept
				{
					return owner_;
				}
		};


		namespace detail
		{
		} //detail
	} //gui_panel


	class GuiPanel : public GuiPanelContainer
	{
		private:

			std::optional<gui_panel::PanelGrid> grid_;

		public:

			//Construct a panel with the given name
			explicit GuiPanel(std::string name);


			/*
				Operators
			*/

			//Returns a mutable reference to the grid cell at the given offset
			[[nodiscard]] auto& operator[](std::pair<int, int> off) noexcept
			{
				assert(grid_.has_value());
				return (*grid_)[off];
			}
				
			//Returns an immutable reference to the grid cell at the given offset
			[[nodiscard]] auto& operator[](std::pair<int, int> off) const noexcept
			{
				assert(grid_.has_value());
				return (*grid_)[off];
			}


			/*
				Modifiers
			*/

			//Show this panel
			void Show() noexcept;


			//Sets the grid layout for this gui panel to the given size, rows and columns
			void GridLayout(const Vector2 &size, int rows, int columns);

			//Unsets the grid layout for this gui panel
			void GridLayout(std::nullopt_t) noexcept;


			/*
				Observers
			*/

			//Returns a mutable reference to the grid layout for this gui panel
			//Returns nullopt if this gui panel has no grid layout
			[[nodiscard]] inline auto& GridLayout() noexcept
			{
				return grid_;
			}

			//Returns an immutable reference to the grid layout for this gui panel
			//Returns nullopt if this gui panel has no grid layout
			[[nodiscard]] inline auto& GridLayout() const noexcept
			{
				return grid_;
			}


			//Returns a pointer to the owner of this panel
			[[nodiscard]] inline auto Owner() const noexcept
			{
				return static_cast<GuiPanelContainer*>(owner_);
			}


			/*
				Tabulating
			*/

			//Sets the tab order of this panel to the given order
			void TabOrder(int order) noexcept;

			//Returns the tab order of this panel
			//Returns nullopt if this panel has no owner
			[[nodiscard]] std::optional<int> TabOrder() const noexcept;


			/*
				Frame events
			*/

			//Called from gui frame when a frame has started
			virtual void FrameStarted(duration time) noexcept;

			//Called from gui frame when a frame has ended
			virtual void FrameEnded(duration time) noexcept;
	};
} //ion::gui

#endif