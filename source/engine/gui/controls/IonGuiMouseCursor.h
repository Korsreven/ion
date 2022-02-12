/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiMouseCursor.h
-------------------------------------------
*/

#ifndef ION_GUI_MOUSE_CURSOR_H
#define ION_GUI_MOUSE_CURSOR_H

#include <optional>
#include <string>

#include "IonGuiControl.h"
#include "memory/IonOwningPtr.h"

namespace ion::gui::controls
{
	namespace gui_mouse_cursor
	{
		enum class MouseCursorHotSpot
		{
			TopLeft,	TopCenter,		TopRight,
			Left,		Center,			Right,
			BottomLeft, BottomCenter,	BottomRight
		};


		struct MouseCursorSkin : gui_control::ControlSkin
		{
			//Empty
		};


		namespace detail
		{
		} //detail
	} //gui_mouse_cursor


	class GuiMouseCursor : public GuiControl
	{
		private:

			void DefaultSetup() noexcept;

		protected:

			gui_mouse_cursor::MouseCursorHotSpot hot_spot_ = gui_mouse_cursor::MouseCursorHotSpot::TopLeft;


			/*
				Skins
			*/

			virtual OwningPtr<gui_control::ControlSkin> AttuneSkin(OwningPtr<gui_control::ControlSkin> skin) const override;

		public:

			//Construct a mouse cursor with the given name and size
			GuiMouseCursor(std::string name, const std::optional<Vector2> &size);

			//Construct a mouse cursor with the given name, skin and size
			GuiMouseCursor(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size);


			/*
				Modifiers
			*/

			//Sets the hot spot for this mouse cursor to the given hot spot
			//The point on the cursor that interacts with other controls on the screen
			inline void HotSpot(gui_mouse_cursor::MouseCursorHotSpot hot_spot) noexcept
			{
				hot_spot_ = hot_spot;
			}


			/*
				Observers
			*/

			//Returns the hot spot for this mouse cursor
			//The point on the cursor that interacts with other controls on the screen
			[[nodiscard]] inline auto HotSpot() const noexcept
			{
				return hot_spot_;
			}
	};

} //ion::gui::controls

#endif