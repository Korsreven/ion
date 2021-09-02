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

#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "events/IonCallback.h"
#include "graphics/utilities/IonVector2.h"
#include "graphics/utilities/IonVector3.h"
#include "managed/IonManagedObject.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene::graph::animations
{
	class NodeAnimation; //Forward declaration
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

				inline auto operator<(const action &rhs) const noexcept
				{
					return time < rhs.time;
				}
			};

			struct node_action : action
			{
				NodeActionType type = NodeActionType::FlipVisibilityCascading;
			};

			struct user_action : action
			{
				events::Callback<void, NodeAnimation&, duration> on_execute;
			};


			using action_types = std::variant<node_action, user_action>;
			using action_container = std::vector<action_types>;

			struct action_types_comparator
			{
				inline auto operator()(const action_types &x, const action_types &y) const noexcept
				{
					return std::visit([](auto &&a) noexcept -> const action& { return a; }, x) <
						   std::visit([](auto &&a) noexcept -> const action& { return a; }, y);
				}
			};


			/*
				Motions
			*/

			struct motion
			{
				duration start_time = 0.0_sec;
				duration total_duration = 0.0_sec;

				inline auto operator<(const motion &rhs) const noexcept
				{
					return start_time + total_duration < rhs.start_time + rhs.total_duration;
				}
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

			struct user_motion : motion
			{
				moving_amount amount;
				events::Callback<void, NodeAnimation&, real> on_elapse;

				inline void Reset() noexcept
				{
					amount.current = 0.0_r;
				}
			};


			using motion_types = std::variant<rotating_motion, scaling_motion, translating_motion, user_motion>;
			using motion_container = std::vector<motion_types>;

			struct motion_types_comparator
			{
				inline auto operator()(const motion_types &x, const motion_types &y) const noexcept
				{
					return std::visit([](auto &&m) noexcept -> const motion& { return m; }, x) <
						   std::visit([](auto &&m) noexcept -> const motion& { return m; }, y);
				}
			};


			/*
				Actions
			*/

			bool execute_action(action &a, duration time, duration current_time, duration start_time) noexcept;

			void elapse_action(NodeAnimation &animation, node_action &a, duration time, duration current_time, duration start_time) noexcept;
			void elapse_action(NodeAnimation &animation, user_action &a, duration time, duration current_time, duration start_time) noexcept;


			/*
				Motions
			*/

			real move_amount(moving_amount &value, real percent) noexcept;

			real elapse_motion(motion &m, duration time, duration current_time, duration start_time) noexcept;
			void elapse_motion(NodeAnimation &animation, rotating_motion &m, duration time, duration current_time, duration start_time) noexcept;
			void elapse_motion(NodeAnimation &animation, scaling_motion &m, duration time, duration current_time, duration start_time) noexcept;
			void elapse_motion(NodeAnimation &animation, translating_motion &m, duration time, duration current_time, duration start_time) noexcept;
			void elapse_motion(NodeAnimation &animation, user_motion &m, duration time, duration current_time, duration start_time) noexcept;
		} //detail
	} //node_animation


	class NodeAnimation final : public managed::ManagedObject<NodeAnimationManager>
	{
		private:

			duration total_duration_ = 0.0_sec;
			node_animation::detail::action_container actions_;
			node_animation::detail::motion_container motions_;

			std::optional<events::Callback<void, NodeAnimation&>> on_start_;
			std::optional<events::Callback<void, NodeAnimation&>> on_finish_;
			std::optional<events::Callback<void, NodeAnimation&>> on_finish_revert_;


			duration RetrieveTotalDuration() const noexcept;

		public:

			using managed::ManagedObject<NodeAnimationManager>::ManagedObject;

			//Construct a new node animation with the given name
			NodeAnimation(std::string name) noexcept;


			/*
				Modifiers
			*/

			//Sets the on start callback
			inline void OnStart(events::Callback<void, NodeAnimation&> on_start) noexcept
			{
				on_start_ = on_start;
			}

			//Sets the on start callback
			inline void OnStart(std::nullopt_t) noexcept
			{
				on_start_ = {};
			}


			//Sets the on finish callback
			inline void OnFinish(events::Callback<void, NodeAnimation&> on_finish) noexcept
			{
				on_finish_ = on_finish;
			}

			//Sets the on finish callback
			inline void OnFinish(std::nullopt_t) noexcept
			{
				on_finish_ = {};
			}


			//Sets the on finish revert callback
			inline void OnFinishRevert(events::Callback<void, NodeAnimation&> on_finish_revert) noexcept
			{
				on_finish_revert_ = on_finish_revert;
			}

			//Sets the on finish revert callback
			inline void OnFinishRevert(std::nullopt_t) noexcept
			{
				on_finish_revert_ = {};
			}


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
			void Elapse(duration time, duration current_time, duration start_time) noexcept;

			//Elapse the total time for this node animation group by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(NodeAnimation &animation, duration time, duration current_time, duration start_time) noexcept;


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

			//Adds a user defined action to this node animation with the given callback and execution time
			void AddAction(events::Callback<void, NodeAnimation&, duration> on_execute, duration time);


			//Clear all actions from this node animation
			void ClearActions() noexcept;


			/*
				Motions
			*/

			//Adds a user defined motion to this node animation with the given target amount, total duration and callback
			void AddMotion(real target_amount, duration total_duration,
				events::Callback<void, NodeAnimation&, real> on_elapse, duration start_time = 0.0_sec,
				node_animation::MotionTechniqueType technique = node_animation::MotionTechniqueType::Linear);


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