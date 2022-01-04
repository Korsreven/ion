/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiListBox.h
-------------------------------------------
*/

#ifndef ION_GUI_LIST_BOX_H
#define ION_GUI_LIST_BOX_H

#include <optional>
#include <string>
#include <vector>

#include "IonGuiScrollable.h"
#include "events/listeners/IonKeyListener.h"
#include "events/listeners/IonMouseListener.h"
#include "graphics/utilities/IonVector2.h"
#include "memory/IonNonOwningPtr.h"
#include "memory/IonOwningPtr.h"
#include "types/IonTypes.h"

//Forward declarations
namespace ion
{
	namespace graphics
	{
		namespace materials
		{
			class Material;
		}

		namespace scene::shapes
		{
			class Sprite;
		}
	}
}

namespace ion::gui::controls
{
	using namespace events::listeners;
	using namespace graphics::utilities;
	using namespace types::type_literals;

	namespace gui_list_box
	{
		enum class ListBoxItemAlignment
		{
			Left,
			Center,
			Right
		};

		enum class ListBoxIconLayout : bool
		{
			Left,
			Right
		};


		struct ListBoxSkin : gui_control::ControlSkin
		{
			gui_control::ControlVisualPart Selection;
			gui_control::ControlCaptionPart Items;

			std::vector<NonOwningPtr<graphics::scene::shapes::Sprite>> Icons;
				//Sprites for each visible icon
		};


		namespace detail
		{
			struct item final
			{
				std::string content;
				NonOwningPtr<graphics::materials::Material> icon;
			};
			
			using items = std::vector<item>;
			
			constexpr auto default_item_height_factor = 2.0_r;
			constexpr auto default_icon_column_width_percent = 0.25_r;
			constexpr auto default_icon_margin = 2.0_r;


			/*
				Skins
			*/

			void resize_skin(ListBoxSkin &skin, const Vector2 &from_size, const Vector2 &to_size) noexcept;
		} //detail
	} //gui_list_box


	class GuiListBox : public GuiScrollable
	{
		private:

			void DefaultSetup() noexcept;

		protected:

			gui_list_box::ListBoxItemAlignment item_alignment_ = gui_list_box::ListBoxItemAlignment::Left;
			std::optional<real> item_height_factor_;
			std::optional<int> item_index_;

			gui_list_box::ListBoxIconLayout icon_layout_ = gui_list_box::ListBoxIconLayout::Left;
			std::optional<real> icon_column_width_;
			std::optional<real> icon_margin_;
			bool show_icons_ = true;

			gui_list_box::detail::items items_;


			/*
				Events
			*/

			//See GuiControl::Resized for more details
			virtual void Resized(Vector2 from_size, Vector2 to_size) noexcept override;


			//See GuiScrollable::Scrolled for more details
			virtual void Scrolled(int delta) noexcept override;

			//See GuiScrollable::TotalElements for more details
			virtual int TotalElements() noexcept override;

			//See GuiScrollable::ElementsInView for more details
			virtual int ElementsInView() noexcept override;

			//See GuiScrollable::ScrollPosition for more details
			virtual int ScrollPosition() noexcept override;
			

			//Called right after an item has been selected
			virtual void ItemSelected() noexcept;

			//Called right after an item has been deselected
			virtual void ItemDeselected() noexcept;


			/*
				States
			*/

			void SetSkinState(gui_control::ControlState state, gui_list_box::ListBoxSkin &skin) noexcept;
			virtual void SetState(gui_control::ControlState state) noexcept override;


			/*
				Skins
			*/
			
			virtual void UpdateSelection() noexcept;
			virtual void UpdateItems() noexcept;

		public:

			//Construct a list box with the given name, caption, tooltip, skin and hit boxes
			GuiListBox(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
				OwningPtr<gui_list_box::ListBoxSkin> skin, gui_control::BoundingBoxes hit_boxes = {});

			//Construct a list box with the given name, caption, tooltip, skin, size and hit boxes
			GuiListBox(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
				OwningPtr<gui_list_box::ListBoxSkin> skin, const Vector2 &size, gui_control::BoundingBoxes hit_boxes = {});


			/*
				Modifiers
			*/

			//Sets the item alignment for this list box to the given alignment
			inline void ItemAlignment(gui_list_box::ListBoxItemAlignment alignment) noexcept
			{
				if (item_alignment_ != alignment)
				{
					item_alignment_ = alignment;
					UpdateItems();
				}
			}

			//Sets the item height factor for this list box to the given factor
			inline void ItemHeightFactor(std::optional<real> factor) noexcept
			{
				if (item_height_factor_ != factor)
				{
					item_height_factor_ = factor;
					UpdateItems();
				}
			}

			//Sets the item index of this list box to the given index
			inline void ItemIndex(std::optional<int> index) noexcept
			{
				if (item_index_ != index)
				{
					item_index_ = index;
					UpdateSelection();
				}
			}


			//Sets the icon layout for this list box to the given layout
			inline void IconLayout(gui_list_box::ListBoxIconLayout layout) noexcept
			{
				if (icon_layout_ != layout)
				{
					icon_layout_ = layout;
					UpdateItems();
				}
			}

			//Sets the icon column width for this list box to the given width (in percentages)
			inline void IconColumnWidth(std::optional<real> percent) noexcept
			{
				if (icon_column_width_ != percent)
				{
					icon_column_width_ = percent;
					UpdateItems();
				}
			}

			//Sets the icon margin for this list box to the given margin
			inline void IconMargin(std::optional<real> margin) noexcept
			{
				if (icon_margin_ != margin)
				{
					icon_margin_ = margin;
					UpdateItems();
				}
			}

			//Sets whether or not this list box is showing icons
			inline void ShowIcons(bool show) noexcept
			{
				if (show_icons_ != show)
				{
					show_icons_ = show;
					UpdateItems();
				}
			}


			/*
				Observers
			*/

			//Returns the icon layout for this list box
			[[nodiscard]] inline auto ItemAlignment() const noexcept
			{
				return item_alignment_;
			}

			//Returns the item height factor for this list box
			//Returns nullopt if no custom item height factor has been set
			[[nodiscard]] inline auto ItemHeightFactor() const noexcept
			{
				return item_height_factor_;
			}

			//Returns the item index of this list box
			//Returns nullopt if no items are selected
			[[nodiscard]] inline auto ItemIndex() const noexcept
			{
				return item_index_;
			}


			//Returns the icon layout for this list box
			[[nodiscard]] inline auto IconLayout() const noexcept
			{
				return icon_layout_;
			}

			//Returns the icon column width for this list box (in percentages)
			//Returns nullopt if no custom icon column width has been set
			[[nodiscard]] inline auto IconColumnWidth() const noexcept
			{
				return icon_column_width_;
			}

			//Returns the icon margin for this list box
			//Returns nullopt if no custom icon margin has been set
			[[nodiscard]] inline auto IconMargin() const noexcept
			{
				return icon_margin_;
			}

			//Returns true if this list box is showing icons
			[[nodiscard]] inline auto ShowIcons() const noexcept
			{
				return show_icons_;
			}


			/*
				Key events
			*/

			//Called from gui control when a key button has been released
			//Returns true if the key release event has been consumed by the control
			virtual bool KeyReleased(KeyButton button) noexcept override;


			/*
				Mouse events
			*/

			//Called from gui control when the mouse button has been released
			//Returns true if the mouse release event has been consumed by the control
			virtual bool MouseReleased(MouseButton button, Vector2 position) noexcept override;

			//Called from gui control when the mouse wheel has been rolled
			//Returns true if the mouse wheel roll event has been consumed by the control
			virtual bool MouseWheelRolled(int delta, Vector2 position) noexcept override;
	};

} //ion::gui::controls

#endif