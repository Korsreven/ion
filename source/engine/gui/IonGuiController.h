/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui
File:	IonGuiController.h
-------------------------------------------
*/

#ifndef ION_GUI_CONTROLLER_H
#define ION_GUI_CONTROLLER_H

#include <string>
#include <string_view>
#include <vector>

#include "IonGuiContainer.h"
#include "IonGuiFrame.h"
#include "events/listeners/IonKeyListener.h"
#include "events/listeners/IonMouseListener.h"
#include "events/listeners/IonWindowListener.h"
#include "graphics/utilities/IonVector2.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

//Forward declarations
namespace ion::graphics::scene::graph
{
	class SceneNode;
}

namespace ion::gui
{
	using namespace events::listeners;
	using namespace graphics::scene::graph;
	using namespace ion::graphics::utilities;

	namespace gui_controller::detail
	{
		using frame_pointers = std::vector<GuiFrame*>;
		using active_frames = std::vector<frame_pointers>;
			//Only the active frames at the top (back) of the stack are enabled
			//The rest of the active frames in the stack are disabled (but visible)
	} //gui_controller::detail


	class GuiController final : public GuiContainer
	{
		private:

			gui_controller::detail::active_frames active_frames_;

		public:

			//Construct a gui controller with the given parent node
			GuiController(SceneNode &parent_node);


			/*
				Ranges
			*/

			//Returns a mutable range of all frames in this controller
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Frames() noexcept
			{
				return Components();
			}

			//Returns an immutable range of all frames in this controller
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Frames() const noexcept
			{
				return Components();
			}


			/*
				Modifiers
			*/




			/*
				Observers
			*/




			/*
				Frame events
			*/

			//Typically called from user code when a frame has started
			void FrameStarted(duration time) noexcept;

			//Typically called from user code when a frame has ended
			void FrameEnded(duration time) noexcept;


			/*
				Key events
			*/

			//Typically called from user code when a key button has been pressed
			void KeyPressed(KeyButton button) noexcept;

			//Typically called from user code when a key button has been released
			void KeyReleased(KeyButton button) noexcept;

			//Typically called from user code when a character has been pressed
			void CharacterPressed(char character) noexcept;


			/*
				Mouse events
			*/

			//Typically called from user code when the mouse button has been pressed
			void MousePressed(MouseButton button, Vector2 position) noexcept;

			//Typically called from user code when the mouse button has been released
			void MouseReleased(MouseButton button, Vector2 position) noexcept;

			//Typically called from user code when the mouse has been moved
			void MouseMoved(Vector2 position) noexcept;

			//Typically called from user code when the mouse wheel has been rolled
			void MouseWheelRolled(int delta, Vector2 position) noexcept;


			/*
				Window events
			*/

			//Typically called from user code when a window action has been received
			void WindowActionReceived(WindowAction action) noexcept;


			/*
				Frames
				Creating
			*/

			//Create a frame with the given name
			NonOwningPtr<GuiFrame> CreateFrame(std::string name);

			//Create a frame by moving the given frame
			NonOwningPtr<GuiFrame> CreateFrame(GuiFrame &&frame);


			/*
				Frames
				Retrieving
			*/

			//Gets a pointer to a mutable frame with the given name
			//Returns nullptr if frame could not be found
			[[nodiscard]] NonOwningPtr<GuiFrame> GetFrame(std::string_view name) noexcept;

			//Gets a pointer to an immutable frame with the given name
			//Returns nullptr if frame could not be found
			[[nodiscard]] NonOwningPtr<const GuiFrame> GetFrame(std::string_view name) const noexcept;


			/*
				Frames
				Removing
			*/

			//Clear all removable frames from this controller
			void ClearFrames() noexcept;

			//Remove a removable frame from this controller
			bool RemoveFrame(GuiFrame &frame) noexcept;

			//Remove a removable frame with the given name from this controller
			bool RemoveFrame(std::string_view name) noexcept;
	};
} //ion::gui

#endif