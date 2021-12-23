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


	class GuiGroupBox : public GuiControl
	{
		private:

			void DefaultSetup() noexcept;

		protected:

			gui_group_box::detail::control_pointers controls_;

		public:

			//Construct a group box with the given name, caption and skin
			GuiGroupBox(std::string name, std::optional<std::string> caption, OwningPtr<gui_group_box::GroupBoxSkin> skin);

			//Construct a group box with the given name, caption, skin and size
			GuiGroupBox(std::string name, std::optional<std::string> caption, OwningPtr<gui_group_box::GroupBoxSkin> skin, const Vector2 &size);

			//Construct a group box with the given name, caption, skin and hit areas
			GuiGroupBox(std::string name, std::optional<std::string> caption, OwningPtr<gui_group_box::GroupBoxSkin> skin, gui_control::Areas areas);


			//Default move constructor
			GuiGroupBox(GuiGroupBox&&) = default;


			//Virtual destructor
			virtual ~GuiGroupBox() noexcept;


			/*
				Operators
			*/

			//Default move assignment
			GuiGroupBox& operator=(GuiGroupBox&&) = default;


			/*
				Ranges
			*/

			//Returns a mutable range of all controls in this group box
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Controls() noexcept
			{
				return adaptors::ranges::Iterable<gui_group_box::detail::control_pointers&>{controls_};
			}

			//Returns an immutable range of all controls in this group box
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Controls() const noexcept
			{
				return adaptors::ranges::Iterable<const gui_group_box::detail::control_pointers&>{controls_};
			}


			/*
				Controls
				Adding
			*/

			//Adds the given control to this group box
			//Can only add a control with the same owner as this group box
			bool AddControl(NonOwningPtr<controls::GuiControl> control);

			//Adds a control with the given name to this group box
			//Can only add a control with the same owner as this group box
			bool AddControl(std::string_view name);


			/*
				Controls
				Retrieving
			*/

			//Gets a pointer to a mutable control with the given name
			//Returns nullptr if control could not be found
			[[nodiscard]] NonOwningPtr<controls::GuiControl> GetControl(std::string_view name) noexcept;

			//Gets a pointer to an immutable control with the given name
			//Returns nullptr if control could not be found
			[[nodiscard]] NonOwningPtr<const controls::GuiControl> GetControl(std::string_view name) const noexcept;


			/*
				Controls
				Removing
			*/

			//Clear all controls from this group box
			void ClearControls() noexcept;

			//Remove a control from this group box
			bool RemoveControl(controls::GuiControl &control) noexcept;

			//Remove a control with the given name from this group box
			bool RemoveControl(std::string_view name) noexcept;
	};

} //ion::gui::controls

#endif