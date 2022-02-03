/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/skins
File:	IonGuiTheme.h
-------------------------------------------
*/

#ifndef ION_GUI_THEME_H
#define ION_GUI_THEME_H

#include <string_view>

#include "IonGuiSkin.h"
#include "managed/IonManagedObject.h"
#include "managed/IonObjectManager.h"

namespace ion::gui
{
	//Forward declarations
	class GuiController;
}

namespace ion::gui::skins
{
	namespace gui_theme::detail
	{
	} //gui_theme::detail


	class GuiTheme final :
		public managed::ManagedObject<GuiController>,
		public managed::ObjectManager<GuiSkin, GuiTheme>
	{
		private:



		public:

			using managed::ManagedObject<GuiController>::ManagedObject;


			/*
				Ranges
			*/

			//Returns a mutable range of all skins in this theme
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Skins() noexcept
			{
				return Objects();
			}

			//Returns an immutable range of all skins in this theme
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Skins() const noexcept
			{
				return Objects();
			}


			/*
				Modifiers
			*/




			/*
				Observers
			*/

			


			/*
				Skins
				Creating
			*/

			//Create a skin with the given name
			NonOwningPtr<GuiSkin> CreateSkin(std::string name);

			//Create a skin by copying the given skin
			NonOwningPtr<GuiSkin> CreateSkin(const GuiSkin &skin);

			//Create a skin by moving the given skin
			NonOwningPtr<GuiSkin> CreateSkin(GuiSkin &&skin);


			/*
				Skins
				Retrieving
			*/

			//Gets a pointer to a mutable skin with the given name
			//Returns nullptr if skin could not be found
			[[nodiscard]] NonOwningPtr<GuiSkin> GetSkin(std::string_view name) noexcept;

			//Gets a pointer to an immutable skin with the given name
			//Returns nullptr if skin could not be found
			[[nodiscard]] NonOwningPtr<const GuiSkin> GetSkin(std::string_view name) const noexcept;


			/*
				Skins
				Removing
			*/

			//Clear all removable skins from this theme
			void ClearSkins() noexcept;

			//Remove a removable skin from this theme
			bool RemoveSkin(GuiSkin &skin) noexcept;

			//Remove a removable skin with the given name from this theme
			bool RemoveSkin(std::string_view name) noexcept;
	};
} //ion::gui::skins

#endif