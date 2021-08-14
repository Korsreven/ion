/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/graph/animations
File:	IonNodeAnimation.h
-------------------------------------------
*/

#ifndef ION_NODE_ANIMATION
#define ION_NODE_ANIMATION

#include <string>
#include <variant>
#include <vector>

#include "graphics/utilities/IonVector2.h"
#include "managed/IonManagedObject.h"
#include "types/IonCumulative.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene::graph::animations
{
	class NodeAnimationManager; //Forward declaration

	using namespace graphics::utilities;
	using namespace types::type_literals;

	namespace node_animation
	{
		enum class ActionToggleType
		{
			EnableAnimations,
			EnableParticleSystem,
			MuteSounds,
			NodeVisibility
		};

		enum class MotionTechniqueType
		{
			Linear,
			Sigmoid,
			Tanh,

			UpwardOpeningParabola,
			ThirdDegreeCurve,
			LogarithmicCurve,
			ExponentialCurve,
			BiologicalGrowthCurve,
			SineWave
		};
		

		namespace detail
		{
			struct moving_amount
			{
				MotionTechniqueType technique = MotionTechniqueType::Linear;
				types::Cumulative<real> value;
			};


			/*
				Actions
			*/

			struct action
			{
				duration time = 0.0_sec;
			};

			struct toggle_action : action
			{
				ActionToggleType type;
				bool cascade = true;
				bool value = false;
			};

			struct user_action : action
			{
				//callback
			};


			using action_types = std::variant<toggle_action, user_action>;
			using action_container = std::vector<action_types>;


			/*
				Motions
			*/

			struct motion
			{
				duration start_time = 0.0_sec;
				duration current_time = 0.0_sec;
				duration total_duration = 0.0_sec;
			};

			struct rotating_motion : motion
			{
				detail::moving_amount angle;
			};

			struct scaling_motion : motion
			{
				detail::moving_amount x;
				detail::moving_amount y;
			};

			struct translating_motion : motion
			{
				detail::moving_amount x;
				detail::moving_amount y;
				detail::moving_amount z;
			};

			struct volume_fader : motion
			{
				detail::moving_amount amount;
			};


			using motion_types = std::variant<rotating_motion, scaling_motion, translating_motion, volume_fader>;
			using motion_container = std::vector<motion_types>;
		} //detail
	} //node_animation


	class NodeAnimation final : public managed::ManagedObject<NodeAnimationManager>
	{
		private:

			duration current_time_ = 0.0_sec;
			duration total_duration_ = 0.0_sec;

			node_animation::detail::action_container actions_;
			node_animation::detail::motion_container motions_;

		public:

			using managed::ManagedObject<NodeAnimationManager>::ManagedObject;

			//Construct a new node animation with the given name
			NodeAnimation(std::string name) noexcept;


			/*
				Observers
			*/

			//Returns the current time of this node animation
			[[nodiscard]] inline auto CurrentTime() const noexcept
			{
				return current_time_;
			}

			//Returns the total duration of this node animation
			[[nodiscard]] inline auto TotalDuration() const noexcept
			{
				return total_duration_;
			}

			//Returns the total percent of this node animation
			[[nodiscard]] inline auto TotalPercent() const noexcept
			{
				return current_time_ / total_duration_;
			}


			/*
				Motions
			*/

			//Adds a rotation motion to this node animation with the given angle (in radians) and total duration
			void AddRotation(real angle, duration total_duration, duration start_time = 0.0_sec,
				node_animation::MotionTechniqueType technique = node_animation::MotionTechniqueType::Linear) noexcept;


			//Adds a scaling motion to this node animation with the given unit and total duration
			void AddScaling(const Vector2 &unit, duration total_duration, duration start_time = 0.0_sec,
				node_animation::MotionTechniqueType technique = node_animation::MotionTechniqueType::Linear) noexcept;

			//Adds a scaling motion to this node animation with the given unit and total duration
			void AddScaling(const Vector2 &unit, duration total_duration, duration start_time = 0.0_sec,
				node_animation::MotionTechniqueType technique_x = node_animation::MotionTechniqueType::Linear,
				node_animation::MotionTechniqueType technique_y = node_animation::MotionTechniqueType::Linear) noexcept;


			//Adds a translation motion to this node animation with the given unit and total duration
			void AddTranslation(const Vector2 &unit, duration total_duration, duration start_time = 0.0_sec,
				node_animation::MotionTechniqueType technique = node_animation::MotionTechniqueType::Linear) noexcept;

			//Adds a translation motion to this node animation with the given unit and total duration
			void AddTranslation(const Vector2 &unit, duration total_duration, duration start_time = 0.0_sec,
				node_animation::MotionTechniqueType technique_x = node_animation::MotionTechniqueType::Linear,
				node_animation::MotionTechniqueType technique_y = node_animation::MotionTechniqueType::Linear) noexcept;
	};
} //ion::graphics::scene::graph::animations

#endif