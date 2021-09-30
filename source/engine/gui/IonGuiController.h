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

#include "IonGuiFrame.h"
#include "events/listeners/IonKeyListener.h"
#include "events/listeners/IonMouseListener.h"
#include "events/listeners/IonWindowListener.h"
#include "graphics/utilities/IonVector2.h"
#include "managed/IonObjectManager.h"
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
	} //gui_controller::detail


	class GuiController final : public managed::ObjectManager<GuiFrame, GuiController>
	{
		private:

			NonOwningPtr<SceneNode> node_;

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
				return Objects();
			}

			//Returns an immutable range of all cameras in this controller
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Frames() const noexcept
			{
				return Objects();
			}


			/*
				Modifiers
			*/




			/*
				Observers
			*/

			//Returns a pointer to the node for this controller
			//This is the top-level node for the controlled by this
			[[nodiscard]] inline auto Node() const noexcept
			{
				return node_;
			}


			/*
				Frame events
			*/

			//Typically called from user code when a frame has started
			bool FrameStarted(duration time) noexcept;

			//Typically called from user code when a frame has ended
			bool FrameEnded(duration time) noexcept;


			/*
				Window events
			*/

			//Typically called from user code when a window action has been received
			void WindowActionReceived(WindowAction action) noexcept;


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