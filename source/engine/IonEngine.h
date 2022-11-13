/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	engine
File:	IonEngine.h
-------------------------------------------
*/

#ifndef ION_ENGINE_H
#define ION_ENGINE_H

#include <optional>
#include <string>
#include <string_view>

#include "events/IonInputController.h"
#include "events/IonListenable.h"
#include "events/listeners/IonFrameListener.h"
#include "graphics/render/IonRenderWindow.h"
#include "graphics/scene/graph/IonSceneGraph.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"
#include "timers/IonStopwatch.h"
#include "timers/IonTimerManager.h"
#include "types/IonTypes.h"
#include "utilities/IonMath.h"

namespace ion
{
	using namespace types::type_literals;

	namespace engine
	{
		enum class VSyncMode
		{
			On,
			Off,
			Adaptive,
			AdaptiveHalfRate
		};

		namespace detail
		{
			constexpr auto meters_to_feet_factor = 3.28084_r;
			constexpr auto feet_to_meters_factor = 0.3048_r;


			bool init_file_system() noexcept;
			bool init_graphics() noexcept;

			void set_swap_interval(int mode) noexcept;
			std::optional<int> get_swap_interval() noexcept;

			void wait_for(duration seconds) noexcept;
		} //detail
	} //engine


	///@brief The class that creates the render window with a default viewport and starts the rendering loop
	class Engine final :
		protected events::Listenable<events::listeners::FrameListener>,	
		public managed::ObjectManager<graphics::scene::graph::SceneGraph, Engine>
	{
		private:

			using FrameEventsBase = events::Listenable<events::listeners::FrameListener>;
			using SceneGraphBase = managed::ObjectManager<graphics::scene::graph::SceneGraph, Engine>;


			bool initialized_ = false;
			timers::Stopwatch frame_stopwatch_;
			timers::Stopwatch total_stopwatch_;
			std::optional<duration> target_frame_time_;

			std::optional<graphics::render::RenderWindow> render_window_;
			std::optional<events::InputController> input_controller_;
			timers::TimerManager timer_manager_;

			static inline auto pixels_per_unit_ = 1.0_r;
			static inline auto units_per_meter_ = 1.0_r;
			static inline auto z_epsilon_ = 0.001_r;

			static inline graphics::scene::graph::SceneGraph *active_scene_graph_ = nullptr;


			/**
				@name Notifying
				@{
			*/

			bool NotifyFrameStarted(duration time) noexcept;
			bool NotifyFrameEnded(duration time) noexcept;

			bool UpdateFrame(duration time) noexcept;

			///@}

		public:

			///@brief Default constructor
			Engine() = default;

			///@brief Deleted copy constructor
			Engine(const Engine&) = delete;

			///@brief Default move constructor
			Engine(Engine&&) = default;


			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			Engine& operator=(const Engine&) = delete;

			///@brief Default move assignment
			Engine& operator=(Engine&&) = default;

			///@}

			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all scene graphs in the engine
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto SceneGraphs() noexcept
			{
				return SceneGraphBase::Objects();
			}

			///@brief Returns an immutable range of all scene graphs in the engine
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto SceneGraphs() const noexcept
			{
				return SceneGraphBase::Objects();
			}

			///@}

			/**
				@name Events
				@{
			*/

			///@brief Returns a mutable reference to the frame listener
			[[nodiscard]] inline auto& FrameEvents() noexcept
			{
				return static_cast<FrameEventsBase&>(*this);
			}

			///@brief Returns an immutable reference to the frame listener
			[[nodiscard]] inline auto& FrameEvents() const noexcept
			{
				return static_cast<const FrameEventsBase&>(*this);
			}

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the target FPS (frames per second) the engine should use
			inline void TargetFPS(std::optional<int> fps) noexcept
			{
				if (fps && *fps > 0)
					target_frame_time_ = 1.0_sec / *fps;
				else
					target_frame_time_ = {};
			}

			///@brief Sets if the engine should use vertical sync or not by the given value
			void VerticalSync(bool vsync) noexcept;

			///@brief Sets the kind of vertical sync the engine should use to the given mode
			void VerticalSync(engine::VSyncMode mode) noexcept;


			///@brief Sets the pixels per unit (PPU) the engine should use (default is 1.0)
			static inline void PixelsPerUnit(real pixels) noexcept
			{
				if (pixels > 0.0_r)
					pixels_per_unit_ = pixels;
			}

			///@brief Sets the units per meter the engine should use for distance measurements (default is 1.0)
			///@details This is mostly used when initializing the sound system (to set distance factor)
			static inline void UnitsPerMeter(real units) noexcept
			{
				if (units > 0.0_r)
					units_per_meter_ = units;
			}

			///@brief Sets the units per foot the engine should use for distance measurements (default is 0.3048)
			///@details This is mostly used when initializing the sound system (to set distance factor)
			static inline void UnitsPerFoot(real units) noexcept
			{
				UnitsPerMeter(units * engine::detail::meters_to_feet_factor);
			}

			///@brief Sets the expected z limit in range [from, to]
			///@details This is used for calculating which z epsilon the engine should use
			static void ZLimit(real from, real to) noexcept;

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the target FPS (frames per second) the engine is using
			///@details Returns nullopt if no target FPS has been set
			[[nodiscard]] inline auto TargetFPS() const noexcept -> std::optional<int>
			{
				if (target_frame_time_)
					return static_cast<int>(utilities::math::Round(1.0_sec / *target_frame_time_));
				else
					return {};
			}

			///@brief Returns the kind of vertical sync the engine is using
			///@details Returns nullopt if vertical sync mode is unknown
			[[nodiscard]] std::optional<engine::VSyncMode> VerticalSync() const noexcept;


			///@brief Returns a pointer to a mutable render window
			///@details Returns nullptr if the engine is not rendering to a target
			[[nodiscard]] inline auto Target() noexcept
			{
				return render_window_ ? &*render_window_ : nullptr;
			}

			///@brief Returns a pointer to an immutable render window
			///@details Returns nullptr if the engine is not rendering to a target
			[[nodiscard]] inline auto Target() const noexcept
			{
				return render_window_ ? &*render_window_ : nullptr;
			}


			///@brief Returns a pointer to a mutable input controller
			///@details Returns nullptr if the engine has no input controller (missing rendering target)
			[[nodiscard]] inline auto Input() noexcept
			{
				return input_controller_ ? &*input_controller_ : nullptr;
			}

			///@brief Returns a pointer to an immutable input controller
			///@details Returns nullptr if the engine has no input controller (missing rendering target)
			[[nodiscard]] inline auto Input() const noexcept
			{
				return input_controller_ ? &*input_controller_ : nullptr;
			}


			///@brief Returns a mutable reference to a timer manager containing syncronized timers
			[[nodiscard]] inline auto& SyncedTimers() noexcept
			{
				return timer_manager_;
			}

			///@brief Returns an immutable reference to a timer manager containing syncronized timers
			[[nodiscard]] inline auto& SyncedTimers() const noexcept
			{
				return timer_manager_;
			}


			///@brief Returns the pixels per unit (PPU) the engine should use (default is 1.0)
			[[nodiscard]] static inline auto PixelsPerUnit() noexcept
			{
				return pixels_per_unit_;
			}

			///@brief Returns the units per meter the engine should use for distance measurements (default is 1.0)
			///@details This is mostly used when initializing the sound system (to set distance factor)
			[[nodiscard]] static inline auto UnitsPerMeter() noexcept
			{
				return units_per_meter_;
			}

			///@brief Returns the units per foot the engine should use for distance measurements (default is 0.3048)
			///@details This is mostly used when initializing the sound system (to set distance factor)
			[[nodiscard]] static inline auto UnitsPerFoot() noexcept
			{
				return units_per_meter_ * engine::detail::feet_to_meters_factor;
			}

			///@brief Returns the z epsilon the engine should use (default is 0.001)
			///@details This is mostly used when initializing primitives above or below other primitives
			[[nodiscard]] static inline auto ZEpsilon() noexcept
			{
				return z_epsilon_;
			}

			///@}

			/**
				@name Engine
				@{
			*/

			///@brief Returns true if all extensions, internal and external requirements where initialized succesfully
			[[nodiscard]] bool Initialize() noexcept;

			///@brief Starts the rendering loop
			///@details Returns 0 if everything went fine
			[[nodiscard]] int Start() noexcept;

			///@}

			/**
				@name Timing
				@{
			*/

			///@brief Returns the last frame time
			[[nodiscard]] duration FrameTime() const noexcept;

			///@brief Returns the total running time
			[[nodiscard]] duration TotalTime() const noexcept;

			///@brief Returns the FPS
			[[nodiscard]] real FPS() const noexcept;


			///@brief Returns true if the engine is running
			[[nodiscard]] bool Running() const noexcept;

			///@}

			/**
				@name Rendering target
				@{
			*/

			///@brief Renders to the given render window, and create a default viewport
			graphics::render::RenderWindow& RenderTo(graphics::render::RenderWindow &&render_window) noexcept;

			///@}

			/**
				@name Scene graphs
				Creating
				@{
			*/

			///@brief Creates a scene graph with the given name and whether or not is should be enabled
			NonOwningPtr<graphics::scene::graph::SceneGraph> CreateSceneGraph(std::optional<std::string> name = {}, bool enabled = true);

			///@}

			/**
				@name Scene graphs
				Retrieving
				@{
			*/

			///@brief Gets a pointer to a mutable scene graph with the given name
			///@details Returns nullptr if scene graph could not be found
			[[nodiscard]] NonOwningPtr<graphics::scene::graph::SceneGraph> GetSceneGraph(std::string_view name) noexcept;

			///@brief Gets a pointer to an immutable scene graph with the given name
			///@details Returns nullptr if scene graph could not be found
			[[nodiscard]] NonOwningPtr<const graphics::scene::graph::SceneGraph> GetSceneGraph(std::string_view name) const noexcept;


			///@brief Gets a pointer to the currently active scene graph
			///@details Returns nullptr if there is no currently active scene graph
			[[nodiscard]] static inline auto GetActiveSceneGraph() noexcept
			{
				return active_scene_graph_;
			}

			///@}

			/**
				@name Scene graphs
				Removing
				@{
			*/

			///@brief Clears all removable scene graphs from the engine
			void ClearSceneGraphs() noexcept;

			///@brief Removes a removable scene graph from the engine
			bool RemoveSceneGraph(graphics::scene::graph::SceneGraph &scene_graph) noexcept;

			///@brief Removes a removable scene graph with the given name from the engine
			bool RemoveSceneGraph(std::string_view name) noexcept;

			///@}

			/**
				@name Viewport
				Retrieving
				@{
			*/

			///@brief Gets a pointer to the default viewport used by the engine
			///@details Returns nullptr if no default viewport could be found
			[[nodiscard]] NonOwningPtr<graphics::render::Viewport> GetDefaultViewport() noexcept;

			///@brief Gets a pointer to the default viewport used by the engine
			///@details Returns nullptr if no default viewport could be found
			[[nodiscard]] NonOwningPtr<const graphics::render::Viewport> GetDefaultViewport() const noexcept;

			///@}
	};
} //ion
#endif