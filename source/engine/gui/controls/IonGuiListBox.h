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
#include "adaptors/ranges/IonIterable.h"
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


		struct ListBoxItem final
		{
			std::string Content;
			NonOwningPtr<graphics::materials::Material> Icon;
		};

		using ListBoxItems = std::vector<ListBoxItem>;


		struct ListBoxSkin : gui_control::ControlSkin
		{
			gui_control::ControlCaptionPart Lines;
			gui_control::ControlVisualPart Selection;		

			std::vector<NonOwningPtr<graphics::scene::shapes::Sprite>> Icons;
				//Sprites for each visible icon
		};


		namespace detail
		{
			constexpr auto default_item_height_factor = 2.0_r;
			constexpr auto default_item_padding_size = 2.0_r;

			constexpr auto default_icon_column_width_percent = 0.25_r;
			constexpr auto default_icon_margin = 2.0_r;


			inline auto item_alignment_to_text_alignment(ListBoxItemAlignment item_alignment) noexcept
			{
				switch (item_alignment)
				{
					case ListBoxItemAlignment::Left:
					return graphics::fonts::text::TextAlignment::Left;

					case ListBoxItemAlignment::Right:
					return graphics::fonts::text::TextAlignment::Right;

					default:
					return graphics::fonts::text::TextAlignment::Center;
				}
			}


			/*
				Skins
			*/

			void resize_skin(ListBoxSkin &skin, const Vector2 &from_size, const Vector2 &to_size) noexcept;


			/*
				Lines
			*/

			std::string item_content_to_text_content(const gui_list_box::ListBoxItems &items);

			Vector2 lines_offset(ListBoxItemAlignment item_alignment, const Vector2 &size, const Vector2 &border_size) noexcept;
			Vector2 lines_area_offset(ListBoxItemAlignment item_alignment, const Vector2 &size, const Vector2 &border_size) noexcept;
		} //detail
	} //gui_list_box


	class GuiListBox : public GuiScrollable
	{
		private:

			void DefaultSetup() noexcept;

		protected:
		
			std::optional<int> item_index_;
			std::optional<real> item_height_factor_;
			std::optional<Vector2> item_padding_;
			gui_list_box::ListBoxItemAlignment item_alignment_ = gui_list_box::ListBoxItemAlignment::Left;
			
			std::optional<real> icon_column_width_;
			std::optional<real> icon_margin_;
			gui_list_box::ListBoxIconLayout icon_layout_ = gui_list_box::ListBoxIconLayout::Left;
			bool show_icons_ = true;

			gui_list_box::ListBoxItems items_;


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

			virtual void AttachSkin() override;
			virtual void DetachSkin() noexcept override;
			virtual void RemoveSkin() noexcept override;
			
			virtual void UpdateLines() noexcept;
			virtual void UpdateSelection() noexcept;		


			/*
				Lines
			*/

			void InsertLines(int off, const gui_list_box::ListBoxItems &items);
			void ReplaceLines(int first, int last, const gui_list_box::ListBoxItems &items);
			void RemoveLines(int first, int last) noexcept;
			void ClearLines() noexcept;

		public:

			//Construct a list box with the given name, caption, tooltip, skin and hit boxes
			GuiListBox(std::string name, std::optional<std::string> caption,
				OwningPtr<gui_list_box::ListBoxSkin> skin, gui_control::BoundingBoxes hit_boxes = {});

			//Construct a list box with the given name, caption, tooltip, skin, size and hit boxes
			GuiListBox(std::string name, std::optional<std::string> caption,
				OwningPtr<gui_list_box::ListBoxSkin> skin, const Vector2 &size, gui_control::BoundingBoxes hit_boxes = {});


			/*
				Ranges
			*/

			//Returns a mutable range of all items in this list box
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Items() noexcept
			{
				return adaptors::ranges::Iterable<gui_list_box::ListBoxItems&>{items_};
			}

			//Returns an immutable range of all items in this list box
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Items() const noexcept
			{
				return adaptors::ranges::Iterable<const gui_list_box::ListBoxItems&>{items_};
			}


			/*
				Modifiers
			*/

			//Sets the item index of this list box to the given index
			inline void ItemIndex(std::optional<int> index) noexcept
			{
				if (item_index_ != index)
				{
					item_index_ = index;
					UpdateSelection();
				}
			}

			//Sets the item height factor for this list box to the given factor
			inline void ItemHeightFactor(std::optional<real> factor) noexcept
			{
				if (item_height_factor_ != factor)
				{
					item_height_factor_ = factor;
					UpdateLines();
				}
			}

			//Sets the item padding for this list box to the given padding
			inline void ItemPadding(const std::optional<Vector2> &padding) noexcept
			{
				if (item_padding_ != padding)
				{
					item_padding_ = padding;
					UpdateLines();
				}
			}

			//Sets the item alignment for this list box to the given alignment
			inline void ItemAlignment(gui_list_box::ListBoxItemAlignment alignment) noexcept
			{
				if (item_alignment_ != alignment)
				{
					item_alignment_ = alignment;
					UpdateLines();
				}
			}


			//Sets the icon column width for this list box to the given width (in percentages)
			inline void IconColumnWidth(std::optional<real> percent) noexcept
			{
				if (icon_column_width_ != percent)
				{
					icon_column_width_ = percent;
					UpdateLines();
				}
			}

			//Sets the icon margin for this list box to the given margin
			inline void IconMargin(std::optional<real> margin) noexcept
			{
				if (icon_margin_ != margin)
				{
					icon_margin_ = margin;
					UpdateLines();
				}
			}

			//Sets the icon layout for this list box to the given layout
			inline void IconLayout(gui_list_box::ListBoxIconLayout layout) noexcept
			{
				if (icon_layout_ != layout)
				{
					icon_layout_ = layout;
					UpdateLines();
				}
			}

			//Sets whether or not this list box is showing icons
			inline void ShowIcons(bool show) noexcept
			{
				if (show_icons_ != show)
				{
					show_icons_ = show;
					UpdateLines();
				}
			}


			/*
				Observers
			*/

			//Returns the item index of this list box
			//Returns nullopt if no items are selected
			[[nodiscard]] inline auto ItemIndex() const noexcept
			{
				return item_index_;
			}

			//Returns the item height factor for this list box
			//Returns nullopt if no custom item height factor has been set
			[[nodiscard]] inline auto ItemHeightFactor() const noexcept
			{
				return item_height_factor_;
			}

			//Returns the item padding for this list box
			//Returns nullopt if no custom item padding has been set
			[[nodiscard]] inline auto ItemPadding() const noexcept
			{
				return item_padding_;
			}

			//Returns the icon layout for this list box
			[[nodiscard]] inline auto ItemAlignment() const noexcept
			{
				return item_alignment_;
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

			//Returns the icon layout for this list box
			[[nodiscard]] inline auto IconLayout() const noexcept
			{
				return icon_layout_;
			}

			//Returns true if this list box is showing icons
			[[nodiscard]] inline auto ShowIcons() const noexcept
			{
				return show_icons_;
			}


			/*
				Items
				Adding/inserting
			*/

			//Adds an item to the list box with the given content and icon
			void AddItem(std::string content, NonOwningPtr<graphics::materials::Material> icon = nullptr);

			//Adds the given item to the list box
			void AddItem(gui_list_box::ListBoxItem item);

			//Adds the given items to the list box
			void AddItems(gui_list_box::ListBoxItems items);


			//Inserts an item to the list box with the given content and icon, at the given offset
			void InsertItem(int off, std::string content, NonOwningPtr<graphics::materials::Material> icon = nullptr);

			//Inserts the given item to the list box, at the given offset
			void InsertItem(int off, gui_list_box::ListBoxItem item);

			//Inserts the given items to the list box, at the given offset
			void InsertItems(int off, gui_list_box::ListBoxItems items);


			/*
				Items
				Replacing
			*/

			//Replaces the item at the given offset, with the given content and icon
			void ReplaceItem(int off, std::string content, NonOwningPtr<graphics::materials::Material> icon = nullptr);

			//Replaces the item at the given offset, with the given item
			void ReplaceItem(int off, gui_list_box::ListBoxItem item);

			//Replaces the item at the given offset, with the given items
			void ReplaceItem(int off, gui_list_box::ListBoxItems items);


			//Replaces the items in range [first, last), with the given content and icon
			void ReplaceItems(int first, int last, std::string content, NonOwningPtr<graphics::materials::Material> icon = nullptr);

			//Replaces the items in range [first, last), with the given item
			void ReplaceItems(int first, int last, gui_list_box::ListBoxItem item);

			//Replaces the items in range [first, last), with the given items
			void ReplaceItems(int first, int last, gui_list_box::ListBoxItems items);


			/*
				Items
				Removing
			*/

			//Clears all items from this list box
			void ClearItems() noexcept;

			//Removes the item at the given offset from this list box
			void RemoveItem(int off) noexcept;

			//Removes all items in range [first, last) from this list box
			void RemoveItems(int first, int last) noexcept;


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