/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiSlider.h
-------------------------------------------
*/

#ifndef ION_GUI_SLIDER_H
#define ION_GUI_SLIDER_H

#include <optional>
#include <string>

#include "IonGuiControl.h"
#include "events/listeners/IonKeyListener.h"
#include "events/listeners/IonMouseListener.h"
#include "graphics/utilities/IonVector2.h"
#include "memory/IonOwningPtr.h"
#include "types/IonProgress.h"
#include "types/IonTypes.h"

namespace ion::gui::controls
{
	using namespace events::listeners;
	using namespace graphics::utilities;

	namespace gui_slider
	{
		enum class SliderType : bool
		{
			Horizontal, //Left to right
			Vertical //Bottom to top
		};

		struct SliderSkin : gui_control::ControlSkin
		{
			gui_control::ControlSkinPart Handle;

			//Copy from most derived
			virtual void Assign(const ControlSkin &control_skin) noexcept
			{
				if (auto skin = dynamic_cast<const SliderSkin*>(&control_skin); skin)
					*this = *skin;
				else //Try less derived
					return ControlSkin::Assign(control_skin);
			}
		};


		namespace detail
		{
			inline auto flipped_arrow_key(KeyButton button) noexcept
			{
				switch (button)
				{
					case KeyButton::UpArrow:
					return KeyButton::DownArrow;

					case KeyButton::LeftArrow:
					return KeyButton::RightArrow;

					case KeyButton::DownArrow:
					return KeyButton::UpArrow;

					case KeyButton::RightArrow:
					return KeyButton::LeftArrow;

					default:
					return button;
				}
			}

			inline auto flip_arrow_keys(KeyButton button) noexcept
			{
				switch (button)
				{
					case KeyButton::UpArrow:
					case KeyButton::RightArrow:
					case KeyButton::DownArrow:
					case KeyButton::LeftArrow:
					return flipped_arrow_key(button);

					default:
					return button;
				}
			}


			/**
				@name Skins
				@{
			*/

			void resize_handle(SliderSkin &skin, const Vector2 &from_size, const Vector2 &to_size) noexcept;
			void resize_skin(SliderSkin &skin, SliderType type, const Vector2 &from_size, const Vector2 &to_size) noexcept;

			///@}
		} //detail
	} //gui_slider


	///@brief A class representing a GUI slider that can be horizontal or vertical, flipped or not
	///@details The slider handle can be dragged with the mouse cursor to change value
	class GuiSlider : public GuiControl
	{
		private:

			void DefaultSetup() noexcept;

		protected:

			gui_slider::SliderType type_ = gui_slider::SliderType::Horizontal;
			bool flipped_ = false;
			types::Progress<int> progress_;
			int step_by_amount_ = 1;

			bool dragged_ = false;


			/**
				@name Events
				@{
			*/

			///@brief See GuiControl::Resized for more details
			virtual void Resized(Vector2 from_size, Vector2 to_size) noexcept override;

			///@brief Called right after a slider has slid
			virtual void Slid(int delta) noexcept;

			///@}

			/**
				@name States
				@{
			*/

			void SetSkinState(gui_control::ControlState state, gui_slider::SliderSkin &skin) noexcept;
			virtual void SetState(gui_control::ControlState state) noexcept override;

			///@}

			/**
				@name Skins
				@{
			*/

			virtual OwningPtr<gui_control::ControlSkin> AttuneSkin(OwningPtr<gui_control::ControlSkin> skin) const override;

			void RotateHandle() noexcept;
			void RotateSkin() noexcept;

			virtual void UpdateHandle() noexcept;

			///@}

		public:

			///@brief Constructs a slider with the given name, size, caption, tooltip, type and hit boxes
			GuiSlider(std::string name, const std::optional<Vector2> &size,
				std::optional<std::string> caption, std::optional<std::string> tooltip, gui_slider::SliderType type = gui_slider::SliderType::Horizontal,
				gui_control::BoundingBoxes hit_boxes = {}) noexcept;

			///@brief Constructs a slider with the given name, skin, size, caption, tooltip, type and hit boxes
			GuiSlider(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
				std::optional<std::string> caption, std::optional<std::string> tooltip, gui_slider::SliderType type = gui_slider::SliderType::Horizontal,
				gui_control::BoundingBoxes hit_boxes = {});


			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the slider type to the given type
			///@details Horizontal type goes left to right.
			///Vertical type goes bottom to top
			inline void Type(gui_slider::SliderType type) noexcept
			{
				if (type_ != type)
				{
					RotateHandle();
					type_ = type;
					RotateSkin();
					UpdateHandle();
				}
			}

			///@brief Sets whether or not this slider is flipped or not
			///@details Horizontal flipped goes right to left.
			///Vertical flipped goes top to bottom
			inline void Flipped(bool flipped) noexcept
			{
				if (flipped_ != flipped)
				{
					flipped_ = flipped;
					UpdateHandle();
				}
			}


			///@brief Sets the value to the given percentage in range [0.0, 1.0]
			void Percent(real percent) noexcept;

			///@brief Sets the value of this slider to the given value
			inline void Value(int value) noexcept
			{
				if (progress_.Value() != value)
				{
					auto val = progress_.Value();
					progress_.Value(value);
					Slid(value - val);
					UpdateHandle();
				}
			}

			///@brief Sets the range of this slider to the given range
			inline void Range(int min, int max) noexcept
			{
				if (progress_.Min() != min || progress_.Max() != max)
				{
					//Go from or to an empty range
					auto empty_range =
						progress_.Min() == progress_.Max() || min == max;

					progress_.Extents(min, max);

					if (empty_range)
						SetState(state_);

					UpdateHandle();
				}
			}

			///@brief Sets the step by amount for this slider to the given amount
			inline void StepByAmount(int amount) noexcept
			{
				step_by_amount_ = amount > 0 ? amount : 1;
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the slider type
			///@details Horizontal type goes left to right.
			///Vertical type goes bottom to top
			[[nodiscard]] inline auto Type() const noexcept
			{
				return type_;
			}

			///@brief Returns true if this slider is flipped
			///@details Horizontal flipped goes right to left.
			///Vertical flipped goes top to bottom
			[[nodiscard]] inline auto Flipped() const noexcept
			{
				return flipped_;
			}


			///@brief Returns the value as a percentage in range [0.0, 1.0]
			[[nodiscard]] inline auto Percent() const noexcept
			{
				return progress_.Percent();
			}

			///@brief Returns the value of this slider
			[[nodiscard]] inline auto Value() const noexcept
			{
				return progress_.Value();
			}		

			///@brief Returns the range of this slider
			[[nodiscard]] inline auto Range() const noexcept
			{
				return progress_.MinMax();
			}

			///@brief Returns the step by amount for this slider
			[[nodiscard]] inline auto StepByAmount() const noexcept
			{
				return step_by_amount_;
			}

			///@}

			/**
				@name Intersecting
				@{
			*/

			///@brief Returns true if the given point intersects with the slider handle
			[[nodiscard]] bool IntersectsHandle(const Vector2 &point) const noexcept;

			///@}

			/**
				@name Key events
				@{
			*/

			///@brief Called from gui control when a key button has been released
			///@details Returns true if the key release event has been consumed by the control
			virtual bool KeyReleased(KeyButton button) noexcept override;

			///@}

			/**
				@name Mouse events
				@{
			*/

			///@brief Called from gui control when the mouse button has been pressed
			///@details Returns true if the mouse press event has been consumed by the control
			virtual bool MousePressed(MouseButton button, Vector2 position) noexcept override;

			///@brief Called from gui control when the mouse button has been released
			///@details Returns true if the mouse release event has been consumed by the control
			virtual bool MouseReleased(MouseButton button, Vector2 position) noexcept override;

			///@brief Called from gui control when the mouse has been moved
			///@details Returns true if the mouse move event has been consumed by the control
			virtual bool MouseMoved(Vector2 position) noexcept override;

			///@brief Called from gui control when the mouse wheel has been rolled
			///@details Returns true if the mouse wheel roll event has been consumed by the control
			virtual bool MouseWheelRolled(int delta, Vector2 position) noexcept override;

			///@}
	};

} //ion::gui::controls

#endif