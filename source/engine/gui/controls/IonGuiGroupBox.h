/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiGroupBox.h
-------------------------------------------
*/

#ifndef ION_GUI_GROUP_BOX_H
#define ION_GUI_GROUP_BOX_H

#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "IonGuiControl.h"
#include "adaptors/ranges/IonIterable.h"
#include "memory/IonNonOwningPtr.h"
#include "memory/IonOwningPtr.h"

namespace ion::gui::controls
{
	namespace gui_group_box
	{
		struct GroupBoxSkin : gui_control::ControlSkin
		{
			//Empty
		};


		namespace detail
		{
			using control_pointers = std::vector<NonOwningPtr<controls::GuiControl>>;


			inline auto get_control_by_name(std::string_view name, const control_pointers &controls) noexcept
			{
				auto iter =
					std::find_if(std::begin(controls), std::end(controls),
						[&](auto &control) noexcept
						{
							return control && control->Name() == name;
						});

				return iter != std::end(controls) ? *iter: NonOwningPtr<GuiControl>{};
			}
		} //detail
	} //gui_group_box


	///@brief A class representing a GUI group box that can contain other GUI controls
	///@details When a control has been added (adopted) it will be positioned relative to the group box.
	///When a control has been removed (orphaned) it will automatically be adopted by its owner
	class GuiGroupBox : public GuiControl
	{
		private:

			void DefaultSetup() noexcept;

		protected:

			gui_group_box::detail::control_pointers controls_;


			/**
				@name Skins
				@{
			*/

			virtual OwningPtr<gui_control::ControlSkin> AttuneSkin(OwningPtr<gui_control::ControlSkin> skin) const override;

			///@}

		public:

			///@brief Constructs a group box with the given name, size, caption and hit boxes
			GuiGroupBox(std::string name, const std::optional<Vector2> &size,
				std::optional<std::string> caption, gui_control::BoundingBoxes hit_boxes = {}) noexcept;

			///@brief Constructs a group box with the given name, skin, size, caption and hit boxes
			GuiGroupBox(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
				std::optional<std::string> caption, gui_control::BoundingBoxes hit_boxes = {});


			///@brief Default move constructor
			GuiGroupBox(GuiGroupBox&&) = default;


			///@brief Virtual destructor
			virtual ~GuiGroupBox() noexcept;


			/**
				@name Operators
				@{
			*/

			///@brief Default move assignment
			GuiGroupBox& operator=(GuiGroupBox&&) = default;

			///@}

			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all controls in this group box
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Controls() noexcept
			{
				return adaptors::ranges::Iterable<gui_group_box::detail::control_pointers&>{controls_};
			}

			///@brief Returns an immutable range of all controls in this group box
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Controls() const noexcept
			{
				return adaptors::ranges::Iterable<const gui_group_box::detail::control_pointers&>{controls_};
			}

			///@}

			/**
				@name Controls
				Adding
				@{
			*/

			///@brief Adds the given control to this group box
			///@details Can only add a control with the same owner as this group box
			bool AddControl(NonOwningPtr<controls::GuiControl> control);

			///@brief Adds a control with the given name to this group box
			///@details Can only add a control with the same owner as this group box
			bool AddControl(std::string_view name);

			///@}

			/**
				@name Controls
				Retrieving
				@{
			*/

			///@brief Gets a pointer to a mutable control with the given name
			///@details Returns nullptr if control could not be found
			[[nodiscard]] NonOwningPtr<controls::GuiControl> GetControl(std::string_view name) noexcept;

			///@brief Gets a pointer to an immutable control with the given name
			///@details Returns nullptr if control could not be found
			[[nodiscard]] NonOwningPtr<const controls::GuiControl> GetControl(std::string_view name) const noexcept;

			///@}

			/**
				@name Controls
				Removing
				@{
			*/

			///@brief Clears all controls from this group box
			void ClearControls() noexcept;

			///@brief Removes a control from this group box
			bool RemoveControl(controls::GuiControl &control) noexcept;

			///@brief Removes a control with the given name from this group box
			bool RemoveControl(std::string_view name) noexcept;

			///@}
	};

} //ion::gui::controls

#endif