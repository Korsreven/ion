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
	using namespace ion::graphics::utilities;

	class GuiController; //Forward declaration

	namespace gui_frame::detail
	{
		using gui_panel_container::detail::control_pointers;


		void get_ordered_controls(GuiPanelContainer &owner, control_pointers &controls);
		control_pointers get_ordered_controls(GuiPanelContainer &owner);


		std::optional<control_pointers::iterator> get_current_control_iterator(control_pointers &controls,
			controls::GuiControl *focused_control) noexcept;

		inline auto get_next_control_iterator(control_pointers::iterator iter, control_pointers &controls) noexcept
		{
			return iter != std::end(controls) ? iter + 1 : std::begin(controls);
		}

		inline auto get_previous_control_iterator(control_pointers::iterator iter, control_pointers &controls) noexcept
		{
			return iter != std::begin(controls) ? iter - 1 : std::end(controls) - 1;
		}
	} //gui_frame::detail


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
		
		protected:

			bool activated_ = false;
			bool focused_ = false;

			controls::GuiControl *focused_control_ = nullptr;
			controls::GuiControl *pressed_control_ = nullptr;
			controls::GuiControl *hovered_control_ = nullptr;

			std::optional<events::Callback<void, GuiFrame&>> on_activate_;
			std::optional<events::Callback<void, GuiFrame&>> on_deactivate_;
			std::optional<events::Callback<void, GuiFrame&>> on_focus_;
			std::optional<events::Callback<void, GuiFrame&>> on_defocus_;


			/*
				Events
			*/

			//See GuiPanelContainer::Created for more details
			virtual void Created(controls::GuiControl &control) noexcept override;

			//See GuiPanelContainer::Removed for more details
			virtual void Removed(controls::GuiControl &control) noexcept override;


			//See Listener<T>::Unsubscribable for more details
			//Make sure that if this gui control listener is about to unsubscribe from the gui frame, cancel it
			bool Unsubscribable(Listenable<events::listeners::GuiControlListener>&) noexcept override final;


			//See GuiControlListener::Enabled for more details
			virtual void Enabled(controls::GuiControl &control) noexcept override;

			//See GuiControlListener::Disabled for more details
			virtual void Disabled(controls::GuiControl &control) noexcept override;


			//See GuiControlListener::Focused for more details
			virtual void Focused(controls::GuiControl &control) noexcept override;

			//See GuiControlListener::Defocused for more details
			virtual void Defocused(controls::GuiControl &control) noexcept override;


			//See GuiControlListener::Pressed for more details
			virtual void Pressed(controls::GuiControl &control) noexcept override;

			//See GuiControlListener::Released for more details
			virtual void Released(controls::GuiControl &control) noexcept override;

			//See GuiControlListener::Clicked for more details
			virtual void Clicked(controls::GuiControl &control) noexcept override;


			//See GuiControlListener::Entered for more details
			virtual void Entered(controls::GuiControl &control) noexcept override;

			//See GuiControlListener::Exited for more details
			virtual void Exited(controls::GuiControl &control) noexcept override;


			//See GuiComponent::Enabled for more details
			virtual void Enabled() noexcept override;

			//See GuiComponent::Disabled for more details
			virtual void Disabled() noexcept override;


			//Called right after a frame has been activated
			virtual void Activated() noexcept;

			//Called right after a frame has been deactivated
			virtual void Deactivated() noexcept;


			//Called right after a frame has been focused
			virtual void Focused() noexcept;

			//Called right after a frame has been defocused
			virtual void Defocused() noexcept;


			/*
				Notifying
			*/

			void NotifyFrameEnabled() noexcept;
			void NotifyFrameDisabled() noexcept;

			void NotifyFrameActivated() noexcept;
			void NotifyFrameDeactivated() noexcept;

			void NotifyFrameFocused() noexcept;
			void NotifyFrameDefocused() noexcept;

		public:

			//Construct a frame with the given name
			GuiFrame(std::string name);


			/*
				Events
			*/

			//Return a mutable reference to the control events of this frame
			[[nodiscard]] inline auto& ControlEvents() noexcept
			{
				return static_cast<ControlEventsBase&>(*this);
			}

			//Return a immutable reference to the control events of this frame
			[[nodiscard]] inline auto& ControlEvents() const noexcept
			{
				return static_cast<const ControlEventsBase&>(*this);
			}


			//Return a mutable reference to the managed object events of this frame
			[[nodiscard]] inline auto& ManagedObjectEvents() noexcept
			{
				return static_cast<ManagedObjectEventsBase&>(*this);
			}

			//Return a immutable reference to the managed object events of this frame
			[[nodiscard]] inline auto& ManagedObjectEvents() const noexcept
			{
				return static_cast<const ManagedObjectEventsBase&>(*this);
			}


			/*
				Modifiers
			*/

			//Activate this frame
			void Activate() noexcept;

			//Deactivate this frame
			void Deactivate() noexcept;


			//Focus this frame
			void Focus() noexcept;

			//Defocus this frame
			void Defocus() noexcept;


			//Sets whether or not this frame is activated
			inline void Activated(bool activated) noexcept
			{
				if (activated)
					Activate();
				else
					Deactivate();
			}

			//Sets whether or not this frame is focused
			inline void Focused(bool focused) noexcept
			{
				if (focused)
					Focus();
				else
					Defocus();
			}


			//Sets the on activate callback
			inline void OnActivate(events::Callback<void, GuiFrame&> on_activate) noexcept
			{
				on_activate_ = on_activate;
			}

			//Sets the on activate callback
			inline void OnActivate(std::nullopt_t) noexcept
			{
				on_activate_ = {};
			}


			//Sets the on deactivate callback
			inline void OnDeactivate(events::Callback<void, GuiFrame&> on_deactivate) noexcept
			{
				on_deactivate_ = on_deactivate;
			}

			//Sets the on deactivate callback
			inline void OnDeactivate(std::nullopt_t) noexcept
			{
				on_deactivate_ = {};
			}


			//Sets the on focus callback
			inline void OnFocus(events::Callback<void, GuiFrame&> on_focus) noexcept
			{
				on_focus_ = on_focus;
			}

			//Sets the on focus callback
			inline void OnFocus(std::nullopt_t) noexcept
			{
				on_focus_ = {};
			}


			//Sets the on defocus callback
			inline void OnDefocus(events::Callback<void, GuiFrame&> on_defocus) noexcept
			{
				on_defocus_ = on_defocus;
			}

			//Sets the on defocus callback
			inline void OnDefocus(std::nullopt_t) noexcept
			{
				on_defocus_ = {};
			}


			//Sets whether or not this frame is enabled
			inline void Enabled(bool enabled) noexcept
			{
				return GuiComponent::Enabled(enabled);
			}


			/*
				Observers
			*/

			//Returns true if this frame is activated
			[[nodiscard]] inline auto IsActivated() const noexcept
			{
				return activated_;
			}

			//Returns true if this frame is focused
			[[nodiscard]] inline auto IsFocused() const noexcept
			{
				return focused_;
			}

			//Returns true if this frame is focusable
			[[nodiscard]] bool IsFocusable() const noexcept;

			//Returns true if this frame is on top
			[[nodiscard]] bool IsOnTop() const noexcept;


			//Returns a pointer to the current focused control in this frame
			//Returns nullptr if there is no currently focused control
			[[nodiscard]] inline auto FocusedControl() const noexcept
			{
				return focused_control_;
			}

			//Returns a pointer to the current pressed control in this frame
			//Returns nullptr if there is no currently pressed control
			[[nodiscard]] inline auto PressedControl() const noexcept
			{
				return pressed_control_;
			}

			//Returns a pointer to the current hovered control in this frame
			//Returns nullptr if there is no currently hovered control
			[[nodiscard]] inline auto HoveredControl() const noexcept
			{
				return hovered_control_;
			}


			//Returns the on activate callback
			[[nodiscard]] inline auto OnActivate() const noexcept
			{
				return on_activate_;
			}

			//Returns the on deactivate callback
			[[nodiscard]] inline auto OnDeactivate() const noexcept
			{
				return on_deactivate_;
			}


			//Returns the on focus callback
			[[nodiscard]] inline auto OnFocus() const noexcept
			{
				return on_focus_;
			}

			//Returns the on defocus callback
			[[nodiscard]] inline auto OnDefocus() const noexcept
			{
				return on_defocus_;
			}


			//Returns a pointer to the owner of this frame
			[[nodiscard]] GuiController* Owner() const noexcept;


			/*
				Tabulating
			*/

			//Focuses the next focusable control in this frame
			void TabForward() noexcept;

			//Focuses the previous focusable control in this frame
			void TabBackward() noexcept;


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