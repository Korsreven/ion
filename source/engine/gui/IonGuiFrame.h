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

#include <optional>
#include <string>
#include <string_view>

#include "IonGuiPanel.h"
#include "IonGuiPanelContainer.h"
#include "controls/IonGuiControl.h"
#include "events/IonCallback.h"
#include "events/IonEventGenerator.h"
#include "events/IonListenable.h"
#include "events/listeners/IonGuiControlListener.h"
#include "events/listeners/IonGuiFrameListener.h"
#include "events/listeners/IonKeyListener.h"
#include "events/listeners/IonMouseListener.h"
#include "graphics/utilities/IonVector2.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::gui
{
	using namespace events::listeners;
	using namespace graphics::utilities;

	class GuiController; //Forward declaration

	namespace skins
	{
		class GuiTheme; //Forward declaration
	}

	namespace gui_frame
	{
		enum class FrameMode : bool
		{
			Modeless,
			Modal
		};

		namespace detail
		{
			using gui_panel_container::detail::control_pointers;


			void get_ordered_controls(GuiPanelContainer &owner, control_pointers &controls);
			control_pointers get_ordered_controls(GuiPanelContainer &owner);


			std::optional<control_pointers::iterator> get_current_control_iterator(control_pointers &controls,
				controls::GuiControl *focused_control) noexcept;

			inline auto get_next_control_iterator(control_pointers::iterator iter, control_pointers &controls) noexcept
			{
				return iter >= std::end(controls) - 1 ? std::begin(controls) : iter + 1;
			}

			inline auto get_previous_control_iterator(control_pointers::iterator iter, control_pointers &controls) noexcept
			{
				return iter == std::begin(controls) ? std::end(controls) - 1 : iter - 1;
			} //detail
		}
	} //gui_frame


	///@brief A class representing a GUI frame that can create and store multiple GUI controls and GUI panels
	///@details A frame without an active theme will automatically use the active theme from its parent controller.
	///A frame can be modeless/modal, activated/deactivated and focused/defocused
	class GuiFrame :
		public GuiPanelContainer,
		public events::Listenable<events::listeners::GuiControlListener>,
		public events::listeners::GuiControlListener,
		protected events::EventGenerator<events::listeners::GuiFrameListener>
	{
		private:

			using ControlEventsBase = events::Listenable<events::listeners::GuiControlListener>;
			using ManagedObjectEventsBase = events::Listenable<events::listeners::ManagedObjectListener<GuiComponent, GuiContainer>>;
			using FrameEventsGeneratorBase = events::EventGenerator<events::listeners::GuiFrameListener>;

			gui_frame::detail::control_pointers ordered_controls_;


			void GatherControls();

			bool TabForward(GuiFrame &from_frame) noexcept;
			bool TabBackward(GuiFrame &from_frame) noexcept;
		
		protected:

			bool activated_ = false;
			bool focused_ = false;
			std::optional<gui_frame::FrameMode> mode_;

			controls::GuiControl *focused_control_ = nullptr;
			controls::GuiControl *pressed_control_ = nullptr;
			controls::GuiControl *hovered_control_ = nullptr;
			controls::GuiControl *last_focused_control_ = nullptr;

			std::optional<events::Callback<void, GuiFrame&>> on_activate_;
			std::optional<events::Callback<void, GuiFrame&>> on_deactivate_;
			std::optional<events::Callback<void, GuiFrame&>> on_focus_;
			std::optional<events::Callback<void, GuiFrame&>> on_defocus_;

			NonOwningPtr<skins::GuiTheme> active_theme_;


			/**
				@name Events
				@{
			*/

			///@brief See GuiPanelContainer::Created for more details
			virtual void Created(GuiComponent &component) noexcept override;
			virtual void Created(controls::GuiControl &control) noexcept override;

			///@brief See GuiPanelContainer::Removed for more details
			virtual void Removed(GuiComponent &component) noexcept override;
			virtual void Removed(controls::GuiControl &control) noexcept override;

			///@brief See GuiPanelContainer::TabOrderChanged for more details
			virtual void TabOrderChanged() noexcept override;


			///@brief See Listener<T>::Unsubscribable for more details
			///@details Make sure that if this gui control listener is about to unsubscribe from the gui frame, cancel it
			bool Unsubscribable(Listenable<events::listeners::GuiControlListener>&) noexcept override final;


			///@brief See GuiControlListener::Enabled for more details
			virtual void Enabled(controls::GuiControl &control) noexcept override;

			///@brief See GuiControlListener::Disabled for more details
			virtual void Disabled(controls::GuiControl &control) noexcept override;


			///@brief See GuiControlListener::Enabled for more details
			virtual void Shown(controls::GuiControl &control) noexcept override;

			///@brief See GuiControlListener::Disabled for more details
			virtual void Hidden(controls::GuiControl &control) noexcept override;


			///@brief See GuiControlListener::Focused for more details
			virtual void Focused(controls::GuiControl &control) noexcept override;

			///@brief See GuiControlListener::Defocused for more details
			virtual void Defocused(controls::GuiControl &control) noexcept override;


			///@brief See GuiControlListener::Pressed for more details
			virtual void Pressed(controls::GuiControl &control) noexcept override;

			///@brief See GuiControlListener::Released for more details
			virtual void Released(controls::GuiControl &control) noexcept override;

			///@brief See GuiControlListener::Clicked for more details
			virtual void Clicked(controls::GuiControl &control) noexcept override;


			///@brief See GuiControlListener::Entered for more details
			virtual void Entered(controls::GuiControl &control) noexcept override;

			///@brief See GuiControlListener::Exited for more details
			virtual void Exited(controls::GuiControl &control) noexcept override;


			///@brief See GuiComponent::Enabled for more details
			virtual void Enabled() noexcept override;

			///@brief See GuiComponent::Disabled for more details
			virtual void Disabled() noexcept override;


			///@brief See GuiComponent::Shown for more details
			virtual void Shown() noexcept override;

			///@brief See GuiComponent::Hidden for more details
			virtual void Hidden() noexcept override;


			///@brief Called right after a frame has been activated
			virtual void Activated() noexcept;

			///@brief Called right after a frame has been deactivated
			virtual void Deactivated() noexcept;


			///@brief Called right after a frame has been focused
			virtual void Focused() noexcept;

			///@brief Called right after a frame has been defocused
			virtual void Defocused() noexcept;

			///@}

			/**
				@name Notifying
				@{
			*/

			void NotifyFrameEnabled() noexcept;
			void NotifyFrameDisabled() noexcept;

			void NotifyFrameActivated() noexcept;
			void NotifyFrameDeactivated() noexcept;

			void NotifyFrameFocused() noexcept;
			void NotifyFrameDefocused() noexcept;

			///@}

			/**
				@name Intersection
				@{
			*/

			controls::GuiControl* IntersectedControl(const Vector2 &position) noexcept;

			///@}

		public:

			///@brief Constructs a frame with the given name
			explicit GuiFrame(std::string name);


			/**
				@name Events
				@{
			*/

			///@brief Returns a mutable reference to the control events of this frame
			[[nodiscard]] inline auto& ControlEvents() noexcept
			{
				return static_cast<ControlEventsBase&>(*this);
			}

			///@brief Returns an immutable reference to the control events of this frame
			[[nodiscard]] inline auto& ControlEvents() const noexcept
			{
				return static_cast<const ControlEventsBase&>(*this);
			}


			///@brief Returns a mutable reference to the managed object events of this frame
			[[nodiscard]] inline auto& ManagedObjectEvents() noexcept
			{
				return static_cast<ManagedObjectEventsBase&>(*this);
			}

			///@brief Returns an immutable reference to the managed object events of this frame
			[[nodiscard]] inline auto& ManagedObjectEvents() const noexcept
			{
				return static_cast<const ManagedObjectEventsBase&>(*this);
			}
			
			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Activates this frame with the given mode
			void Activate(gui_frame::FrameMode mode = gui_frame::FrameMode::Modeless) noexcept;

			///@brief Deactivates this frame
			void Deactivate() noexcept;


			///@brief Shows this frame with the given mode (same as calling Activate)
			void Show(gui_frame::FrameMode mode = gui_frame::FrameMode::Modeless) noexcept;

			///@brief Hides this frame (same as calling Deactivate)
			void Hide() noexcept;


			///@brief Focuses this frame
			void Focus() noexcept;

			///@brief Defocuses this frame
			void Defocus() noexcept;


			///@brief Sets whether or not this frame is activated
			inline void Activated(bool activated) noexcept
			{
				if (activated)
					Activate();
				else
					Deactivate();
			}

			///@brief Sets whether or not this frame is focused
			inline void Focused(bool focused) noexcept
			{
				if (focused)
					Focus();
				else
					Defocus();
			}


			///@brief Sets the on activate callback
			inline void OnActivate(events::Callback<void, GuiFrame&> on_activate) noexcept
			{
				on_activate_ = on_activate;
			}

			///@brief Sets the on activate callback
			inline void OnActivate(std::nullopt_t) noexcept
			{
				on_activate_ = {};
			}


			///@brief Sets the on deactivate callback
			inline void OnDeactivate(events::Callback<void, GuiFrame&> on_deactivate) noexcept
			{
				on_deactivate_ = on_deactivate;
			}

			///@brief Sets the on deactivate callback
			inline void OnDeactivate(std::nullopt_t) noexcept
			{
				on_deactivate_ = {};
			}


			///@brief Sets the on focus callback
			inline void OnFocus(events::Callback<void, GuiFrame&> on_focus) noexcept
			{
				on_focus_ = on_focus;
			}

			///@brief Sets the on focus callback
			inline void OnFocus(std::nullopt_t) noexcept
			{
				on_focus_ = {};
			}


			///@brief Sets the on defocus callback
			inline void OnDefocus(events::Callback<void, GuiFrame&> on_defocus) noexcept
			{
				on_defocus_ = on_defocus;
			}

			///@brief Sets the on defocus callback
			inline void OnDefocus(std::nullopt_t) noexcept
			{
				on_defocus_ = {};
			}


			///@brief Sets the theme used by this frame to the theme with the given name
			void ActiveTheme(std::string_view name) noexcept;

			///@brief Sets whether or not this frame is enabled
			inline void Enabled(bool enabled) noexcept
			{
				return GuiComponent::Enabled(enabled);
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns true if this frame is activated
			[[nodiscard]] inline auto IsActivated() const noexcept
			{
				return activated_;
			}

			///@brief Returns true if this frame is focused
			[[nodiscard]] inline auto IsFocused() const noexcept
			{
				return focused_;
			}

			///@brief Returns the mode of this frame
			///@details Returns nullopt if this frame is not activated
			[[nodiscard]] inline auto Mode() const noexcept
			{
				return mode_;
			}

			///@brief Returns true if this frame is focusable
			[[nodiscard]] bool IsFocusable() const noexcept;

			///@brief Returns true if this frame is on top
			[[nodiscard]] bool IsOnTop() const noexcept;


			///@brief Returns a pointer to the current focused control in this frame
			///@details Returns nullptr if there is no currently focused control
			[[nodiscard]] inline auto FocusedControl() const noexcept
			{
				return focused_control_;
			}

			///@brief Returns a pointer to the current pressed control in this frame
			///@details Returns nullptr if there is no currently pressed control
			[[nodiscard]] inline auto PressedControl() const noexcept
			{
				return pressed_control_;
			}

			///@brief Returns a pointer to the current hovered control in this frame
			///@details Returns nullptr if there is no currently hovered control
			[[nodiscard]] inline auto HoveredControl() const noexcept
			{
				return hovered_control_;
			}


			///@brief Returns the on activate callback
			[[nodiscard]] inline auto OnActivate() const noexcept
			{
				return on_activate_;
			}

			///@brief Returns the on deactivate callback
			[[nodiscard]] inline auto OnDeactivate() const noexcept
			{
				return on_deactivate_;
			}


			///@brief Returns the on focus callback
			[[nodiscard]] inline auto OnFocus() const noexcept
			{
				return on_focus_;
			}

			///@brief Returns the on defocus callback
			[[nodiscard]] inline auto OnDefocus() const noexcept
			{
				return on_defocus_;
			}


			///@brief Returns a pointer to the theme used by this frame
			///@details Returns nullptr if there is no theme in use
			[[nodiscard]] inline auto ActiveTheme() const noexcept
			{
				return active_theme_.get();
			}

			///@brief Returns a pointer to the owner of this frame
			[[nodiscard]] GuiController* Owner() const noexcept;

			///@}

			/**
				@name Tabulating
				@{
			*/

			///@brief Focuses the next focusable control in this frame
			void TabForward() noexcept;

			///@brief Focuses the previous focusable control in this frame
			void TabBackward() noexcept;

			///@}

			/**
				@name Frame events
				@{
			*/

			///@brief Called from gui controller when a frame has started
			virtual void FrameStarted(duration time) noexcept;

			///@brief Called from gui controller when a frame has ended
			virtual void FrameEnded(duration time) noexcept;

			///@}

			/**
				@name Key events
				@{
			*/

			///@brief Called from gui controller when a key button has been pressed
			///@details Returns true if the key press event has been consumed by the frame
			virtual bool KeyPressed(KeyButton button) noexcept;

			///@brief Called from gui controller when a key button has been released
			///@details Returns true if the key release event has been consumed by the frame
			virtual bool KeyReleased(KeyButton button) noexcept;

			///@brief Called from gui controller when a character has been pressed
			///@details Returns true if the character press event has been consumed by the frame
			virtual bool CharacterPressed(char character) noexcept;

			///@}

			/**
				@name Mouse events
				@{
			*/

			///@brief Called from gui controller when the mouse button has been pressed
			///@details Returns true if the mouse press event has been consumed by the frame
			virtual bool MousePressed(MouseButton button, Vector2 position) noexcept;

			///@brief Called from gui controller when the mouse button has been released
			///@details Returns true if the mouse release event has been consumed by the frame
			virtual bool MouseReleased(MouseButton button, Vector2 position) noexcept;

			///@brief Called from gui controller when the mouse has been moved
			///@details Returns true if the mouse move event has been consumed by the frame
			virtual bool MouseMoved(Vector2 position) noexcept;

			///@brief Called from gui controller when the mouse wheel has been rolled
			///@details Returns true if the mouse wheel roll event has been consumed by the frame
			virtual bool MouseWheelRolled(int delta, Vector2 position) noexcept;

			///@}
	};
} //ion::gui

#endif