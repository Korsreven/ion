/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiScrollBar.h
-------------------------------------------
*/

#ifndef ION_GUI_SCROLL_BAR_H
#define ION_GUI_SCROLL_BAR_H

#include <optional>
#include <string>
#include <utility>

#include "IonGuiSlider.h"
#include "events/listeners/IonMouseListener.h"
#include "graphics/utilities/IonVector2.h"
#include "memory/IonNonOwningPtr.h"
#include "memory/IonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::gui::controls
{
	using namespace graphics::utilities;
	using namespace types::type_literals;

	class GuiScrollable; //Forward declaration

	namespace gui_scroll_bar
	{
		struct ScrollBarSkin : gui_slider::SliderSkin
		{
			//Empty
		};


		namespace detail
		{
			constexpr auto default_handle_size_min_percent = 0.05_r;
			constexpr auto default_handle_size_max_percent = 1.0_r;
		} //detail
	} //gui_scroll_bar


	///@brief A class representing a GUI scroll bar that can be horizontal or vertical, flipped or not
	///@details A scroll bar can be attached to a GUI scrollable and vice versa (usually to scroll up and down).
	///The scroll bar handle can be dragged with the mouse cursor to change value
	class GuiScrollBar : public GuiSlider
	{
		private:

			void DefaultSetup() noexcept;

		protected:

			std::pair<real, real> handle_size_ =
				{gui_scroll_bar::detail::default_handle_size_min_percent,
				 gui_scroll_bar::detail::default_handle_size_max_percent};
			NonOwningPtr<GuiScrollable> scrollable_;


			/**
				@name Events
				@{
			*/

			//See GuiSlider::Slid for more details
			virtual void Slid(int delta) noexcept override;

			///@}

			/**
				@name Skins
				@{
			*/

			virtual OwningPtr<gui_control::ControlSkin> AttuneSkin(OwningPtr<gui_control::ControlSkin> skin) const override;
			virtual void UpdateHandle() noexcept override;

			///@}

		public:

			///@brief Constructs a scroll bar with the given name, size, caption, type and hit boxes
			GuiScrollBar(std::string name, const std::optional<Vector2> &size,
				std::optional<std::string> caption, gui_slider::SliderType type = gui_slider::SliderType::Vertical,
				gui_control::BoundingBoxes hit_boxes = {}) noexcept;

			///@brief Constructs a scroll bar with the given name, skin, size, caption, type and hit boxes
			GuiScrollBar(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
				std::optional<std::string> caption, gui_slider::SliderType type = gui_slider::SliderType::Vertical,
				gui_control::BoundingBoxes hit_boxes = {});


			///@brief Default move constructor
			GuiScrollBar(GuiScrollBar&&) = default;


			///@brief Virtual destructor
			virtual ~GuiScrollBar() noexcept;


			/**
				@name Operators
				@{
			*/

			///@brief Default move assignment
			GuiScrollBar& operator=(GuiScrollBar&&) = default;

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the min/max handle size of this scroll bar to the given range (in percentages)
			inline void HandleSize(real min_percent, real max_percent) noexcept
			{
				auto min = min_percent;
				auto max = min_percent > max_percent ? min_percent : max_percent;

				if (auto handle_size = std::pair{min, max}; handle_size_ != handle_size)
				{
					handle_size_ = handle_size;
					UpdateHandle();
				}
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the min/max handle size of this scroll bar (in percentages)
			[[nodiscard]] inline auto& HandleSize() const noexcept
			{
				return handle_size_;
			}

			///@}

			/**
				@name Scrollable
				@{
			*/

			///@brief Sets the scrollable attached to this scroll bar to the given scrollable
			void AttachedScrollable(NonOwningPtr<GuiScrollable> scrollable) noexcept;

			///@brief Returns a pointer to the scrollable attached to this scroll bar
			///@details Returns nullptr if this scroll bar does not have a scrollable attached
			[[nodiscard]] inline auto AttachedScrollable() const noexcept
			{
				return scrollable_;
			}

			///@}

			/**
				@name Mouse events
				@{
			*/

			///@brief Called from gui slider when the mouse button has been released
			///@details Returns true if the mouse release event has been consumed by the control
			virtual bool MouseReleased(MouseButton button, Vector2 position) noexcept override;

			///@}
	};

} //ion::gui::controls

#endif