/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui
File:	IonGuiFrame.h
-------------------------------------------
*/

#ifndef ION_GUI_FRAME_H
#define ION_GUI_FRAME_H

#include <string>
#include <string_view>

#include "IonGuiPanel.h"
#include "IonGuiPanelContainer.h"
#include "controls/IonGuiControl.h"
#include "events/listeners/IonKeyListener.h"
#include "events/listeners/IonMouseListener.h"
#include "graphics/utilities/IonVector2.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::gui
{
	using namespace events::listeners;
	using namespace ion::graphics::utilities;

	class GuiController; //Forward declaration

	namespace gui_frame::detail
	{
	} //gui_frame::detail


	class GuiFrame : public GuiPanelContainer
	{
		private:



		protected:

			/*
				Events
			*/

			//See GuiPanelContainer::Enabled for more details
			virtual void Enabled(GuiComponent &component) noexcept override;

			//See GuiPanelContainer::Disabled for more details
			virtual void Disabled(GuiComponent &component) noexcept override;


			//See GuiPanelContainer::Focused for more details
			virtual void Focused(controls::GuiControl &control) noexcept override;

			//See GuiPanelContainer::Defocused for more details
			virtual void Defocused(controls::GuiControl &control) noexcept override;

		public:

			//Construct a frame with the given name
			GuiFrame(std::string name);


			/*
				Modifiers
			*/




			/*
				Observers
			*/

			//Returns a pointer to the owner of this frame
			[[nodiscard]] GuiController* Owner() const noexcept;


			/*
				Frame events
			*/

			//Called from gui controller when a frame has started
			virtual void FrameStarted(duration time) noexcept;

			//Called from gui controller when a frame has ended
			virtual void FrameEnded(duration time) noexcept;


			/*
				Key events
			*/

			//Called from gui controller when a key button has been pressed
			virtual void KeyPressed(KeyButton button) noexcept;

			//Called from gui controller when a key button has been released
			virtual void KeyReleased(KeyButton button) noexcept;

			//Called from gui controller when a character has been pressed
			virtual void CharacterPressed(char character) noexcept;


			/*
				Mouse events
			*/

			//Called from gui controller when the mouse button has been pressed
			virtual void MousePressed(MouseButton button, Vector2 position) noexcept;

			//Called from gui controller when the mouse button has been released
			virtual void MouseReleased(MouseButton button, Vector2 position) noexcept;

			//Called from gui controller when the mouse has been moved
			virtual void MouseMoved(Vector2 position) noexcept;

			//Called from gui controller when the mouse wheel has been rolled
			virtual void MouseWheelRolled(int delta, Vector2 position) noexcept;
	};
} //ion::gui

#endif