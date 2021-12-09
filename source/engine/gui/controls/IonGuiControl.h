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
#include "graphics/fonts/IonText.h"
#include "graphics/utilities/IonAabb.h"
#include "graphics/utilities/IonVector2.h"
#include "gui/IonGuiComponent.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

//Forward declarations
namespace ion
{
	namespace gui
	{
		class GuiPanelContainer;
	}

	namespace graphics
	{
		namespace materials
		{
			class Material;
		}

		namespace scene
		{
			class DrawableText;
			class Model;

			namespace shapes
			{
				class Sprite;
			}
		}
	}
}

namespace ion::gui::controls
{
	using namespace events::listeners;
	using namespace graphics::utilities;

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

		enum class ControlCaptionLayout
		{
								OutsideTopLeft,		OutsideTopCenter,		OutsideTopRight,
			OutsideLeftTop,		TopLeft,			TopCenter,				TopRight,			OutsideRightTop,
			OutsideLeftCenter,	Left,				Center,					Right,				OutsideRightCenter,
			OutsideLeftBottom,	BottomLeft,			BottomCenter,			BottomRight,		OutsideRightBottom,
								OutsideBottomLeft,	OutsideBottomCenter,	OutsideBottomRight
		};

		using Areas = std::vector<Aabb>;


		struct ControlVisualPart final
		{
			NonOwningPtr<graphics::scene::shapes::Sprite> SpriteObject;
			NonOwningPtr<graphics::materials::Material> EnabledMaterial;
			NonOwningPtr<graphics::materials::Material> DisabledMaterial;
			NonOwningPtr<graphics::materials::Material> FocusedMaterial;
			NonOwningPtr<graphics::materials::Material> PressedMaterial;
			NonOwningPtr<graphics::materials::Material> HoveredMaterial;


			[[nodiscard]] inline operator bool() const noexcept
			{
				return !!SpriteObject;
			}

			[[nodiscard]] inline auto operator->() const noexcept
			{
				return SpriteObject.get();
			}
		};

		struct ControlVisualParts final
		{
			NonOwningPtr<graphics::scene::Model> ModelObject;

			//Center
			ControlVisualPart Center;

			//Sides
			ControlVisualPart Top;
			ControlVisualPart Left;
			ControlVisualPart Bottom;
			ControlVisualPart Right;

			//Corners
			ControlVisualPart TopLeft;
			ControlVisualPart BottomLeft;
			ControlVisualPart TopRight;
			ControlVisualPart BottomRight;


			[[nodiscard]] inline operator bool() const noexcept
			{
				return !!ModelObject;
			}

			[[nodiscard]] inline auto operator->() const noexcept
			{
				return ModelObject.get();
			}
		};

		struct ControlCaptionPart final
		{
			NonOwningPtr<graphics::scene::DrawableText> TextObject;
			std::optional<graphics::fonts::text::TextBlockStyle> EnabledStyle;
			std::optional<graphics::fonts::text::TextBlockStyle> DisabledStyle;
			std::optional<graphics::fonts::text::TextBlockStyle> FocusedStyle;
			std::optional<graphics::fonts::text::TextBlockStyle> PressedStyle;
			std::optional<graphics::fonts::text::TextBlockStyle> HoveredStyle;


			[[nodiscard]] inline operator bool() const noexcept
			{
				return !!TextObject;
			}

			[[nodiscard]] inline auto operator->() const noexcept
			{
				return TextObject.get();
			}
		};


		struct ControlSkin final
		{
			ControlVisualParts Parts;
			ControlCaptionPart Caption;
		};

		/*
		struct CheckBoxSkin final
		{
			NonOwningPtr<graphics::scene::Model> CheckMark;
		};

		struct ListBoxSkin final
		{
			NonOwningPtr<graphics::scene::Model> Selection;
			NonOwningPtr<graphics::scene::DrawableText> Text;
		};

		struct ProgressBarSkin final
		{
			NonOwningPtr<graphics::scene::Model> Bar;
			NonOwningPtr<graphics::scene::Model> BarPrecise;
		};

		struct ScrollBarSkin final
		{
			NonOwningPtr<graphics::scene::Model> Bar;
		};

		struct TextBoxSkin final
		{
			NonOwningPtr<graphics::scene::Model> Cursor;
			NonOwningPtr<graphics::scene::DrawableText> Text;
		};
		*/


		namespace detail
		{
			constexpr auto default_caption_padding_size = 2.0_r;


			inline auto control_state_to_material(ControlState state, ControlVisualPart &part) noexcept
			{
				switch (state)
				{
					case ControlState::Disabled:
					return part.DisabledMaterial;

					case ControlState::Focused:
					return part.FocusedMaterial;

					case ControlState::Pressed:
					return part.PressedMaterial;

					case ControlState::Hovered:
					return part.HoveredMaterial;

					case ControlState::Enabled:
					default:
					return part.EnabledMaterial;
				}
			}

			inline auto& control_state_to_style(ControlState state, ControlCaptionPart &part) noexcept
			{
				switch (state)
				{
					case ControlState::Disabled:
					return part.DisabledStyle;

					case ControlState::Focused:
					return part.FocusedStyle;

					case ControlState::Pressed:
					return part.PressedStyle;

					case ControlState::Hovered:
					return part.HoveredStyle;

					case ControlState::Enabled:
					default:
					return part.EnabledStyle;
				}
			}


			void resize_part(ControlVisualPart &part, const Vector2 &delta_size, const Vector2 &delta_position, const Vector2 &center) noexcept;		
			void resize_skin(ControlSkin &skin, const Vector2 &from_size, const Vector2 &to_size) noexcept;

			void resize_area(Aabb &area, const Vector2 &scaling) noexcept;
			void resize_areas(Areas &areas, const Vector2 &from_size, const Vector2 &to_size) noexcept;

			std::optional<Aabb> get_visual_area(const ControlSkin &skin, bool include_caption) noexcept;
			std::optional<Aabb> get_center_area(const ControlSkin &skin, bool include_caption) noexcept;
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

			std::optional<std::string> caption_;
			std::optional<std::string> tooltip_;

			std::optional<Vector2> caption_size_;
			std::optional<Vector2> caption_padding_;
			gui_control::ControlCaptionLayout caption_layout_ = gui_control::ControlCaptionLayout::Center;

			gui_control::ControlState state_ = gui_control::ControlState::Enabled;
			gui_control::ControlSkin skin_;
			gui_control::Areas hit_areas_;
			
			std::optional<events::Callback<void, GuiControl&>> on_focus_;
			std::optional<events::Callback<void, GuiControl&>> on_defocus_;
			std::optional<events::Callback<void, GuiControl&>> on_press_;
			std::optional<events::Callback<void, GuiControl&>> on_release_;
			std::optional<events::Callback<void, GuiControl&>> on_click_;
			std::optional<events::Callback<void, GuiControl&>> on_enter_;
			std::optional<events::Callback<void, GuiControl&>> on_exit_;
			std::optional<events::Callback<void, GuiControl&>> on_change_;
			std::optional<events::Callback<void, GuiControl&>> on_resize_;
			std::optional<events::Callback<void, GuiControl&>> on_state_change_;


			/*
				Events
			*/

			//See GuiComponent::Created for more details
			virtual void Created() noexcept override;

			//See GuiComponent::Removed for more details
			virtual void Removed() noexcept override;


			//See GuiComponent::Enabled for more details
			virtual void Enabled() noexcept override;

			//See GuiComponent::Disabled for more details
			virtual void Disabled() noexcept override;


			//See GuiComponent::Shown for more details
			virtual void Shown() noexcept override;

			//See GuiComponent::Hidden for more details
			virtual void Hidden() noexcept override;


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

			//Called right after a control has been resized
			virtual void Resized() noexcept;


			//Called right after a control has changed state
			virtual void StateChanged() noexcept;


			/*
				Notifying
			*/

			void NotifyControlEnabled() noexcept;
			void NotifyControlDisabled() noexcept;

			void NotifyControlShown() noexcept;
			void NotifyControlHidden() noexcept;

			void NotifyControlFocused() noexcept;
			void NotifyControlDefocused() noexcept;

			void NotifyControlPressed() noexcept;
			void NotifyControlReleased() noexcept;
			void NotifyControlClicked() noexcept;

			void NotifyControlEntered() noexcept;
			void NotifyControlExited() noexcept;

			void NotifyControlChanged() noexcept;
			void NotifyControlResized() noexcept;

			void NotifyControlStateChanged() noexcept;


			/*
				States
			*/
			
			NonOwningPtr<graphics::materials::Material> GetStateMaterial(gui_control::ControlState state, gui_control::ControlVisualPart &part) noexcept;
			std::optional<graphics::fonts::text::TextBlockStyle>& GetStateStyle(gui_control::ControlState state, gui_control::ControlCaptionPart &part) noexcept;

			void SetPartState(gui_control::ControlState state, gui_control::ControlVisualPart &part) noexcept;
			void SetCaptionState(gui_control::ControlState state, gui_control::ControlCaptionPart &part) noexcept;
			void SetSkinState(gui_control::ControlState state, gui_control::ControlSkin &skin) noexcept;

			void SetState(gui_control::ControlState state) noexcept;


			/*
				Skins
			*/

			void AttachSkin();
			void DetachSkin() noexcept;
			void RemoveSkin() noexcept;

			virtual void UpdateCaption() noexcept;

		public:

			//Construct a control with the given name
			GuiControl(std::string name);

			//Construct a control with the given name and hit size
			GuiControl(std::string name, const Vector2 &size);

			//Construct a control with the given name and hit areas
			GuiControl(std::string name, gui_control::Areas areas);


			//Construct a control with the given name, caption, tooltip and skin
			GuiControl(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
				gui_control::ControlSkin skin);

			//Construct a control with the given name, caption, tooltip, skin and size
			GuiControl(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
				gui_control::ControlSkin skin, const Vector2 &size);

			//Construct a control with the given name, caption, tooltip, skin and hit areas
			GuiControl(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
				gui_control::ControlSkin skin, gui_control::Areas areas);


			//Virtual destructor
			virtual ~GuiControl() noexcept;


			/*
				Modifiers
			*/

			//Show this control
			void Show() noexcept;


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
					focusable_ = focusable;

					if (!focusable && focused_)
						Defocus();
				}
			}


			//Sets the caption text for this control to the given text
			inline void Caption(std::optional<std::string> text) noexcept
			{
				if (caption_ != text)
				{
					caption_ = std::move(text);
					UpdateCaption();
				}
			}

			//Sets the tooltip text (hint) for this control to the given text
			inline void Tooltip(std::optional<std::string> text) noexcept
			{
				tooltip_ = std::move(text);
			}
			

			//Sets the size of this control to the given size
			void Size(const Vector2 &size) noexcept;

			//Sets the caption size for this control to the given size
			inline void CaptionSize(const std::optional<Vector2> &size) noexcept
			{
				if (caption_size_ != size)
				{
					caption_size_ = size;
					UpdateCaption();
				}
			}

			//Sets the caption padding for this control to the given padding
			inline void CaptionPadding(const std::optional<Vector2> &padding) noexcept
			{
				if (caption_padding_ != padding)
				{
					caption_padding_ = padding;
					UpdateCaption();
				}
			}

			//Sets the caption layout for this control to the given layout
			inline void CaptionLayout(gui_control::ControlCaptionLayout layout) noexcept
			{
				if (caption_layout_ != layout)
				{
					caption_layout_ = layout;
					UpdateCaption();
				}
			}
			

			//Sets the skin for this control to the given skin
			void Skin(gui_control::ControlSkin skin) noexcept;

			//Sets the hit areas of this control to the given areas
			inline void HitAreas(gui_control::Areas areas) noexcept
			{
				hit_areas_ = std::move(areas);
			}

			//Sets the hit area of this control to the given area
			inline void HitArea(const Aabb &area) noexcept
			{
				hit_areas_.clear();
				hit_areas_.push_back(area);
			}


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


			//Sets the on resize callback
			inline void OnResize(events::Callback<void, GuiControl&> on_resize) noexcept
			{
				on_resize_ = on_resize;
			}

			//Sets the on resize callback
			inline void OnResize(std::nullopt_t) noexcept
			{
				on_resize_ = {};
			}


			//Sets the on state change callback
			inline void OnStateChange(events::Callback<void, GuiControl&> on_state_change) noexcept
			{
				on_state_change_ = on_state_change;
			}

			//Sets the on state change callback
			inline void OnStateChange(std::nullopt_t) noexcept
			{
				on_state_change_ = {};
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


			//Returns the caption text for this control
			//Returns nullopt if this control has no caption
			[[nodiscard]] inline auto& Caption() const noexcept
			{
				return caption_;
			}

			//Returns the tooltip text (hint) for this control
			//Returns nullopt if this control has no tooltip
			[[nodiscard]] inline auto& Tooltip() const noexcept
			{
				return tooltip_;
			}


			//Returns the size of this control
			[[nodiscard]] Vector2 Size() const noexcept;

			//Returns the caption size for this control
			//Returns nullopt if no custom caption size has been set
			[[nodiscard]] inline auto& CaptionSize() const noexcept
			{
				return caption_size_;
			}

			//Returns the caption padding size for this control
			//Returns nullopt if no custom caption padding has been set
			[[nodiscard]] inline auto& CaptionPadding() const noexcept
			{
				return caption_padding_;
			}

			//Returns the caption layout for this control
			[[nodiscard]] inline auto& CaptionLayout() const noexcept
			{
				return caption_layout_;
			}


			//Returns the current state of this control
			[[nodiscard]] inline auto State() const noexcept
			{
				return state_;
			}

			//Returns the skin attached to this control
			[[nodiscard]] inline auto& Skin() const noexcept
			{
				return skin_;
			}

			//Returns all of the hit areas of this control
			[[nodiscard]] inline auto& HitAreas() const noexcept
			{
				return hit_areas_;
			}

			//Returns the hit area of this control
			//Returns nullopt of this control has no hit area
			[[nodiscard]] std::optional<Aabb> HitArea() const noexcept;

			//Returns the visual area of this control
			//The returned area includes the center and border parts (or caption)
			//Returns nullopt of this control has no visuals
			[[nodiscard]] std::optional<Aabb> VisualArea() const noexcept;


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

			//Returns the on resize callback
			[[nodiscard]] inline auto OnResize() const noexcept
			{
				return on_resize_;
			}

			//Returns the on state change callback
			[[nodiscard]] inline auto OnStateChange() const noexcept
			{
				return on_state_change_;
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