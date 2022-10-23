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
#include "adaptors/ranges/IonDereferenceIterable.h"
#include "controls/IonGuiMouseCursor.h"
#include "controls/IonGuiTooltip.h"
#include "events/IonListenable.h"
#include "events/listeners/IonGuiFrameListener.h"
#include "events/listeners/IonKeyListener.h"
#include "events/listeners/IonMouseListener.h"
#include "events/listeners/IonWindowListener.h"
#include "graphics/utilities/IonVector2.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"
#include "skins/IonGuiSkin.h"
#include "skins/IonGuiTheme.h"
#include "types/IonTypes.h"

//Forward declarations
namespace ion
{
	namespace graphics
	{
		namespace render
		{
			class Viewport;
		}

		namespace scene
		{
			class SceneManager;

			namespace graph
			{
				class SceneNode;
			}
		}
	}

	namespace sounds
	{
		class SoundChannelGroup;
	}
}

namespace ion::gui
{
	using namespace events::listeners;
	using namespace graphics::scene::graph;
	using namespace graphics::utilities;

	namespace gui_controller::detail
	{
		using frame_pointers = std::vector<GuiFrame*>;
		using mouse_cursor_pointers = std::vector<controls::GuiMouseCursor*>;
		using tooltip_pointers = std::vector<controls::GuiTooltip*>;

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

		void activate_frame(GuiFrame &frame, frames &to_frames) noexcept;
		void deactivate_frame(GuiFrame &frame, frames &from_frames) noexcept;


		std::optional<frame_pointers::const_iterator> get_frame_iterator(const frames &frames, GuiFrame *frame) noexcept;

		inline auto get_next_frame_iterator(frame_pointers::const_iterator iter, const frame_pointers &frames) noexcept
		{
			return iter >= std::end(frames) - 1 ? std::begin(frames) : iter + 1;
		}

		inline auto get_previous_frame_iterator(frame_pointers::const_iterator iter, const frame_pointers &frames) noexcept
		{
			return iter == std::begin(frames) ? std::end(frames) - 1 : iter - 1;
		}
	} //gui_controller::detail


	//A class representing a GUI controller that can create and store multiple GUI frames and GUI themes
	//A controller can also create and store different kinds of mouse cursors and tooltips
	//A controller can be seen as a GUI manager, and is the link between the scene graph and the rest of the GUI system
	class GuiController final :
		public GuiContainer,
		public events::Listenable<events::listeners::GuiFrameListener>,
		public events::listeners::GuiFrameListener,
		public managed::ObjectManager<skins::GuiTheme, GuiController>
	{
		private:

			using FrameEventsBase = events::Listenable<events::listeners::GuiFrameListener>;
			using ManagedObjectEventsBase = events::Listenable<events::listeners::ManagedObjectListener<GuiComponent, GuiContainer>>;
			using ThemeBase = managed::ObjectManager<skins::GuiTheme, GuiController>;

			
			GuiFrame *focused_frame_ = nullptr;
			controls::GuiMouseCursor *active_mouse_cursor_ = nullptr;
			controls::GuiTooltip *active_tooltip_ = nullptr;
			skins::GuiTheme *active_theme_ = nullptr;

			gui_controller::detail::frames active_frames_;
			gui_controller::detail::frame_pointers frames_;
			gui_controller::detail::mouse_cursor_pointers mouse_cursors_;
			gui_controller::detail::tooltip_pointers tooltips_;

			NonOwningPtr<graphics::render::Viewport> default_viewport_;
			NonOwningPtr<sounds::SoundChannelGroup> default_sound_channel_group_;	
			bool shift_pressed_ = false;


			GuiFrame* NextFocusableFrame(GuiFrame *from_frame) const noexcept;
			GuiFrame* PreviousFocusableFrame(GuiFrame *from_frame) const noexcept;

			const skins::GuiSkin* GetSkin(std::string_view name) const noexcept;


			/*
				Events
			*/

			//See ObjectManager::Created for more details
			void Created(GuiComponent &component) noexcept override final;
			void Created(GuiFrame &frame) noexcept;
			void Created(controls::GuiMouseCursor &mouse_cursor) noexcept;
			void Created(controls::GuiTooltip &tooltip) noexcept;
			void Created(skins::GuiTheme &theme) noexcept;

			//See ObjectManager::Removed for more details
			void Removed(GuiComponent &component) noexcept override final;
			void Removed(GuiFrame &frame) noexcept;
			void Removed(controls::GuiMouseCursor &mouse_cursor) noexcept;
			void Removed(controls::GuiTooltip &tooltip) noexcept;
			void Removed(skins::GuiTheme &theme) noexcept;


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


			//See GuiComponent::Enabled for more details
			virtual void Enabled() noexcept override final;

			//See GuiComponent::Disabled for more details
			virtual void Disabled() noexcept override final;


			//See GuiComponent::Shown for more details
			virtual void Shown() noexcept override final;

			//See GuiComponent::Hidden for more details
			virtual void Hidden() noexcept override final;

		public:

			//Constructs a gui controller with the given parent node, default viewport and default sound channel group
			GuiController(SceneNode &parent_node, NonOwningPtr<graphics::render::Viewport> default_viewport = nullptr,
				NonOwningPtr<sounds::SoundChannelGroup> default_sound_channel_group = nullptr);


			/*
				Events
			*/

			//Returns a mutable reference to the frame events of this controller
			[[nodiscard]] inline auto& FrameEvents() noexcept
			{
				return static_cast<FrameEventsBase&>(*this);
			}

			//Returns an immutable reference to the frame events of this controller
			[[nodiscard]] inline auto& FrameEvents() const noexcept
			{
				return static_cast<const FrameEventsBase&>(*this);
			}


			//Returns a mutable reference to the managed object events of this controller
			[[nodiscard]] inline auto& ManagedObjectEvents() noexcept
			{
				return static_cast<ManagedObjectEventsBase&>(*this);
			}

			//Returns an immutable reference to the managed object events of this controller
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
				return adaptors::ranges::DereferenceIterable<gui_controller::detail::frame_pointers&>{frames_};
			}

			//Returns an immutable range of all frames in this controller
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Frames() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const gui_controller::detail::frame_pointers&>{frames_};
			}


			//Returns a mutable range of all mouse cursors in this controller
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto MouseCursors() noexcept
			{
				return adaptors::ranges::DereferenceIterable<gui_controller::detail::mouse_cursor_pointers&>{mouse_cursors_};
			}

			//Returns an immutable range of all mouse cursors in this controller
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto MouseCursors() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const gui_controller::detail::mouse_cursor_pointers&>{mouse_cursors_};
			}


			//Returns a mutable range of all tooltips in this controller
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Tooltips() noexcept
			{
				return adaptors::ranges::DereferenceIterable<gui_controller::detail::tooltip_pointers&>{tooltips_};
			}

			//Returns an immutable range of all tooltips in this controller
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Tooltips() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const gui_controller::detail::tooltip_pointers&>{tooltips_};
			}


			//Returns a mutable range of all themes in this controller
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Themes() noexcept
			{
				return ThemeBase::Objects();
			}

			//Returns an immutable range of all themes in this controller
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Themes() const noexcept
			{
				return ThemeBase::Objects();
			}


			/*
				Modifiers
			*/

			//Sets the mouse cursor used by this controller to the mouse cursor with the given name
			void ActiveMouseCursor(std::string_view name) noexcept;

			//Sets the tooltip used by this controller to the tooltip with the given name
			void ActiveTooltip(std::string_view name) noexcept;

			//Sets the theme used by this controller to the theme with the given name
			void ActiveTheme(std::string_view name) noexcept;


			/*
				Observers
			*/

			//Returns a pointer to the mouse cursor used by this controller
			//Returns nullptr if there is no mouse cursor in use
			[[nodiscard]] inline auto ActiveMouseCursor() const noexcept
			{
				return active_mouse_cursor_;
			}

			//Returns a pointer to the tooltip used by this controller
			//Returns nullptr if there is no tooltip in use
			[[nodiscard]] inline auto ActiveTooltip() const noexcept
			{
				return active_tooltip_;
			}

			//Returns a pointer to the theme used by this controller
			//Returns nullptr if there is no theme in use
			[[nodiscard]] inline auto ActiveTheme() const noexcept
			{
				return active_theme_;
			}


			//Returns true if the given frame is on top
			[[nodiscard]] bool IsOnTop(const GuiFrame &frame) const noexcept;


			//Returns a pointer to the current focused frame in this controller
			//Returns nullptr if there is no currently focused frame
			[[nodiscard]] inline auto FocusedFrame() const noexcept
			{
				return focused_frame_;
			}


			//Returns the default viewport for this controller
			//Returns nullptr if this controller does not have a default viewport
			[[nodiscard]] inline auto DefaultViewport() const noexcept
			{
				return default_viewport_;
			}

			//Returns a pointer to the default sound channel group for this controller
			//Returns nullptr if this controller does not have a default sound channel group
			[[nodiscard]] inline auto DefaultSoundChannelGroup() const noexcept
			{
				return default_sound_channel_group_;
			}


			/*
				Tabulating
			*/

			//Focuses the next focusable control in the current focused frame in this controller
			//Automatically focuses the next active frame when focusing past the last control
			void TabForward() noexcept;

			//Focuses the previous focusable control in the current focused frame in this controller
			//Automatically focuses the previous active frame when focusing after the first control
			void TabBackward() noexcept;


			//Returns the next focusable frame in this controller
			//Returns nullptr if no frames are focusable
			GuiFrame* NextFocusableFrame() const noexcept;

			//Returns the previous focusable frame in this controller
			//Returns nullptr if no frames are focusable
			GuiFrame* PreviousFocusableFrame() const noexcept;


			//Returns the next focusable frame from the given frame in this controller
			//Returns nullptr if no frames are focusable
			GuiFrame* NextFocusableFrame(GuiFrame &from_frame) const noexcept;

			//Returns the previous focusable frame from the given frame in this controller
			//Returns nullptr if no frames are focusable
			GuiFrame* PreviousFocusableFrame(GuiFrame &from_frame) const noexcept;


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
			//Returns true if the key press event has been consumed by the controller
			bool KeyPressed(KeyButton button) noexcept;

			//Typically called from user code when a key button has been released
			//Returns true if the key release event has been consumed by the controller
			bool KeyReleased(KeyButton button) noexcept;

			//Typically called from user code when a character has been pressed
			//Returns true if the character press event has been consumed by the controller
			bool CharacterPressed(char character) noexcept;


			/*
				Mouse events
			*/

			//Typically called from user code when the mouse button has been pressed
			//Returns true if the mouse press event has been consumed by the controller
			bool MousePressed(MouseButton button, Vector2 position) noexcept;

			//Typically called from user code when the mouse button has been released
			//Returns true if the mouse release event has been consumed by the controller
			bool MouseReleased(MouseButton button, Vector2 position) noexcept;

			//Typically called from user code when the mouse has been moved
			//Returns true if the mouse move event has been consumed by the controller
			bool MouseMoved(Vector2 position) noexcept;

			//Typically called from user code when the mouse wheel has been rolled
			//Returns true if the mouse wheel roll event has been consumed by the controller
			bool MouseWheelRolled(int delta, Vector2 position) noexcept;


			/*
				Window events
			*/

			//Typically called from user code when a window action has been received
			void WindowActionReceived(WindowAction action) noexcept;


			/*
				Frames
				Creating
			*/

			//Creates a frame with the given name
			NonOwningPtr<GuiFrame> CreateFrame(std::string name);

			//Creates a frame by moving the given frame
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

			//Clears all removable frames from this controller
			void ClearFrames() noexcept;

			//Removes a removable frame from this controller
			bool RemoveFrame(GuiFrame &frame) noexcept;

			//Removes a removable frame with the given name from this controller
			bool RemoveFrame(std::string_view name) noexcept;


			/*
				Mouse cursors
				Creating
			*/

			//Creates a mouse cursor with the given name and size
			NonOwningPtr<controls::GuiMouseCursor> CreateMouseCursor(std::string name, const std::optional<Vector2> &size);

			//Creates a mouse cursor with the given name, skin and size
			NonOwningPtr<controls::GuiMouseCursor> CreateMouseCursor(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size);

			//Creates a mouse cursor by moving the given mouse cursor
			NonOwningPtr<controls::GuiMouseCursor> CreateMouseCursor(controls::GuiMouseCursor &&mouse_cursor);


			/*
				Mouse cursors
				Retrieving
			*/

			//Gets a pointer to a mutable mouse cursor with the given name
			//Returns nullptr if mouse cursor could not be found
			[[nodiscard]] NonOwningPtr<controls::GuiMouseCursor> GetMouseCursor(std::string_view name) noexcept;

			//Gets a pointer to an immutable mouse cursor with the given name
			//Returns nullptr if mouse cursor could not be found
			[[nodiscard]] NonOwningPtr<const controls::GuiMouseCursor> GetMouseCursor(std::string_view name) const noexcept;


			/*
				Mouse cursors
				Removing
			*/

			//Clears all removable mouse cursors from this controller
			void ClearMouseCursors() noexcept;

			//Removes a removable mouse cursor from this controller
			bool RemoveMouseCursor(controls::GuiMouseCursor &mouse_cursor) noexcept;

			//Removes a removable mouse cursor with the given name from this controller
			bool RemoveMouseCursor(std::string_view name) noexcept;


			/*
				Tooltips
				Creating
			*/

			//Creates a tooltip with the given name and size
			NonOwningPtr<controls::GuiTooltip> CreateTooltip(std::string name, const std::optional<Vector2> &size);

			//Creates a tooltip with the given name, skin and size
			NonOwningPtr<controls::GuiTooltip> CreateTooltip(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size);

			//Creates a tooltip by moving the given tooltip
			NonOwningPtr<controls::GuiTooltip> CreateTooltip(controls::GuiTooltip &&tooltip);


			/*
				Tooltips
				Retrieving
			*/

			//Gets a pointer to a mutable tooltip with the given name
			//Returns nullptr if tooltip could not be found
			[[nodiscard]] NonOwningPtr<controls::GuiTooltip> GetTooltip(std::string_view name) noexcept;

			//Gets a pointer to an immutable tooltip with the given name
			//Returns nullptr if tooltip could not be found
			[[nodiscard]] NonOwningPtr<const controls::GuiTooltip> GetTooltip(std::string_view name) const noexcept;


			/*
				Tooltips
				Removing
			*/

			//Clears all removable tooltips from this controller
			void ClearTooltips() noexcept;

			//Removes a removable tooltip from this controller
			bool RemoveTooltip(controls::GuiTooltip &tooltip) noexcept;

			//Removes a removable tooltip with the given name from this controller
			bool RemoveTooltip(std::string_view name) noexcept;


			/*
				Components
				Removing (optimization)
			*/

			//Clears all removable components from this container
			void ClearComponents() noexcept;


			/*
				Themes
				Creating
			*/

			//Creates a theme with the given name
			NonOwningPtr<skins::GuiTheme> CreateTheme(std::string name, NonOwningPtr<graphics::scene::SceneManager> scene_manager);

			//Creates a theme by moving the given theme
			NonOwningPtr<skins::GuiTheme> CreateTheme(skins::GuiTheme &&theme);


			/*
				Themes
				Retrieving
			*/

			//Gets a pointer to a mutable theme with the given name
			//Returns nullptr if theme could not be found
			[[nodiscard]] NonOwningPtr<skins::GuiTheme> GetTheme(std::string_view name) noexcept;

			//Gets a pointer to an immutable theme with the given name
			//Returns nullptr if theme could not be found
			[[nodiscard]] NonOwningPtr<const skins::GuiTheme> GetTheme(std::string_view name) const noexcept;


			/*
				Themes
				Removing
			*/

			//Clears all removable themes from this theme
			void ClearThemes() noexcept;

			//Removes a removable theme from this theme
			bool RemoveTheme(skins::GuiTheme &theme) noexcept;

			//Removes a removable theme with the given name from this theme
			bool RemoveTheme(std::string_view name) noexcept;
	};
} //ion::gui

#endif