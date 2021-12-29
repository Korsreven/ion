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

		enum class InterpolationType
		{
			Forward,
			Backward,
			Bidirectional
		};

		struct ProgressBarSkin : gui_control::ControlSkin
		{
			gui_control::ControlVisualPart Bar;
			gui_control::ControlVisualPart BarInterpolated;
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


			void resize_skin(ProgressBarSkin &skin, const Vector2 &from_size, const Vector2 &to_size) noexcept;
		} //detail
	} //gui_progress_bar


	class GuiProgressBar : public GuiControl
	{
		private:

			void DefaultSetup() noexcept;

		protected:

			gui_progress_bar::ProgressBarType type_ = gui_progress_bar::ProgressBarType::Horizontal;
			bool flipped_ = false;
			types::Progress<real> progress_;
			real snap_interval_ = 1.0_r;

			gui_progress_bar::InterpolationType interpolation_type_ = gui_progress_bar::InterpolationType::Bidirectional;
			duration interpolation_time_ = gui_progress_bar::detail::default_interpolation_time;
			duration interpolation_delay_ = gui_progress_bar::detail::default_interpolation_delay;

			gui_progress_bar::detail::interpolation_phase phase_ = gui_progress_bar::detail::interpolation_phase::PreInterpolate;
			types::Cumulative<duration> phase_duration_{interpolation_delay_};


			/*
				Events
			*/

			//See GuiControl::Resized for more details
			virtual void Resized(Vector2 from_size, Vector2 to_size) noexcept override;

			//Called right after a progress bar has progressed
			virtual void Progressed(real delta) noexcept;


			/*
				States
			*/

			void SetSkinState(gui_control::ControlState state, gui_progress_bar::ProgressBarSkin &skin) noexcept;
			virtual void SetState(gui_control::ControlState state) noexcept override;


			/*
				Skins
			*/

			void RotateBars() noexcept;
			void RotateSkin() noexcept;

			virtual void UpdateBars() noexcept;


			/*
				Phase
			*/

			void SetPhase(gui_progress_bar::detail::interpolation_phase phase) noexcept;
			void UpdatePhaseDuration() noexcept;

		public:

			//Construct a progress bar with the given name, caption, skin and type
			GuiProgressBar(std::string name, std::optional<std::string> caption, OwningPtr<gui_progress_bar::ProgressBarSkin> skin,
				gui_progress_bar::ProgressBarType type = gui_progress_bar::ProgressBarType::Horizontal);

			//Construct a progress bar with the given name, caption, tooltip, skin, size and type
			GuiProgressBar(std::string name, std::optional<std::string> caption, OwningPtr<gui_progress_bar::ProgressBarSkin> skin, const Vector2 &size,
				gui_progress_bar::ProgressBarType type = gui_progress_bar::ProgressBarType::Horizontal);


			/*
				Modifiers
			*/

			//Sets the progress bar type to the given type
			//Horizontal type goes left to right
			//Vertical type goes bottom to top
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

			//Sets whether or not this progress bar is flipped or not
			//Horizontal flipped goes right to left
			//Vertical flipped goes top to bottom
			inline void Flipped(bool flipped) noexcept
			{
				if (flipped_ != flipped)
				{
					flipped_ = flipped;
					UpdateBars();
				}
			}


			//Sets the position to the given percentage in range [0.0, 1.0]
			void Percent(real percent) noexcept;

			//Sets the position of this progress bar to the given value
			inline void Position(real position) noexcept
			{
				if (progress_.Position() != position)
				{
					auto pos = progress_.Position();
					progress_.Position(position);		
					Progressed(position - pos);
					UpdateBars();
				}
			}

			//Sets the range of this progress bar to the given range
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

			//Sets the snap by interval for this progress bar to the given interval
			inline void SnapInterval(real interval) noexcept
			{
				snap_interval_ = interval >= 0.0_r ? interval : 0.0_r;
			}


			//Sets the interpolation time for this progress bar to the given time
			inline void InterpolationTime(duration time) noexcept
			{
				if (interpolation_time_ != time && time >= 0.0_sec)
				{
					interpolation_time_ = time;
					UpdatePhaseDuration();
				}
			}

			//Sets the interpolation delay for this progress bar to the given time
			inline void InterpolationDelay(duration time) noexcept
			{
				if (interpolation_delay_ != time && time >= 0.0_sec)
				{
					interpolation_delay_ = time;
					UpdatePhaseDuration();
				}
			}


			/*
				Observers
			*/

			//Returns the progress bar type
			//Horizontal type goes left to right
			//Vertical type goes bottom to top
			[[nodiscard]] inline auto Type() const noexcept
			{
				return type_;
			}

			//Returns true if this progress bar is flipped
			//Horizontal flipped goes right to left
			//Vertical flipped goes top to bottom
			[[nodiscard]] inline auto Flipped() const noexcept
			{
				return flipped_;
			}


			//Returns the position as a percentage in range [0.0, 1.0]
			[[nodiscard]] inline auto Percent() const noexcept
			{
				return progress_.Percent();
			}

			//Returns the position of this progress bar
			[[nodiscard]] inline auto Position() const noexcept
			{
				return progress_.Position();
			}		

			//Returns the range of this progress bar
			[[nodiscard]] inline auto Range() const noexcept
			{
				return progress_.MinMax();
			}

			//Returns the snap interval for this progress bar
			[[nodiscard]] inline auto SnapInterval() const noexcept
			{
				return snap_interval_;
			}


			//Returns the interpolation time for this progress bar to the given time
			[[nodiscard]] inline auto InterpolationTime() const noexcept
			{
				return interpolation_time_;
			}

			//Returns the interpolation delay for this progress bar to the given time
			[[nodiscard]] inline auto InterpolationDelay() const noexcept
			{
				return interpolation_delay_;
			}


			/*
				Frame events
			*/

			//Called from gui control when a frame has started
			virtual void FrameStarted(duration time) noexcept override;
	};

} //ion::gui::controls

#endif