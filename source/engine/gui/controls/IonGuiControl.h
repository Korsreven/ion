/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiControl.h
-------------------------------------------
*/

#ifndef ION_GUI_CONTROL_H
#define ION_GUI_CONTROL_H

#include <optional>
#include <string>
#include <vector>

#include "events/IonCallback.h"
#include "events/IonEventGenerator.h"
#include "events/listeners/IonGuiControlListener.h"
#include "events/listeners/IonKeyListener.h"
#include "events/listeners/IonMouseListener.h"
#include "graphics/utilities/IonAabb.h"
#include "graphics/utilities/IonVector2.h"
#include "gui/IonGuiComponent.h"
#include "types/IonTypes.h"

namespace ion::gui
{
	class GuiPanelContainer; //Forward declaration
}

namespace ion::gui::controls
{
	using namespace events::listeners;
	using namespace ion::graphics::utilities;

	namespace gui_control
	{
		enum class ControlState
		{
			Enabled,
			Disabled,
			Focused,
			Pressed,
			Hovered
		};

		using Areas = std::vector<Aabb>;


		namespace detail
		{
		} //detail
	} //gui_control


	class GuiControl :
		public GuiComponent,
		protected events::EventGenerator<events::listeners::GuiControlListener>
	{
		protected:
		
			bool focused_ = false;
			bool pressed_ = false;
			bool hovered_ = false;
			bool focusable_ = true;
			gui_control::ControlState state_ = gui_control::ControlState::Enabled;
			gui_control::Areas clickable_areas_;
			
			std::optional<events::Callback<void, GuiControl&>> on_focus_;
			std::optional<events::Callback<void, GuiControl&>> on_defocus_;
			std::optional<events::Callback<void, GuiControl&>> on_press_;
			std::optional<events::Callback<void, GuiControl&>> on_release_;
			std::optional<events::Callback<void, GuiControl&>> on_click_;
			std::optional<events::Callback<void, GuiControl&>> on_enter_;
			std::optional<events::Callback<void, GuiControl&>> on_exit_;
			std::optional<events::Callback<void, GuiControl&>> on_change_;


			/*
				Events
			*/

			//See GuiComponent::Enabled for more details
			virtual void Enabled() noexcept override;

			//See GuiComponent::Disabled for more details
			virtual void Disabled() noexcept override;


			//Called right after a control has been focused
			virtual void Focused() noexcept;

			//Called right after a control has been defocused
			virtual void Defocused() noexcept;


			//Called right after a control has been pressed
			virtual void Pressed() noexcept;

			//Called right after a control has been released
			virtual void Released() noexcept;

			//Called right after a control has been clicked
			//Namely after a complete press and release
			virtual void Clicked() noexcept;


			//Called right after a control has been entered
			//Namely when the mouse cursor has entered the control
			virtual void Entered() noexcept;

			//Called right after a control has been exited
			//Namely when the mouse cursor has exited the control
			virtual void Exited() noexcept;


			//Called right after a control has been changed
			virtual void Changed() noexcept;


			/*
				Notifying
			*/

			void NotifyControlEnabled() noexcept;
			void NotifyControlDisabled() noexcept;

			void NotifyControlFocused() noexcept;
			void NotifyControlDefocused() noexcept;

			void NotifyControlPressed() noexcept;
			void NotifyControlReleased() noexcept;
			void NotifyControlClicked() noexcept;

			void NotifyControlEntered() noexcept;
			void NotifyControlExited() noexcept;

			void NotifyControlChanged() noexcept;


			/*
				States
			*/

			void SetState(gui_control::ControlState state) noexcept;

		public:

			//Construct a control with the given name
			GuiControl(std::string name);

			//Construct a control with the given name and clickable size
			GuiControl(std::string name, const Vector2 &size);

			//Construct a control with the given name and clickable areas
			GuiControl(std::string name, gui_control::Areas areas);


			/*
				Modifiers
			*/

			//Focus this control
			void Focus() noexcept;

			//Defocus this control
			void Defocus() noexcept;


			//Press this control
			void Press() noexcept;

			//Release this control
			void Release() noexcept;

			//Click this control
			void Click() noexcept;


			//Enter this control (start hovering)
			void Enter() noexcept;

			//Exit this control (stop hovering)
			void Exit() noexcept;


			//Reset this control to its enabled/disabled state
			//Namely by executing release, defocus and exit
			void Reset() noexcept;


			//Sets whether or not this control is enabled
			inline void Enabled(bool enabled) noexcept
			{
				return GuiComponent::Enabled(enabled);
			}

			//Sets whether or not this control is focused
			inline void Focused(bool focused) noexcept
			{
				if (focused)
					Focus();
				else
					Defocus();
			}

			//Sets whether or not this control is focusable
			inline void Focusable(bool focusable) noexcept
			{
				if (focusable_ != focusable)
				{
					if (!(focusable_ = focusable) && focused_)
						Defocus();
				}
			}


			//Sets the clickable areas of this control to the given areas
			inline void ClickableAreas(gui_control::Areas areas) noexcept
			{
				clickable_areas_ = std::move(areas);
			}

			//Sets the clickable size of this control to the given size
			void ClickableSize(const Vector2 &size) noexcept;


			//Sets the on focus callback
			inline void OnFocus(events::Callback<void, GuiControl&> on_focus) noexcept
			{
				on_focus_ = on_focus;
			}

			//Sets the on focus callback
			inline void OnFocus(std::nullopt_t) noexcept
			{
				on_focus_ = {};
			}


			//Sets the on defocus callback
			inline void OnDefocus(events::Callback<void, GuiControl&> on_defocus) noexcept
			{
				on_defocus_ = on_defocus;
			}

			//Sets the on defocus callback
			inline void OnDefocus(std::nullopt_t) noexcept
			{
				on_defocus_ = {};
			}


			//Sets the on press callback
			inline void OnPress(events::Callback<void, GuiControl&> on_press) noexcept
			{
				on_press_ = on_press;
			}

			//Sets the on press callback
			inline void OnPress(std::nullopt_t) noexcept
			{
				on_press_ = {};
			}


			//Sets the on release callback
			inline void OnRelease(events::Callback<void, GuiControl&> on_release) noexcept
			{
				on_release_ = on_release;
			}

			//Sets the on release callback
			inline void OnRelease(std::nullopt_t) noexcept
			{
				on_release_ = {};
			}


			//Sets the on click callback
			inline void OnClick(events::Callback<void, GuiControl&> on_click) noexcept
			{
				on_click_ = on_click;
			}

			//Sets the on click callback
			inline void OnClick(std::nullopt_t) noexcept
			{
				on_click_ = {};
			}


			//Sets the on enter callback
			inline void OnEnter(events::Callback<void, GuiControl&> on_enter) noexcept
			{
				on_enter_ = on_enter;
			}

			//Sets the on enter callback
			inline void OnEnter(std::nullopt_t) noexcept
			{
				on_enter_ = {};
			}


			//Sets the on exit callback
			inline void OnExit(events::Callback<void, GuiControl&> on_exit) noexcept
			{
				on_exit_ = on_exit;
			}

			//Sets the on exit callback
			inline void OnExit(std::nullopt_t) noexcept
			{
				on_exit_ = {};
			}


			//Sets the on change callback
			inline void OnChange(events::Callback<void, GuiControl&> on_change) noexcept
			{
				on_change_ = on_change;
			}

			//Sets the on change callback
			inline void OnChange(std::nullopt_t) noexcept
			{
				on_change_ = {};
			}


			/*
				Observers
			*/

			//Returns true if this control is focused
			[[nodiscard]] inline auto IsFocused() const noexcept
			{
				return focused_;
			}

			//Returns true if this control is focusable
			[[nodiscard]] inline auto IsFocusable() const noexcept
			{
				return focusable_;
			}

			//Returns true if this control is pressed
			[[nodiscard]] inline auto IsPressed() const noexcept
			{
				return pressed_;
			}

			//Returns true if this control is hovered
			[[nodiscard]] inline auto IsHovered() const noexcept
			{
				return hovered_;
			}

			//Returns the visual state of this control
			[[nodiscard]] inline auto VisualState() const noexcept
			{
				return state_;
			}


			//Returns all of the clickable areas of this control
			[[nodiscard]] inline auto& ClickableAreas() const noexcept
			{
				return clickable_areas_;
			}

			//Returns the clickable size of this control
			//If multiple clickable areas, the total merged size is returned
			[[nodiscard]] Vector2 ClickableSize() const noexcept;


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

			//Returns the on press callback
			[[nodiscard]] inline auto OnPress() const noexcept
			{
				return on_press_;
			}

			//Returns the on release callback
			[[nodiscard]] inline auto OnRelease() const noexcept
			{
				return on_release_;
			}

			//Returns the on enter callback
			[[nodiscard]] inline auto OnEnter() const noexcept
			{
				return on_enter_;
			}

			//Returns the on exit callback
			[[nodiscard]] inline auto OnExit() const noexcept
			{
				return on_exit_;
			}

			//Returns the on change callback
			[[nodiscard]] inline auto OnChange() const noexcept
			{
				return on_change_;
			}


			//Returns a pointer to the owner of this control
			[[nodiscard]] GuiPanelContainer* Owner() const noexcept;


			/*
				Intersecting
			*/

			//Returns true if the given point intersects with this control
			[[nodiscard]] bool Intersects(const Vector2 &point) const noexcept;


			/*
				Tabulating
			*/

			//Sets the tab order of this control to the given order
			void TabOrder(int order) noexcept;

			//Returns the tab order of this control
			//Returns nullopt if this control has no owner
			[[nodiscard]] std::optional<int> TabOrder() const noexcept;


			/*
				Frame events
			*/

			//Called from gui frame when a frame has started
			virtual void FrameStarted(duration time) noexcept;

			//Called from gui frame when a frame has ended
			virtual void FrameEnded(duration time) noexcept;


			/*
				Key events
			*/

			//Called from gui frame when a key button has been pressed
			//Returns true if the key press event has been consumed by the control
			virtual bool KeyPressed(KeyButton button) noexcept;

			//Called from gui frame when a key button has been released
			//Returns true if the key release event has been consumed by the control
			virtual bool KeyReleased(KeyButton button) noexcept;

			//Called from gui frame when a character has been pressed
			//Returns true if the character press event has been consumed by the control
			virtual bool CharacterPressed(char character) noexcept;


			/*
				Mouse events
			*/

			//Called from gui frame when the mouse button has been pressed
			//Returns true if the mouse press event has been consumed by the control
			virtual bool MousePressed(MouseButton button, Vector2 position) noexcept;

			//Called from gui frame when the mouse button has been released
			//Returns true if the mouse release event has been consumed by the control
			virtual bool MouseReleased(MouseButton button, Vector2 position) noexcept;

			//Called from gui frame when the mouse has been moved
			//Returns true if the mouse move event has been consumed by the control
			virtual bool MouseMoved(Vector2 position) noexcept;

			//Called from gui frame when the mouse wheel has been rolled
			//Returns true if the mouse wheel roll event has been consumed by the control
			virtual bool MouseWheelRolled(int delta, Vector2 position) noexcept;
	};
} //ion::gui::controls

#endif