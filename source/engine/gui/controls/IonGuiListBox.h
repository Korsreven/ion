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
#include "events/IonCallback.h"
#include "events/listeners/IonKeyListener.h"
#include "events/listeners/IonMouseListener.h"
#include "graphics/fonts/IonText.h"
#include "graphics/utilities/IonVector2.h"
#include "memory/IonNonOwningPtr.h"
#include "memory/IonOwningPtr.h"
#include "timers/IonStopwatch.h"
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
		enum class ListBoxItemLayout
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
			gui_control::ControlSkinTextPart Lines;
			gui_control::ControlSkinPart Selection;

			std::vector<NonOwningPtr<graphics::scene::shapes::Sprite>> Icons;
				//Sprites for each visible icon

			//Copy from most derived
			virtual void Assign(const ControlSkin &control_skin) noexcept
			{
				if (auto skin = dynamic_cast<const ListBoxSkin*>(&control_skin); skin)
					*this = *skin;
				else //Try less derived
					return ControlSkin::Assign(control_skin);
			}
		};


		namespace detail
		{
			constexpr auto default_item_height_factor = 2.0_r;
			constexpr auto default_item_padding_size = 2.0_r;
			constexpr auto default_selection_padding_size = 0.0_r;

			constexpr auto default_icon_column_width_percent = 0.25_r;
			constexpr auto default_icon_padding_size = 2.0_r;

			constexpr auto default_double_click_time = 0.5_sec;


			inline auto item_layout_to_text_alignment(ListBoxItemLayout item_alignment) noexcept
			{
				switch (item_alignment)
				{
					case ListBoxItemLayout::Left:
					return graphics::fonts::text::TextAlignment::Left;

					case ListBoxItemLayout::Right:
					return graphics::fonts::text::TextAlignment::Right;

					default:
					return graphics::fonts::text::TextAlignment::Center;
				}
			}


			/**
				@name Skins
				@{
			*/

			void resize_skin(ListBoxSkin &skin, const Vector2 &from_size, const Vector2 &to_size) noexcept;

			///@}

			/**
				@name Items/lines
				@{
			*/

			void trim_item(gui_list_box::ListBoxItem &item) noexcept;
			void trim_items(gui_list_box::ListBoxItems &items) noexcept;			
			std::string items_to_text_content(const gui_list_box::ListBoxItems &items);

			Vector2 lines_area_offset(ListBoxIconLayout icon_layout, const Vector2 &icon_column_size) noexcept;

			///@}
		} //detail
	} //gui_list_box


	///@brief A class representing a GUI list box with items that can be left, center or right aligned
	///@details List box items can have associated icons shown, that are either left or right aligned
	class GuiListBox : public GuiScrollable
	{
		private:

			void DefaultSetup() noexcept;

		protected:
		
			std::optional<int> item_index_;
			std::optional<real> item_height_factor_;
			std::optional<Vector2> item_padding_;
			gui_list_box::ListBoxItemLayout item_layout_ = gui_list_box::ListBoxItemLayout::Left;
			std::optional<Vector2> selection_padding_;
			
			std::optional<real> icon_column_width_;
			std::optional<Vector2> icon_max_size_;
			std::optional<Vector2> icon_padding_;
			gui_list_box::ListBoxIconLayout icon_layout_ = gui_list_box::ListBoxIconLayout::Left;
			bool show_icons_ = false;

			gui_list_box::ListBoxItems items_;
			std::optional<int> previous_item_index_;
			timers::Stopwatch item_click_time_;

			std::optional<events::Callback<void, GuiListBox&>> on_item_double_click_;


			/**
				@name Events
				@{
			*/

			///@brief See GuiControl::Resized for more details
			virtual void Resized(Vector2 from_size, Vector2 to_size) noexcept override;


			///@brief See GuiScrollable::Scrolled for more details
			virtual void Scrolled(int delta) noexcept override;

			///@brief See GuiScrollable::TotalElements for more details
			virtual int TotalElements() noexcept override;

			///@brief See GuiScrollable::ElementsInView for more details
			virtual int ElementsInView() noexcept override;

			///@brief See GuiScrollable::ScrollPosition for more details
			virtual int ScrollPosition() noexcept override;
			

			///@brief Called right after an item has been selected
			virtual void ItemSelected() noexcept;

			///@brief Called right after an item has been deselected
			virtual void ItemDeselected() noexcept;


			///@brief Called right after an item has been double clicked
			virtual void ItemDoubleClicked() noexcept;

			///@}

			/**
				@name States
				@{
			*/

			void SetSkinState(gui_control::ControlState state, gui_list_box::ListBoxSkin &skin) noexcept;
			virtual void SetState(gui_control::ControlState state) noexcept override;

			///@}

			/**
				@name Skins
				@{
			*/

			virtual OwningPtr<gui_control::ControlSkin> AttuneSkin(OwningPtr<gui_control::ControlSkin> skin) const override;

			virtual void AttachSkin() override;
			virtual void DetachSkin() noexcept override;
			virtual void RemoveSkin() noexcept override;
			
			virtual void UpdateLines() noexcept;
			virtual void UpdateIcons() noexcept;
			virtual void UpdateSelection() noexcept;

			///@}

			/**
				@name Lines
				@{
			*/

			void InsertLines(int off, const gui_list_box::ListBoxItems &items);
			void ReplaceLines(int first, int last, const gui_list_box::ListBoxItems &items);
			void RemoveLines(int first, int last) noexcept;
			void ClearLines() noexcept;

			///@}

			/**
				@name Icons
				@{
			*/

			NonOwningPtr<graphics::scene::shapes::Sprite> CreateIcon(NonOwningPtr<graphics::materials::Material> material);
			void RemoveIcons() noexcept;

			///@}

		public:

			///@brief Constructs a list box with the given name, size, caption, tooltip and hit boxes
			GuiListBox(std::string name, const std::optional<Vector2> &size,
				std::optional<std::string> caption, gui_control::BoundingBoxes hit_boxes = {}) noexcept;

			///@brief Constructs a list box with the given name, skin, size, caption, tooltip and hit boxes
			GuiListBox(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
				std::optional<std::string> caption, gui_control::BoundingBoxes hit_boxes = {});


			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all items in this list box
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Items() noexcept
			{
				return adaptors::ranges::Iterable<gui_list_box::ListBoxItems&>{items_};
			}

			///@brief Returns an immutable range of all items in this list box
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Items() const noexcept
			{
				return adaptors::ranges::Iterable<const gui_list_box::ListBoxItems&>{items_};
			}

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the item index of this list box to the given index
			void ItemIndex(std::optional<int> index) noexcept;

			///@brief Sets the item height factor for this list box to the given factor
			inline void ItemHeightFactor(std::optional<real> factor) noexcept
			{
				if (item_height_factor_ != factor)
				{
					item_height_factor_ = factor;
					UpdateLines();
				}
			}

			///@brief Sets the item padding for this list box to the given padding
			inline void ItemPadding(const std::optional<Vector2> &padding) noexcept
			{
				if (item_padding_ != padding)
				{
					item_padding_ = padding;
					UpdateLines();
				}
			}

			///@brief Sets the item layout for this list box to the given layout
			inline void ItemLayout(gui_list_box::ListBoxItemLayout layout) noexcept
			{
				if (item_layout_ != layout)
				{
					item_layout_ = layout;
					UpdateLines();
				}
			}

			///@brief Sets the selection padding for this list box to the given padding
			inline void SelectionPadding(const std::optional<Vector2> &padding) noexcept
			{
				if (selection_padding_ != padding)
				{
					selection_padding_ = padding;
					UpdateSelection();
				}
			}


			///@brief Sets the icon column width for this list box to the given width (in percentages)
			inline void IconColumnWidth(std::optional<real> percent) noexcept
			{
				if (icon_column_width_ != percent)
				{
					icon_column_width_ = percent;

					if (show_icons_)
						UpdateLines();
				}
			}

			///@brief Sets the icon max size for this list box to the given size
			inline void IconMaxSize(const std::optional<Vector2> &size) noexcept
			{
				if (icon_max_size_ != size)
				{
					icon_max_size_ = size;

					if (show_icons_)
						UpdateIcons();
				}
			}

			///@brief Sets the icon padding for this list box to the given padding
			inline void IconPadding(const std::optional<Vector2> &padding) noexcept
			{
				if (icon_padding_ != padding)
				{
					icon_padding_ = padding;

					if (show_icons_)
						UpdateIcons();
				}
			}

			///@brief Sets the icon layout for this list box to the given layout
			inline void IconLayout(gui_list_box::ListBoxIconLayout layout) noexcept
			{
				if (icon_layout_ != layout)
				{
					icon_layout_ = layout;

					if (show_icons_)
						UpdateLines();
				}
			}

			///@brief Sets whether or not this list box is showing icons
			inline void ShowIcons(bool show) noexcept
			{
				if (show_icons_ != show)
				{
					show_icons_ = show;
					UpdateLines();
				}
			}


			///@brief Sets the on item double click callback
			inline void OnItemDoubleClick(events::Callback<void, GuiListBox&> on_item_double_click) noexcept
			{
				on_item_double_click_ = on_item_double_click;
			}

			///@brief Sets the on item double click callback
			inline void OnItemDoubleClick(std::nullopt_t) noexcept
			{
				on_item_double_click_ = {};
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the item index of this list box
			///@details Returns nullopt if no items are selected
			[[nodiscard]] inline auto ItemIndex() const noexcept
			{
				return item_index_;
			}

			///@brief Returns the item height factor for this list box
			///@details Returns nullopt if no custom item height factor has been set
			[[nodiscard]] inline auto ItemHeightFactor() const noexcept
			{
				return item_height_factor_;
			}

			///@brief Returns the item padding for this list box
			///@details Returns nullopt if no custom item padding has been set
			[[nodiscard]] inline auto ItemPadding() const noexcept
			{
				return item_padding_;
			}

			///@brief Returns the item layout for this list box
			[[nodiscard]] inline auto ItemLayout() const noexcept
			{
				return item_layout_;
			}

			///@brief Returns the selection padding for this list box
			///@details Returns nullopt if no custom selection padding has been set
			[[nodiscard]] inline auto SelectionPadding() const noexcept
			{
				return selection_padding_;
			}


			///@brief Returns the icon column width for this list box (in percentages)
			///@details Returns nullopt if no custom icon column width has been set
			[[nodiscard]] inline auto IconColumnWidth() const noexcept
			{
				return icon_column_width_;
			}

			///@brief Returns the icon max size for this list box
			///@details Returns nullopt if no icon max size has been set
			[[nodiscard]] inline auto IconMaxSize() const noexcept
			{
				return icon_max_size_;
			}

			///@brief Returns the icon padding for this list box
			///@details Returns nullopt if no custom icon padding has been set
			[[nodiscard]] inline auto IconPadding() const noexcept
			{
				return icon_padding_;
			}

			///@brief Returns the icon layout for this list box
			[[nodiscard]] inline auto IconLayout() const noexcept
			{
				return icon_layout_;
			}

			///@brief Returns true if this list box is showing icons
			[[nodiscard]] inline auto ShowIcons() const noexcept
			{
				return show_icons_;
			}


			///@brief Returns the on item double click callback
			[[nodiscard]] inline auto OnItemDoubleClick() const noexcept
			{
				return on_item_double_click_;
			}

			///@}

			/**
				@name Items - Adding/inserting
				@{
			*/

			///@brief Adds an item to the list box with the given content and icon
			void AddItem(std::string content, NonOwningPtr<graphics::materials::Material> icon = nullptr);

			///@brief Adds the given item to the list box
			void AddItem(gui_list_box::ListBoxItem item);

			///@brief Adds the given items to the list box
			void AddItems(gui_list_box::ListBoxItems items);


			///@brief Inserts an item to the list box with the given content and icon, at the given offset
			void InsertItem(int off, std::string content, NonOwningPtr<graphics::materials::Material> icon = nullptr);

			///@brief Inserts the given item to the list box, at the given offset
			void InsertItem(int off, gui_list_box::ListBoxItem item);

			///@brief Inserts the given items to the list box, at the given offset
			void InsertItems(int off, gui_list_box::ListBoxItems items);

			///@}

			/**
				@name Items - Replacing
				@{
			*/

			///@brief Replaces the item at the given offset, with the given content and icon
			void ReplaceItem(int off, std::string content, NonOwningPtr<graphics::materials::Material> icon = nullptr);

			///@brief Replaces the item at the given offset, with the given item
			void ReplaceItem(int off, gui_list_box::ListBoxItem item);

			///@brief Replaces the item at the given offset, with the given items
			void ReplaceItem(int off, gui_list_box::ListBoxItems items);


			///@brief Replaces the items in range [first, last), with the given content and icon
			void ReplaceItems(int first, int last, std::string content, NonOwningPtr<graphics::materials::Material> icon = nullptr);

			///@brief Replaces the items in range [first, last), with the given item
			void ReplaceItems(int first, int last, gui_list_box::ListBoxItem item);

			///@brief Replaces the items in range [first, last), with the given items
			void ReplaceItems(int first, int last, gui_list_box::ListBoxItems items);

			///@}

			/**
				@name Items - Retrieving
				@{
			*/

			///@brief Gets the item with the given item index
			///@details Returns nullopt if item could not be found
			[[nodiscard]] std::optional<gui_list_box::ListBoxItem> GetItem(int item_index) const noexcept;

			///@brief Gets the selected item
			///@details Returns nullopt if no items are selected
			[[nodiscard]] std::optional<gui_list_box::ListBoxItem> GetSelectedItem() const noexcept;

			///@}

			/**
				@name Items - Removing
				@{
			*/

			///@brief Clears all items from this list box
			void ClearItems() noexcept;

			///@brief Removes the item at the given offset from this list box
			void RemoveItem(int off) noexcept;

			///@brief Removes all items in range [first, last) from this list box
			void RemoveItems(int first, int last) noexcept;

			///@}

			/**
				@name Key events
				@{
			*/

			///@brief Called from gui control when a key button has been released
			///@details Returns true if the key release event has been consumed by the control
			virtual bool KeyReleased(KeyButton button) noexcept override;

			///@}

			/**
				@name Mouse events
				@{
			*/

			///@brief Called from gui control when the mouse button has been released
			///@details Returns true if the mouse release event has been consumed by the control
			virtual bool MouseReleased(MouseButton button, Vector2 position) noexcept override;

			///@}
	};

} //ion::gui::controls

#endif