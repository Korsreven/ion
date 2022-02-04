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
#include "controls/IonGuiTooltip.h"
#include "events/IonListenable.h"
#include "events/listeners/IonGuiFrameListener.h"
#include "events/listeners/IonKeyListener.h"
#include "events/listeners/IonMouseListener.h"
#include "events/listeners/IonWindowListener.h"
#include "graphics/utilities/IonVector2.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"
#include "skins/IonGuiTheme.h"
#include "types/IonTypes.h"

//Forward declarations
namespace ion::graphics::scene
{
	class SceneManager;

	namespace graph
	{
		class SceneNode;
	}
}

namespace ion::gui
{
	using namespace events::listeners;
	using namespace graphics::scene::graph;
	using namespace graphics::utilities;

	namespace gui_controller
	{
		enum class GuiMouseCursorHotSpot
		{
			TopLeft,	TopCenter,		TopRight,
			Left,		Center,			Right,
			BottomLeft, BottomCenter,	BottomRight
		};

		struct GuiMouseCursorSkin final
		{
			NonOwningPtr<graphics::scene::Model> ModelObject;


			[[nodiscard]] inline operator bool() const noexcept
			{
				return !!ModelObject;
			}

			[[nodiscard]] inline auto operator->() const noexcept
			{
				return ModelObject.get();
			}
		};


		namespace detail
		{
			using frame_pointers = std::vector<GuiFrame*>;
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


			Vector2 cursor_hot_spot_offset(GuiMouseCursorHotSpot hot_spot, const Vector2 &cursor_size) noexcept;
			Vector2 tooltip_hot_spot_offset(GuiMouseCursorHotSpot hot_spot, const Vector2 &tooltip_size, const Vector2 &cursor_size) noexcept;
		} //detail
	} //gui_controller


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
			controls::GuiTooltip *active_tooltip_ = nullptr;
			gui_controller::detail::frames active_frames_;
			bool shift_pressed_ = false;

			gui_controller::GuiMouseCursorSkin mouse_cursor_skin_;
			gui_controller::GuiMouseCursorHotSpot mouse_cursor_hot_spot_ = gui_controller::GuiMouseCursorHotSpot::TopLeft;

			gui_controller::detail::frame_pointers frames_;
			gui_controller::detail::tooltip_pointers tooltips_;


			GuiFrame* NextFocusableFrame(GuiFrame *from_frame) const noexcept;
			GuiFrame* PreviousFocusableFrame(GuiFrame *from_frame) const noexcept;


			/*
				Events
			*/

			//See ObjectManager::Created for more details
			void Created(GuiComponent &component) noexcept override final;
			void Created(GuiFrame &frame) noexcept;
			void Created(controls::GuiTooltip &tooltip) noexcept;

			//See ObjectManager::Removed for more details
			void Removed(GuiComponent &component) noexcept override final;
			void Removed(GuiFrame &frame) noexcept;
			void Removed(controls::GuiTooltip &tooltip) noexcept;


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


			/*
				Mouse cursor skin
			*/

			void AttachMouseCursorSkin(real z_order);
			void DetachMouseCursorSkin() noexcept;
			void RemoveMouseCursorSkin() noexcept;

			void UpdateMouseCursor(const Vector2 &position) noexcept;

		public:

			//Construct a gui controller with the given parent node
			explicit GuiController(SceneNode &parent_node);

			//Destructor
			~GuiController() noexcept;


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
				return adaptors::ranges::DereferenceIterable<gui_controller::detail::frame_pointers&>{frames_};
			}

			//Returns an immutable range of all frames in this controller
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Frames() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const gui_controller::detail::frame_pointers&>{frames_};
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

			//Sets the tooltip used by this controller to the tooltip with the given name
			void ActiveTooltip(std::string_view name) noexcept;

			//Sets the mouse cursor skin used by this controller to the given skin with the given z-order
			void MouseCursorSkin(gui_controller::GuiMouseCursorSkin skin, real z_order) noexcept;

			//Sets the mouse cursor hot spot to the given hot spot
			//The point in the mouse cursor skin that interacts with other elements on the screen
			inline void MouseCursorHotSpot(gui_controller::GuiMouseCursorHotSpot hot_spot) noexcept
			{
				mouse_cursor_hot_spot_ = hot_spot;
			}


			/*
				Observers
			*/

			//Returns a pointer to the tooltip used by this controller
			[[nodiscard]] inline auto& ActiveTooltip() const noexcept
			{
				return active_tooltip_;
			}

			//Returns the mouse cursor skin used by this controller
			[[nodiscard]] inline auto& MouseCursorSkin() const noexcept
			{
				return mouse_cursor_skin_;
			}

			//Returns the mouse cursor hot spot
			//The point in the mouse cursor skin that interacts with other elements on the screen
			[[nodiscard]] inline auto& MouseCursorHotSpot() const noexcept
			{
				return mouse_cursor_hot_spot_;
			}


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


			/*
				Tooltips
				Creating
			*/

			//Create a tooltip with the given name and skin
			NonOwningPtr<controls::GuiTooltip> CreateTooltip(std::string name, controls::gui_tooltip::TooltipSkin skin);

			//Create a tooltip with the given name, skin and size
			NonOwningPtr<controls::GuiTooltip> CreateTooltip(std::string name, controls::gui_tooltip::TooltipSkin skin, const Vector2 &size);

			//Create a tooltip by moving the given tooltip
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

			//Clear all removable tooltips from this controller
			void ClearTooltips() noexcept;

			//Remove a removable tooltip from this controller
			bool RemoveTooltip(controls::GuiTooltip &tooltip) noexcept;

			//Remove a removable tooltip with the given name from this controller
			bool RemoveTooltip(std::string_view name) noexcept;


			/*
				Components
				Removing (optimization)
			*/

			//Clear all removable components from this container
			void ClearComponents() noexcept;


			/*
				Themes
				Creating
			*/

			//Create a theme with the given name
			NonOwningPtr<skins::GuiTheme> CreateTheme(std::string name, NonOwningPtr<graphics::scene::SceneManager> scene_manager);

			//Create a theme by moving the given theme
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

			//Clear all removable themes from this theme
			void ClearThemes() noexcept;

			//Remove a removable theme from this theme
			bool RemoveTheme(skins::GuiTheme &theme) noexcept;

			//Remove a removable theme with the given name from this theme
			bool RemoveTheme(std::string_view name) noexcept;
	};
} //ion::gui

#endif