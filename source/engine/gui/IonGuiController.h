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

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "IonGuiContainer.h"
#include "IonGuiFrame.h"
#include "events/IonListenable.h"
#include "events/listeners/IonGuiFrameListener.h"
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

		struct layer
		{
			GuiFrame *current_frame = nullptr;
			frame_pointers frames;		
		};

		using frames = std::vector<layer>;
			//Only the active frames at the top (back) of the stack are interactive
			//The rest of the active frames in the stack are non-interactive (but visible)


		bool is_frame_on_top(const GuiFrame &frame, const frames &frames) noexcept;
		bool is_frame_activated(const GuiFrame &frame, const frames &frames) noexcept;

		void activate_frame(GuiFrame &frame, frames &to_frames, bool modal) noexcept;
		void deactivate_frame(GuiFrame &frame, frames &from_frames) noexcept;


		std::optional<frame_pointers::iterator> get_current_frame_iterator(frames &frames) noexcept;

		inline auto get_next_frame_iterator(frame_pointers::iterator iter, frame_pointers &frames) noexcept
		{
			return iter != std::end(frames) ? iter + 1 : std::begin(frames);
		}

		inline auto get_previous_frame_iterator(frame_pointers::iterator iter, frame_pointers &frames) noexcept
		{
			return iter != std::begin(frames) ? iter - 1 : std::end(frames) - 1;
		}
	} //gui_controller::detail


	class GuiController final :
		public GuiContainer,
		public events::Listenable<events::listeners::GuiFrameListener>,
		public events::listeners::GuiFrameListener
	{
		private:

			using FrameEventsBase = events::Listenable<events::listeners::GuiFrameListener>;
			using ManagedObjectEventsBase = events::Listenable<events::listeners::ManagedObjectListener<GuiComponent, GuiContainer>>;

			
			GuiFrame *focused_frame_ = nullptr;
			gui_controller::detail::frames active_frames_;

			/*
				Events
			*/

			//See ObjectManager::Created for more details
			void Created(GuiComponent &component) noexcept override final;
			void Created(GuiFrame &frame) noexcept;

			//See ObjectManager::Removed for more details
			void Removed(GuiComponent &component) noexcept override final;
			void Removed(GuiFrame &frame) noexcept;


			//See Listener<T>::Unsubscribable for more details
			//Make sure that if this gui frame listener is about to unsubscribe from the gui controller, cancel it
			bool Unsubscribable(Listenable<events::listeners::GuiFrameListener>&) noexcept override final;


			//See GuiFrameListener::Enabled for more details
			void Enabled(GuiFrame &frame) noexcept override final;

			//See GuiFrameListener::Disabled for more details
			void Disabled(GuiFrame &frame) noexcept override final;


			//See GuiFrameListener::Activated for more details
			void Activated(GuiFrame &frame) noexcept override final;

			//See GuiFrameListener::Deactivated for more details
			void Deactivated(GuiFrame &frame) noexcept override final;


			//See GuiFrameListener::Focused for more details
			void Focused(GuiFrame &frame) noexcept override final;

			//See GuiFrameListener::Defocused for more details
			void Defocused(GuiFrame &frame) noexcept override final;

		public:

			//Construct a gui controller with the given parent node
			GuiController(SceneNode &parent_node);


			/*
				Events
			*/

			//Return a mutable reference to the frame events of this controller
			[[nodiscard]] inline auto& FrameEvents() noexcept
			{
				return static_cast<FrameEventsBase&>(*this);
			}

			//Return a immutable reference to the frame events of this controller
			[[nodiscard]] inline auto& FrameEvents() const noexcept
			{
				return static_cast<const FrameEventsBase&>(*this);
			}


			//Return a mutable reference to the managed object events of this controller
			[[nodiscard]] inline auto& ManagedObjectEvents() noexcept
			{
				return static_cast<ManagedObjectEventsBase&>(*this);
			}

			//Return a immutable reference to the managed object events of this controller
			[[nodiscard]] inline auto& ManagedObjectEvents() const noexcept
			{
				return static_cast<const ManagedObjectEventsBase&>(*this);
			}


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

			//Returns true if the given frame is on top
			[[nodiscard]] bool IsOnTop(const GuiFrame &frame) const noexcept;


			//Returns a pointer to the current focused frame in this controller
			//Returns nullptr if there is no currently focused frame
			[[nodiscard]] inline auto FocusedFrame() const noexcept
			{
				return focused_frame_;
			}


			/*
				Tabulating
			*/

			//Focuses the next focusable frame in this controller
			void TabForward() noexcept;

			//Focuses the previous focusable frame in this controller
			void TabBackward() noexcept;


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