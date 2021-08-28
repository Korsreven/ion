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

#include <algorithm>
#include <string>
#include <variant>
#include <vector>

#include "graphics/utilities/IonVector2.h"
#include "graphics/utilities/IonVector3.h"
#include "managed/IonManagedObject.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene::graph
{
	class SceneNode; //Forward declaration
}

namespace ion::graphics::scene::graph::animations
{
	class NodeAnimationManager; //Forward declaration
	class NodeAnimationTimeline; //Forward declaration

	using namespace graphics::utilities;
	using namespace types::type_literals;

	namespace node_animation
	{
		enum class NodeActionType
		{
			//Visibility
			FlipVisibility,
			FlipVisibilityCascading,
			Show,
			ShowCascading,
			Hide,
			HideCascading,	

			//Transformation
			InheritRotation,
			InheritScaling,
			DisinheritRotation,
			DisinheritScaling
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
				real current = 0.0_r;
				real target = 0.0_r;
				MotionTechniqueType technique = MotionTechniqueType::Linear;
			};


			/*
				Actions
			*/

			struct action
			{
				duration time = 0.0_sec;
			};

			struct node_action : action
			{
				NodeActionType type = NodeActionType::FlipVisibilityCascading;
			};

			struct user_action : action
			{
				//callback
			};


			using action_types = std::variant<node_action, user_action>;
			using action_container = std::vector<action_types>;


			/*
				Motions
			*/

			struct motion
			{
				duration start_time = 0.0_sec;
				duration total_duration = 0.0_sec;
			};

			struct rotating_motion : motion
			{
				moving_amount angle;

				inline void Reset() noexcept
				{
					angle.current = 0.0_r;
				}
			};

			struct scaling_motion : motion
			{
				moving_amount x;
				moving_amount y;

				inline void Reset() noexcept
				{
					x.current = 0.0_r;
					y.current = 0.0_r;
				}
			};

			struct translating_motion : motion
			{
				moving_amount x;
				moving_amount y;
				moving_amount z;

				inline void Reset() noexcept
				{
					x.current = 0.0_r;
					y.current = 0.0_r;
					z.current = 0.0_r;
				}
			};


			using motion_types = std::variant<rotating_motion, scaling_motion, translating_motion>;
			using motion_container = std::vector<motion_types>;


			/*
				Actions
			*/

			inline auto upper_bound(const action_container &actions, duration time) noexcept
			{
				return std::upper_bound(std::begin(actions), std::end(actions), time,
					[](duration time, const action_types &a) noexcept
					{
						return std::visit([&](auto &&a) noexcept { return time < a.time; }, a);
					});
			}


			bool execute_action(action &a, duration time, duration current_time, duration start_time) noexcept;

			void execute_action(node_action &a, duration time, duration current_time, duration start_time, SceneNode &node) noexcept;
			void execute_action(user_action &a, duration time, duration current_time, duration start_time, SceneNode &node) noexcept;


			/*
				Motions
			*/

			inline auto upper_bound(const motion_container &motions, duration total_duration) noexcept
			{
				return std::upper_bound(std::begin(motions), std::end(motions), total_duration,
					[](duration total_duration, const motion_types &m) noexcept
					{
						return std::visit([&](auto &&m) noexcept { return total_duration < m.start_time + m.total_duration; }, m);
					});
			}


			real move_amount(moving_amount &value, real percent) noexcept;

			real elapse_motion(motion &m, duration time, duration current_time, duration start_time) noexcept;
			void elapse_motion(rotating_motion &m, duration time, duration current_time, duration start_time, SceneNode &node) noexcept;
			void elapse_motion(scaling_motion &m, duration time, duration current_time, duration start_time, SceneNode &node) noexcept;
			void elapse_motion(translating_motion &m, duration time, duration current_time, duration start_time, SceneNode &node) noexcept;
		} //detail
	} //node_animation


	class NodeAnimation final : public managed::ManagedObject<NodeAnimationManager>
	{
		private:

			duration total_duration_ = 0.0_sec;
			node_animation::detail::action_container actions_;
			node_animation::detail::motion_container motions_;


			duration RetrieveTotalDuration() const noexcept;

		public:

			using managed::ManagedObject<NodeAnimationManager>::ManagedObject;

			//Construct a new node animation with the given name
			NodeAnimation(std::string name) noexcept;


			/*
				Modifiers
			*/

			//Reset this node animation
			void Reset() noexcept;


			/*
				Observers
			*/

			//Returns the total duration of this node animation
			[[nodiscard]] inline auto TotalDuration() const noexcept
			{
				return total_duration_;
			}


			/*
				Elapse time
			*/

			//Elapse the total time for this node animation group by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time, duration current_time, duration start_time, SceneNode &node) noexcept;


			/*
				Playback
			*/

			//Returns a newly created timeline with this animation attached to it
			NonOwningPtr<NodeAnimationTimeline> Start(real playback_rate = 1.0_r, bool running = true);


			/*
				Actions
			*/

			//Adds an action to this node animation with the given type and execution time
			void AddAction(node_animation::NodeActionType type, duration time);

			//Clear all actions from this node animation
			void ClearActions() noexcept;


			/*
				Motions
			*/

			//Adds a rotation motion to this node animation with the given angle (in radians) and total duration
			void AddRotation(real angle, duration total_duration, duration start_time = 0.0_sec,
				node_animation::MotionTechniqueType technique = node_animation::MotionTechniqueType::Linear);


			//Adds a scaling motion to this node animation with the given unit and total duration
			void AddScaling(const Vector2 &unit, duration total_duration, duration start_time = 0.0_sec,
				node_animation::MotionTechniqueType technique = node_animation::MotionTechniqueType::Linear);

			//Adds a scaling motion to this node animation with the given unit and total duration
			void AddScaling(const Vector2 &unit, duration total_duration, duration start_time,
				node_animation::MotionTechniqueType technique_x,
				node_animation::MotionTechniqueType technique_y);


			//Adds a translation motion to this node animation with the given unit and total duration
			void AddTranslation(const Vector3 &unit, duration total_duration, duration start_time = 0.0_sec,
				node_animation::MotionTechniqueType technique = node_animation::MotionTechniqueType::Linear);

			//Adds a translation motion to this node animation with the given unit and total duration
			void AddTranslation(const Vector3 &unit, duration total_duration, duration start_time,
				node_animation::MotionTechniqueType technique_x,
				node_animation::MotionTechniqueType technique_y,
				node_animation::MotionTechniqueType technique_z);


			//Clear all motions from this node animation
			void ClearMotions() noexcept;


			/*
				Actions / motions
			*/

			//Clear all actions and motions from this node animation
			void Clear() noexcept;
	};
} //ion::graphics::scene::graph::animations

#endif