/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiProgressBar.h
-------------------------------------------
*/

#ifndef ION_GUI_PROGRESS_BAR_H
#define ION_GUI_PROGRESS_BAR_H

#include <optional>
#include <string>

#include "IonGuiControl.h"
#include "graphics/utilities/IonVector2.h"
#include "memory/IonOwningPtr.h"
#include "types/IonCumulative.h"
#include "types/IonProgress.h"
#include "types/IonTypes.h"

namespace ion::gui::controls
{
	using namespace events::listeners;
	using namespace graphics::utilities;
	using namespace types::type_literals;

	namespace gui_progress_bar
	{
		enum class ProgressBarType : bool
		{
			Horizontal, //Left to right
			Vertical //Bottom to top
		};

		enum class BarInterpolationType
		{
			Forward,
			Backward,
			Bidirectional
		};

		struct ProgressBarSkin : gui_control::ControlSkin
		{
			gui_control::ControlSkinPart Bar;
			gui_control::ControlSkinPart BarInterpolated;

			//Copy from most derived
			virtual void Assign(const ControlSkin &control_skin) noexcept
			{
				if (auto skin = dynamic_cast<const ProgressBarSkin*>(&control_skin); skin)
					*this = *skin;
				else //Try less derived
					return ControlSkin::Assign(control_skin);
			}

			virtual void GetParts(gui_control::SkinPartPointers &parts, std::string_view name) const override;
		};


		namespace detail
		{
			enum class interpolation_phase : bool
			{
				PreInterpolate,
				Interpolate
			};

			constexpr auto default_interpolation_time = 1.0_sec;
			constexpr auto default_interpolation_delay = 0.5_sec;


			/**
				@name Skins
				@{
			*/

			void resize_bar(gui_control::ControlSkinPart &bar, const Vector2 &delta_size) noexcept;
			void crop_bar(gui_control::ControlSkinPart &bar, ProgressBarType type, bool flipped, real percent) noexcept;
			void update_bar(gui_control::ControlSkinPart &bar, ProgressBarType type, bool flipped, real percent, const Aabb &area) noexcept;

			void resize_skin(ProgressBarSkin &skin, const Vector2 &from_size, const Vector2 &to_size) noexcept;

			///@}
		} //detail
	} //gui_progress_bar


	///@brief A class representing a GUI progress bar that can be horizontal or vertical, flipped or not
	///@details A progress bar can contain two bars, the normal bar, and a bar which can be interpolated forward, backward or bidirectional
	class GuiProgressBar : public GuiControl
	{
		private:

			void DefaultSetup() noexcept;

		protected:

			gui_progress_bar::ProgressBarType type_ = gui_progress_bar::ProgressBarType::Horizontal;
			bool flipped_ = false;
			types::Progress<real> progress_;

			gui_progress_bar::BarInterpolationType interpolation_type_ = gui_progress_bar::BarInterpolationType::Bidirectional;
			duration interpolation_time_ = gui_progress_bar::detail::default_interpolation_time;
			duration interpolation_delay_ = gui_progress_bar::detail::default_interpolation_delay;

			gui_progress_bar::detail::interpolation_phase phase_ = gui_progress_bar::detail::interpolation_phase::PreInterpolate;
			types::Cumulative<duration> phase_duration_{interpolation_delay_};
			std::optional<real> interpolated_percent_;


			/**
				@name Events
				@{
			*/

			///@brief See GuiControl::Resized for more details
			virtual void Resized(Vector2 from_size, Vector2 to_size) noexcept override;

			///@brief Called right after a progress bar has progressed
			virtual void Progressed(real delta) noexcept;

			///@}

			/**
				@name States
				@{
			*/

			void SetSkinState(gui_control::ControlState state, gui_progress_bar::ProgressBarSkin &skin) noexcept;
			virtual void SetState(gui_control::ControlState state) noexcept override;

			///@}

			/**
				@name Skins
				@{
			*/

			virtual OwningPtr<gui_control::ControlSkin> AttuneSkin(OwningPtr<gui_control::ControlSkin> skin) const override;

			void RotateBars() noexcept;
			void RotateSkin() noexcept;

			void UpdateBar() noexcept;
			void UpdateBarInterpolated() noexcept;
			void UpdateBars() noexcept;

			///@}

			/**
				@name Phase
				@{
			*/

			void SetPhase(gui_progress_bar::detail::interpolation_phase phase) noexcept;
			void UpdatePhaseDuration() noexcept;

			void StartInterpolation(real from_percent) noexcept;

			///@}

		public:

			///@brief Constructs a progress bar with the given name, size, caption and type
			GuiProgressBar(std::string name, const std::optional<Vector2> &size,
				std::optional<std::string> caption, gui_progress_bar::ProgressBarType type = gui_progress_bar::ProgressBarType::Horizontal) noexcept;

			///@brief Constructs a progress bar with the given name, skin, size, caption and type
			GuiProgressBar(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
				std::optional<std::string> caption, gui_progress_bar::ProgressBarType type = gui_progress_bar::ProgressBarType::Horizontal);


			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the progress bar type to the given type
			///@details Horizontal type goes left to right.
			///Vertical type goes bottom to top
			inline void Type(gui_progress_bar::ProgressBarType type) noexcept
			{
				if (type_ != type)
				{
					RotateBars();
					type_ = type;
					RotateSkin();
					UpdateBars();
				}
			}

			///@brief Sets whether or not this progress bar is flipped or not
			///@details Horizontal flipped goes right to left.
			///Vertical flipped goes top to bottom
			inline void Flipped(bool flipped) noexcept
			{
				if (flipped_ != flipped)
				{
					flipped_ = flipped;
					UpdateBars();
				}
			}


			///@brief Sets the value to the given percentage in range [0.0, 1.0]
			void Percent(real percent) noexcept;

			///@brief Sets the value of this progress bar to the given value
			inline void Value(real value) noexcept
			{
				if (auto [min, max] = progress_.MinMax();
					progress_.Value() != value &&
					(value >= min || progress_.Value() > min) &&
					(value <= max || progress_.Value() < max))
				{
					auto val = progress_.Value();
					auto percent = progress_.Percent();
					progress_.Value(value);
					Progressed(value - val);
					UpdateBar();
					StartInterpolation(percent);
				}
			}

			///@brief Sets the range of this progress bar to the given range
			inline void Range(real min, real max) noexcept
			{
				if (progress_.Min() != min || progress_.Max() != max)
				{
					//Go from or to an empty range
					auto empty_range =
						progress_.Min() == progress_.Max() || min == max;

					progress_.Extents(min, max);

					if (empty_range)
						SetState(state_);

					UpdateBars();
				}
			}


			///@brief Sets the interpolation type for this progress bar to the given type
			inline void InterpolationType(gui_progress_bar::BarInterpolationType type) noexcept
			{
				if (interpolation_type_ != type)
				{
					interpolation_type_ = type;

					if (interpolated_percent_)
						StartInterpolation(*interpolated_percent_);
				}
			}

			///@brief Sets the interpolation time for this progress bar to the given time
			inline void InterpolationTime(duration time) noexcept
			{
				if (interpolation_time_ != time && time >= 0.0_sec)
				{
					interpolation_time_ = time;
					UpdatePhaseDuration();
				}
			}

			///@brief Sets the interpolation delay for this progress bar to the given time
			inline void InterpolationDelay(duration time) noexcept
			{
				if (interpolation_delay_ != time && time >= 0.0_sec)
				{
					interpolation_delay_ = time;
					UpdatePhaseDuration();
				}
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the progress bar type
			///@details Horizontal type goes left to right.
			///Vertical type goes bottom to top
			[[nodiscard]] inline auto Type() const noexcept
			{
				return type_;
			}

			///@brief Returns true if this progress bar is flipped
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

			///@brief Returns the value of this progress bar
			[[nodiscard]] inline auto Value() const noexcept
			{
				return progress_.Value();
			}		

			///@brief Returns the range of this progress bar
			[[nodiscard]] inline auto Range() const noexcept
			{
				return progress_.MinMax();
			}


			///@brief Returns the interpolation type for this progress bar
			[[nodiscard]] inline auto InterpolationType() const noexcept
			{
				return interpolation_type_;
			}

			///@brief Returns the interpolation time for this progress bar to the given time
			[[nodiscard]] inline auto InterpolationTime() const noexcept
			{
				return interpolation_time_;
			}

			///@brief Returns the interpolation delay for this progress bar to the given time
			[[nodiscard]] inline auto InterpolationDelay() const noexcept
			{
				return interpolation_delay_;
			}


			///@brief Returns the interpolated value as a percentage in range [0.0, 1.0]
			[[nodiscard]] real InterpolatedPercent() const noexcept;

			///@brief Returns the interpolated value of this progress bar
			[[nodiscard]] real InterpolatedValue() const noexcept;

			///@}

			/**
				@name Frame events
				@{
			*/

			///@brief Called from gui control when a frame has started
			virtual void FrameStarted(duration time) noexcept override;

			///@}
	};

} //ion::gui::controls

#endif