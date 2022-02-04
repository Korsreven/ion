/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/skins
File:	IonGuiSkin.h
-------------------------------------------
*/

#ifndef ION_GUI_SKIN_H
#define ION_GUI_SKIN_H

#include <optional>
#include <string>
#include <string_view>

#include "adaptors/IonFlatMap.h"
#include "events/IonCallback.h"
#include "graphics/fonts/IonText.h"
#include "gui/controls/IonGuiControl.h"
#include "managed/IonManagedObject.h"
#include "memory/IonNonOwningPtr.h"
#include "memory/IonOwningPtr.h"
#include "types/IonSingleton.h"

//Forward declarations
namespace ion::graphics
{
	namespace materials
	{
		class Material;
	}

	namespace scene
	{
		class SceneManager;
	}
}

namespace ion::gui::skins
{
	//Forward declarations
	class GuiTheme;

	namespace gui_skin
	{
		struct SkinPart
		{
			NonOwningPtr<graphics::materials::Material> Enabled;
			NonOwningPtr<graphics::materials::Material> Disabled;
			NonOwningPtr<graphics::materials::Material> Focused;
			NonOwningPtr<graphics::materials::Material> Pressed;
			NonOwningPtr<graphics::materials::Material> Hovered;

			[[nodiscard]] inline operator bool() const noexcept
			{
				return Enabled || Disabled || Focused || Pressed || Hovered;
			}
		};

		struct SkinTextPart
		{
			std::optional<graphics::fonts::text::TextBlockStyle> Enabled;
			std::optional<graphics::fonts::text::TextBlockStyle> Disabled;
			std::optional<graphics::fonts::text::TextBlockStyle> Focused;
			std::optional<graphics::fonts::text::TextBlockStyle> Pressed;
			std::optional<graphics::fonts::text::TextBlockStyle> Hovered;

			[[nodiscard]] inline operator bool() const noexcept
			{
				return Enabled || Disabled || Focused || Pressed || Hovered;
			}
		};

		struct SkinSideParts
		{
			SkinPart Top;
			SkinPart Bottom;
			SkinPart Left;
			SkinPart Right;

			[[nodiscard]] inline operator bool() const noexcept
			{
				return Top || Bottom || Left || Right;
			}
		};

		struct SkinCornerParts
		{
			SkinPart TopLeft;
			SkinPart TopRight;
			SkinPart BottomLeft;
			SkinPart BottomRight;

			[[nodiscard]] inline operator bool() const noexcept
			{
				return TopLeft || TopRight || BottomLeft || BottomRight;
			}
		};

		struct SkinBorderParts
		{
			SkinSideParts Sides;
			SkinCornerParts Corners;

			[[nodiscard]] inline operator bool() const noexcept
			{
				return Sides || Corners;
			}
		};

		struct SkinParts
		{
			SkinPart Center;
			SkinBorderParts Border;

			[[nodiscard]] inline operator bool() const noexcept
			{
				return Center || Border;
			}
		};


		using SkinPartMap = adaptors::FlatMap<std::string, SkinPart>;
		using SkinTextPartMap = adaptors::FlatMap<std::string, SkinTextPart>;

		using SkinMaker = events::Callback<OwningPtr<controls::gui_control::ControlSkin>, const SkinPartMap&, const SkinTextPartMap&, graphics::scene::SceneManager&>;
		using SkinMakerMap = adaptors::FlatMap<std::string, SkinMaker>;


		namespace detail
		{
			controls::gui_control::ControlSkin make_control_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager);

			OwningPtr<controls::gui_control::ControlSkin> make_button_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager);
			OwningPtr<controls::gui_control::ControlSkin> make_check_box_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager);
			OwningPtr<controls::gui_control::ControlSkin> make_group_box_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager);
			OwningPtr<controls::gui_control::ControlSkin> make_label_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager);
			OwningPtr<controls::gui_control::ControlSkin> make_list_box_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager);
			OwningPtr<controls::gui_control::ControlSkin> make_progress_bar_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager);
			OwningPtr<controls::gui_control::ControlSkin> make_radio_button_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager);
			OwningPtr<controls::gui_control::ControlSkin> make_scroll_bar_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager);
			OwningPtr<controls::gui_control::ControlSkin> make_slider_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager);
			OwningPtr<controls::gui_control::ControlSkin> make_text_box_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager);
			OwningPtr<controls::gui_control::ControlSkin> make_tooltip_skin(const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager);
		} //detail


		class SkinFactory final : types::Singleton<SkinFactory>
		{
			private:

				SkinMakerMap skin_makers_;

			public:

				static void Register(std::string name, SkinMaker skin_maker);
				static OwningPtr<controls::gui_control::ControlSkin> Make(std::string_view name,
					const SkinPartMap &parts, const SkinTextPartMap &text_parts, graphics::scene::SceneManager &scene_manager);
		};
	} //gui_skin


	class GuiSkin final : public managed::ManagedObject<GuiTheme>
	{
		private:

			gui_skin::SkinPartMap parts_;
			gui_skin::SkinTextPartMap text_parts_;

		public:

			//Construct a skin with the given name
			explicit GuiSkin(std::string name);
			
			//Construct a skin with the given name, parts and caption
			GuiSkin(std::string name, const gui_skin::SkinParts &parts, const gui_skin::SkinTextPart &caption = {});

			//Construct a skin with the given name, border parts and caption
			GuiSkin(std::string name, const gui_skin::SkinBorderParts &border_parts, const gui_skin::SkinTextPart &caption = {});

			//Construct a skin with the given name, side parts and caption
			GuiSkin(std::string name, const gui_skin::SkinSideParts &side_parts, const gui_skin::SkinTextPart &caption = {});

			//Construct a skin with the given name, center part and caption
			GuiSkin(std::string name, const gui_skin::SkinPart &center, const gui_skin::SkinTextPart &caption = {});

			//Construct a skin with the given name and caption
			GuiSkin(std::string name, const gui_skin::SkinTextPart &caption);


			/*
				Ranges
			*/

			//Returns a mutable range of all parts in this skin
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Parts() noexcept
			{
				return parts_.Elements();
			}

			//Returns an immutable range of all parts in this skin
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Parts() const noexcept
			{
				return parts_.Elements();
			}


			//Returns a mutable range of all parts in this skin
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto TextParts() noexcept
			{
				return text_parts_.Elements();
			}

			//Returns an immutable range of all parts in this skin
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto TextParts() const noexcept
			{
				return text_parts_.Elements();
			}
			

			/*
				Modifiers
			*/




			/*
				Observers
			*/




			/*
				Instantiating
			*/

			//Returns an instantiation of this skin
			[[nodiscard]] OwningPtr<controls::gui_control::ControlSkin> Instantiate() const;


			/*
				Parts
				Adding
			*/

			//Adds a part to this skin with the given name
			void AddPart(std::string name, const gui_skin::SkinPart &part);

			//Adds a text part to this skin with the given name
			void AddTextPart(std::string name, const gui_skin::SkinTextPart &text_part);


			/*
				Parts
				Retrieving
			*/

			//Gets a copy of the part with the given name
			gui_skin::SkinPart GetPart(std::string_view name) const;

			//Gets a copy of the text part with the given name
			gui_skin::SkinTextPart GetTextPart(std::string_view name) const;


			/*
				Parts
				Removing
			*/

			//Clear all parts from this skin
			void ClearParts() noexcept;

			//Remove a part with the given name from this skin
			bool RemovePart(std::string_view name) noexcept;


			//Clear all text parts from this skin
			void ClearTextParts() noexcept;

			//Remove a text part with the given name from this skin
			bool RemoveTextPart(std::string_view name) noexcept;
	};
} //ion::gui::skins

#endif