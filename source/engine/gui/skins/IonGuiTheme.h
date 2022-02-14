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

#include <cassert>
#include <string>
#include <string_view>
#include <typeinfo>
#include <type_traits>

#include "IonGuiSkin.h"
#include "gui/controls/IonGuiControl.h"
#include "managed/IonManagedObject.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"

//Forward declarations
namespace ion
{
	namespace graphics::scene
	{
		class SceneManager;
	}

	namespace gui
	{
		class GuiController;
	}
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

			NonOwningPtr<graphics::scene::SceneManager> scene_manager_;


			//Create a skin with the given name and arguments
			template <typename T, typename... Args>
			auto CreateNamedSkin(std::string name, Args &&...args)
			{
				static_assert(std::is_base_of_v<controls::GuiControl, T>);
				return Create<GuiSkin>(std::move(name), typeid(T), std::forward<Args>(args)...);
			}

			//Create a default skin with the given arguments
			template <typename T, typename... Args>
			auto CreateDefaultSkin(Args &&...args)
			{
				static_assert(std::is_base_of_v<controls::GuiControl, T>);

				auto name = GuiSkin::GetDefaultSkinName<T>();
				assert(name.has_value());
				return CreateNamedSkin<T>(std::string{*name}, std::forward<Args>(args)...);
			}

		public:

			//Construct a gui theme with the given scene manager
			GuiTheme(std::string name, NonOwningPtr<graphics::scene::SceneManager> scene_manager) noexcept;


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

			//Sets the scene manager connected to this theme to the given scene manager
			inline void ConnectedSceneManager(NonOwningPtr<graphics::scene::SceneManager> scene_manager) noexcept
			{
				scene_manager_ = scene_manager;
			}


			/*
				Observers
			*/

			//Returns the scene manager connected to this theme
			[[nodiscard]] inline auto ConnectedSceneManager() const noexcept
			{
				return scene_manager_;
			}


			/*
				Skins
				Creating
			*/

			//Create a default skin
			template <typename T>
			NonOwningPtr<GuiSkin> CreateSkin()
			{
				return CreateDefaultSkin<T>();
			}

			//Create a default skin with the given parts, caption part and sound parts
			template <typename T>
			NonOwningPtr<GuiSkin> CreateSkin(const gui_skin::SkinParts &parts,
				const gui_skin::SkinTextPart &caption_part = {}, const gui_skin::SkinSoundParts &sound_parts = {})
			{
				return CreateDefaultSkin<T>(parts, caption_part, sound_parts);
			}

			//Create a default skin with the given border parts, caption part and sound parts
			template <typename T>
			NonOwningPtr<GuiSkin> CreateSkin(const gui_skin::SkinBorderParts &border_parts,
				const gui_skin::SkinTextPart &caption_part = {}, const gui_skin::SkinSoundParts &sound_parts = {})
			{
				return CreateDefaultSkin<T>(border_parts, caption_part, sound_parts);
			}

			//Create a default skin with the given side parts, caption part and sound parts
			template <typename T>
			NonOwningPtr<GuiSkin> CreateSkin(const gui_skin::SkinSideParts &side_parts,
				const gui_skin::SkinTextPart &caption_part = {}, const gui_skin::SkinSoundParts &sound_parts = {})
			{
				return CreateDefaultSkin<T>(side_parts, caption_part, sound_parts);
			}

			//Create a default skin with the given center part, caption part and sound parts
			template <typename T>
			NonOwningPtr<GuiSkin> CreateSkin(const gui_skin::SkinPart &center_part,
				const gui_skin::SkinTextPart &caption_part = {}, const gui_skin::SkinSoundParts &sound_parts = {})
			{
				return CreateDefaultSkin<T>(center_part, caption_part, sound_parts);
			}

			//Create a default skin with the given caption part and sound parts
			template <typename T>
			NonOwningPtr<GuiSkin> CreateSkin(const gui_skin::SkinTextPart &caption_part, const gui_skin::SkinSoundParts &sound_parts = {})
			{
				return CreateDefaultSkin<T>(caption_part, sound_parts);
			}


			//Create a skin with the given name
			template <typename T>
			NonOwningPtr<GuiSkin> CreateSkin(std::string name)
			{
				return CreateNamedSkin<T>(std::move(name));
			}

			//Create a skin with the given name, parts, caption part and sound parts
			template <typename T>
			NonOwningPtr<GuiSkin> CreateSkin(std::string name, const gui_skin::SkinParts &parts,
				const gui_skin::SkinTextPart &caption_part = {}, const gui_skin::SkinSoundParts &sound_parts = {})
			{
				return CreateNamedSkin<T>(std::move(name), parts, caption_part, sound_parts);
			}

			//Create a skin with the given name, border parts, caption part and sound parts
			template <typename T>
			NonOwningPtr<GuiSkin> CreateSkin(std::string name, const gui_skin::SkinBorderParts &border_parts,
				const gui_skin::SkinTextPart &caption_part = {}, const gui_skin::SkinSoundParts &sound_parts = {})
			{
				return CreateNamedSkin<T>(std::move(name), border_parts, caption_part, sound_parts);
			}

			//Create a skin with the given name, side parts, caption part and sound parts
			template <typename T>
			NonOwningPtr<GuiSkin> CreateSkin(std::string name, const gui_skin::SkinSideParts &side_parts,
				const gui_skin::SkinTextPart &caption_part = {}, const gui_skin::SkinSoundParts &sound_parts = {})
			{
				return CreateNamedSkin<T>(std::move(name), side_parts, caption_part, sound_parts);
			}

			//Create a skin with the given name, center part, caption part and sound parts
			template <typename T>
			NonOwningPtr<GuiSkin> CreateSkin(std::string name, const gui_skin::SkinPart &center_part,
				const gui_skin::SkinTextPart &caption_part = {}, const gui_skin::SkinSoundParts &sound_parts = {})
			{
				return CreateNamedSkin<T>(std::move(name), center_part, caption_part, sound_parts);
			}

			//Create a skin with the given name, caption part and sound parts
			template <typename T>
			NonOwningPtr<GuiSkin> CreateSkin(std::string name,
				const gui_skin::SkinTextPart &caption_part, const gui_skin::SkinSoundParts &sound_parts = {})
			{
				return CreateNamedSkin<T>(std::move(name), caption_part, sound_parts);
			}


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