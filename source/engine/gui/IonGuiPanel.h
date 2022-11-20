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

		///@brief A class representing a grid cell that can attach and align GUI controls
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

				void Align(controls::GuiControl &control) noexcept;

			public:
			
				///@brief Constructs a grid cell with the given owner
				explicit GridCell(PanelGrid &owner) noexcept;

				///@brief Destructor
				~GridCell() noexcept;


				/**
					@name Ranges
					@{
				*/

				///@brief Returns a mutable range of all controls in this grid cell
				///@details This can be used directly with a range-based for loop
				[[nodiscard]] inline auto Controls() noexcept
				{
					return adaptors::ranges::Iterable<detail::control_pointers&>{controls_};
				}

				///@brief Returns an immutable range of all controls in this grid cell
				///@details This can be used directly with a range-based for loop
				[[nodiscard]] inline auto Controls() const noexcept
				{
					return adaptors::ranges::Iterable<const detail::control_pointers&>{controls_};
				}

				///@}

				/**
					@name Modifiers
					@{
				*/

				///@brief Sets the horizontal alignment for this grid cells attached controls to the given alignment
				inline void Alignment(GridCellAlignment alignment) noexcept
				{
					if (alignment_ != alignment)
					{
						alignment_ = alignment;
						Reposition();
						Realign();
					}
				}

				///@brief Sets the vertical alignment for this grid cells attached controls to the given alignment
				inline void VerticalAlignment(GridCellVerticalAlignment vertical_alignment) noexcept
				{
					if (vertical_alignment_ != vertical_alignment)
					{
						vertical_alignment_ = vertical_alignment;
						Reposition();
						Realign();
					}
				}


				///@brief Shows this grid cell
				void Show() noexcept;

				///@brief Realigns all controls attached to this grid cell
				void Realign() noexcept;

				///@brief Repositions the node for this grid cell
				void Reposition() noexcept;

				///@}

				/**
					@name Observers
					@{
				*/

				///@brief Returns the horizontal alignment for this grid cells attached controls
				[[nodiscard]] inline auto Alignment() const noexcept
				{
					return alignment_;
				}

				///@brief Returns the vertical alignment for this grid cells attached controls
				[[nodiscard]] inline auto VerticalAlignment() const noexcept
				{
					return vertical_alignment_;
				}

				///@brief Returns a pointer to the owner of this grid cell
				[[nodiscard]] inline auto Owner() const noexcept
				{
					return owner_;
				}


				///@brief Returns the position of this grid cell
				[[nodiscard]] Vector2 Position() const noexcept;

				///@brief Returns the size of this grid cell
				[[nodiscard]] Vector2 Size() const noexcept;

				///@brief Returns the offset of this grid cell
				[[nodiscard]] std::pair<int, int> Offset() const noexcept;

				///@}

				/**
					@name Controls - Attaching/detaching
					@{
				*/

				///@brief Attaches the given control to this grid cell
				bool AttachControl(NonOwningPtr<controls::GuiControl> control);

				///@brief Detaches the given control from this grid cell
				bool DetachControl(controls::GuiControl &control) noexcept;

				///@brief Detaches all controls attached to this grid cell
				void DetachAllControls() noexcept;

				///@}
		};

		///@brief A class representing a panel grid with size, rows and columns
		///@details A panel grid can contain multiple (rows * columns) grid cells
		class PanelGrid final
		{
			private:

				Vector2 size_;
				int rows_ = 0;
				int columns_ = 0;
				GuiPanel *owner_ = nullptr;

				adaptors::FlatMap<std::pair<int, int>, GridCell> cells_;

			public:

				///@brief Constructs a panel grid with the given owner, size, rows and columns
				PanelGrid(GuiPanel &owner, const Vector2 &size, int rows, int columns) noexcept;


				/**
					@name Ranges
					@{
				*/

				///@brief Returns a mutable range of all cells in this panel grid
				///@details This can be used directly with a range-based for loop
				[[nodiscard]] inline auto Cells() noexcept
				{
					return cells_.Elements();
				}

				///@brief Returns an immutable range of all cells in this panel grid
				///@details This can be used directly with a range-based for loop
				[[nodiscard]] inline auto Cells() const noexcept
				{
					return cells_.Elements();
				}

				///@}

				/**
					@name Operators
					@{
				*/

				///@brief Returns a mutable reference to the grid cell at the given offset
				[[nodiscard]] inline auto& operator[](std::pair<int, int> off) noexcept
				{
					assert(off.first >= 0 && off.first < rows_ &&
						   off.second >= 0 && off.second < columns_);

					if (auto iter = cells_.find(off); iter != std::end(cells_))
						return iter->second;
					else
						return cells_.emplace(off, *this).first->second;
				}
				
				///@brief Returns an immutable reference to the grid cell at the given offset
				[[nodiscard]] inline auto& operator[](std::pair<int, int> off) const noexcept
				{
					assert(off.first >= 0 && off.first < rows_ &&
						   off.second >= 0 && off.second < columns_);
					return cells_.at(off);
				}

				///@}

				/**
					@name Modifiers
					@{
				*/

				///@brief Sets the size of this panel grid to the given size
				inline void Size(const Vector2 &size) noexcept
				{
					if (size_ != size)
					{
						size_ = size;
						Reposition();
					}
				}


				///@brief Shows this panel grid
				void Show() noexcept;

				///@brief Realigns all controls attached to this panel grid
				void Realign() noexcept;

				///@brief Repositions all cells in this panel grid
				void Reposition() noexcept;

				///@}

				/**
					@name Observers
					@{
				*/

				///@brief Returns the size of this panel grid
				[[nodiscard]] inline auto& Size() const noexcept
				{
					return size_;
				}

				///@brief Returns the number of rows in this panel grid
				[[nodiscard]] inline auto Rows() const noexcept
				{
					return rows_;
				}

				///@brief Returns the number of columns in this panel grid
				[[nodiscard]] inline auto Columns() const noexcept
				{
					return columns_;
				}

				///@brief Returns a pointer to the owner of this panel grid
				[[nodiscard]] inline auto Owner() const noexcept
				{
					return owner_;
				}

				///@}
		};


		namespace detail
		{
		} //detail
	} //gui_panel


	///@brief A class representing a GUI panel that can create and store multiple GUI controls and GUI panels
	///@details A panel can contain multiple rows/columns in a grid layout to easier position and align attached controls
	class GuiPanel : public GuiPanelContainer
	{
		private:

			std::optional<gui_panel::PanelGrid> grid_;

		public:

			///@brief Constructs a panel with the given name
			explicit GuiPanel(std::string name) noexcept;


			/**
				@name Modifiers
				@{
			*/

			///@brief Shows this panel
			void Show() noexcept;


			///@brief Sets the grid layout for this gui panel to the given size, rows and columns
			gui_panel::PanelGrid& GridLayout(const Vector2 &size, int rows, int columns);

			///@brief Clears the grid layout for this gui panel
			void GridLayout(std::nullopt_t) noexcept;

			///@}

			/**
				Observers
				@{
			*/

			///@brief Returns a mutable reference to the grid defined for this gui panel
			///@details Returns nullopt if this gui panel has no grid
			[[nodiscard]] inline auto& Grid() noexcept
			{
				return grid_;
			}

			///@brief Returns an immutable reference to the grid defined for this gui panel
			///@details Returns nullopt if this gui panel has no grid
			[[nodiscard]] inline auto& Grid() const noexcept
			{
				return grid_;
			}


			///@brief Returns a pointer to the owner of this panel
			[[nodiscard]] inline auto Owner() const noexcept
			{
				return static_cast<GuiPanelContainer*>(owner_);
			}

			///@}

			/**
				@name Tabulating
				@{
			*/

			///@brief Sets the tab order of this panel to the given order
			void TabOrder(int order) noexcept;

			///@brief Returns the tab order of this panel
			///@details Returns nullopt if this panel has no owner
			[[nodiscard]] std::optional<int> TabOrder() const noexcept;

			///@}

			/**
				@name Frame events
				@{
			*/

			///@brief Called from gui frame when a frame has started
			virtual void FrameStarted(duration time) noexcept;

			///@brief Called from gui frame when a frame has ended
			virtual void FrameEnded(duration time) noexcept;

			///@}
	};
} //ion::gui

#endif