/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/graph/animations
File:	IonNodeAnimation.cpp
-------------------------------------------
*/

#include "IonNodeAnimation.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <tuple>
#include <utility>
#include <variant>

#include "IonNodeAnimationManager.h"
#include "IonNodeAnimationTimeline.h"
#include "adaptors/ranges/IonIterable.h"
#include "graphics/scene/IonCamera.h"
#include "graphics/scene/IonDrawableObject.h"
#include "graphics/scene/IonDrawableParticleSystem.h"
#include "graphics/scene/IonDrawableText.h"
#include "graphics/scene/IonLight.h"
#include "graphics/scene/IonModel.h"
#include "graphics/scene/IonMovableObject.h"
#include "graphics/scene/IonMovableSound.h"
#include "graphics/scene/graph/IonSceneNode.h"
#include "sounds/IonSoundChannel.h"

namespace ion::graphics::scene::graph::animations
{

using namespace node_animation;

namespace node_animation::detail
{

/*
	Actions
*/

bool execute_action(action &a, duration time, duration current_time, duration start_time) noexcept
{
	auto local_time = current_time - (start_time + a.time);

	//Execute action?
	if (auto reverse = time < 0.0_sec; reverse ?
		local_time <= 0.0_sec && local_time - time >= 0.0_sec :
		local_time >= 0.0_sec && local_time - time <= 0.0_sec)
	
		return true;

	else
		return false;
}


void elapse_action(NodeAnimation &animation, node_action &a, duration time, duration current_time, duration start_time) noexcept
{
	if (execute_action(a, time, current_time, start_time))
	{
		auto reverse = time < 0.0_sec;
			//Execute the opposite action if in reverse

		if (auto owner = animation.Owner(); owner)
		{
			auto &node = owner->ParentNode();

			switch (a.type)
			{
				//Visibility

				case NodeActionType::Show:
				node.Visible(reverse ? false : true, false);
				break;

				case NodeActionType::ShowCascading:
				node.Visible(reverse ? false : true);
				break;

				case NodeActionType::Hide:
				node.Visible(reverse ? true : false, false);
				break;

				case NodeActionType::HideCascading:
				node.Visible(reverse ? true : false);
				break;

				case NodeActionType::FlipVisibility:
				node.FlipVisibility(false);
				break;

				case NodeActionType::FlipVisibilityCascading:
				node.FlipVisibility();
				break;


				//Transformation

				case NodeActionType::InheritRotation:
				node.InheritRotation(reverse ? false : true);
				break;

				case NodeActionType::InheritScaling:
				node.InheritScaling(reverse ? false : true);
				break;

				case NodeActionType::DisinheritRotation:
				node.InheritRotation(reverse ? true : false);
				break;

				case NodeActionType::DisinheritScaling:
				node.InheritScaling(reverse ? true : false);
				break;
			}
		}
	}
}

void elapse_action(NodeAnimation &animation, node_timeline_action &a, duration time, duration current_time, duration start_time) noexcept
{
	if (execute_action(a, time, current_time, start_time))
	{
		auto reverse = time < 0.0_sec;
			//Execute the opposite action if in reverse

		if (auto owner = animation.Owner(); owner)
		{
			auto &node = owner->ParentNode();
			auto targets = get_timelines(a.target_name, node);

			for (auto &timeline : targets)
			{
				switch (a.type)
				{
					case NodeTimelineActionType::Start:
					{
						if (reverse)
							timeline->Reset();
						else
							timeline->Start();

						break;
					}

					case NodeTimelineActionType::Stop:
					{
						if (reverse)
							timeline->Start();
						else
							timeline->Reset();

						break;
					}

					case NodeTimelineActionType::Pause:
					{
						if (reverse)
							timeline->Stop();
						else
							timeline->Start();

						break;
					}
				}
			}
		}
	}
}

void elapse_action(NodeAnimation &animation, object_action &a, duration time, duration current_time, duration start_time) noexcept
{
	if (execute_action(a, time, current_time, start_time))
	{
		auto reverse = time < 0.0_sec;
			//Execute the opposite action if in reverse

		if (auto owner = animation.Owner(); owner)
		{
			auto &node = owner->ParentNode();
			auto targets = get_targets<MovableObject>(a.target_name, node);

			for (auto &object : targets)
			{
				switch (a.type)
				{
					case ObjectActionType::Show:
					object->Visible(reverse ? false : true);
					break;

					case ObjectActionType::Hide:
					object->Visible(reverse ? true : false);
					break;

					case ObjectActionType::FlipVisibility:
					object->Visible(!object->Visible());
					break;
				}
			}
		}
	}
}

void elapse_action(NodeAnimation &animation, model_action &a, duration time, duration current_time, duration start_time) noexcept
{
	if (execute_action(a, time, current_time, start_time))
	{
		auto reverse = time < 0.0_sec;
			//Execute the opposite action if in reverse

		if (auto owner = animation.Owner(); owner)
		{
			auto &node = owner->ParentNode();

			auto [outer_target_name, inner_target_name] = split_target_name(a.target_name);
			auto targets = get_targets<Model>(outer_target_name, node);

			for (auto &model : targets)
			{
				for (auto &mesh : model->Meshes())
				{
					if (std::empty(inner_target_name) || //All
						mesh.Name() == inner_target_name) //Specific
					{
						switch (a.type)
						{
							case ModelActionType::Show:
							mesh.Visible(reverse ? false : true);
							break;

							case ModelActionType::Hide:
							mesh.Visible(reverse ? true : false);
							break;

							case ModelActionType::FlipVisibility:
							mesh.Visible(!mesh.Visible());
							break;
						}

						if (!std::empty(inner_target_name))
							break;
					}
				}
			}
		}
	}
}

void elapse_action(NodeAnimation &animation, particle_system_action &a, duration time, duration current_time, duration start_time) noexcept
{
	if (execute_action(a, time, current_time, start_time))
	{
		auto reverse = time < 0.0_sec;
			//Execute the opposite action if in reverse

		if (auto owner = animation.Owner(); owner)
		{
			auto &node = owner->ParentNode();

			auto [outer_target_name, inner_target_name] = split_target_name(a.target_name);
			auto targets = get_targets<DrawableParticleSystem>(outer_target_name, node);

			for (auto &target : targets)
			{
				if (auto &particle_system = target->Get(); particle_system)
				{
					switch (a.type)
					{
						case ParticleSystemActionType::Start:
						{
							if (std::empty(inner_target_name)) //All
							{
								if (reverse)
									particle_system->ResetAll();
								else
									particle_system->StartAll();
							}
							else if (auto emitter = particle_system->GetEmitter(inner_target_name); emitter) //Specific
							{
								if (reverse)
									emitter->Reset();
								else
									emitter->Start();
							}

							break;
						}

						case ParticleSystemActionType::Stop:
						{
							if (std::empty(inner_target_name)) //All
							{
								if (reverse)
									particle_system->StartAll();
								else
									particle_system->ResetAll();
							}
							else if (auto emitter = particle_system->GetEmitter(inner_target_name); emitter) //Specific
							{
								if (reverse)
									emitter->Start();
								else
									emitter->Reset();
							}

							break;
						}

						case ParticleSystemActionType::Pause:
						{
							if (std::empty(inner_target_name)) //All
							{
								if (reverse)
									particle_system->StartAll();
								else
									particle_system->StopAll();
							}
							else if (auto emitter = particle_system->GetEmitter(inner_target_name); emitter) //Specific
							{
								if (reverse)
									emitter->Start();
								else
									emitter->Stop();
							}

							break;
						}
					}
				}
			}
		}
	}
}

void elapse_action(NodeAnimation &animation, sound_action &a, duration time, duration current_time, duration start_time) noexcept
{
	if (execute_action(a, time, current_time, start_time))
	{
		auto reverse = time < 0.0_sec;
			//Execute the opposite action if in reverse

		if (auto owner = animation.Owner(); owner)
		{
			auto &node = owner->ParentNode();
			auto targets = get_targets<MovableSound>(a.target_name, node);

			for (auto &target : targets)
			{
				if (auto &sound = target->Get(); sound)
				{
					switch (a.type)
					{
						case SoundActionType::Start:
						{
							if (reverse)
								sound->Reset();
							else
								sound->Resume();

							break;
						}

						case SoundActionType::Stop:
						{
							if (reverse)
								sound->Resume();
							else
								sound->Reset();

							break;
						}

						case SoundActionType::Pause:
						{
							if (reverse)
								sound->Resume();
							else
								sound->Pause();

							break;
						}

						case SoundActionType::Mute:
						sound->Mute(reverse ? false : true);
						break;

						case SoundActionType::Unmute:
						sound->Mute(reverse ? true : false);
						break;
					}
				}
			}
		}
	}
}

void elapse_action(NodeAnimation &animation, user_action &a, duration time, duration current_time, duration start_time) noexcept
{
	if (execute_action(a, time, current_time, start_time))
	{
		if (auto reverse = time < 0.0_sec; reverse)
			//Execute the opposite action if in reverse
		{
			if (a.on_execute_opposite)
				(*a.on_execute_opposite)(std::ref(animation), std::ref(a.user_data));
		}
		else
			a.on_execute(std::ref(animation), std::ref(a.user_data));
	}
}


/*
	Motions
*/

real move_amount(moving_amount &amount, real percent) noexcept
{
	using namespace ion::utilities;

	auto current =
		[&]() noexcept
		{
			if (amount.user_technique)
				return (*amount.user_technique)(amount.target, percent);

			else
			{
				switch (amount.technique)
				{
					case MotionTechniqueType::Cubic:
					return amount.target * cubic(percent, 0.0_r, 10.0_r);

					case MotionTechniqueType::Exponential:
					return amount.target * exp(percent, 0.0_r, 10.0_r);

					case MotionTechniqueType::Logarithmic:
					return amount.target * log(percent, 1.0_r, 10.0_r);

					case MotionTechniqueType::Sigmoid:
					return amount.target * sigmoid(percent, -5.0_r, 5.0_r);

					case MotionTechniqueType::Sinh:
					return amount.target * sinh(percent, -2.5_r, 2.5_r);

					case MotionTechniqueType::Tanh:
					return amount.target * tanh(percent, -2.5_r, 2.5_r);

					case MotionTechniqueType::Linear:
					default:
					return amount.target * percent;
				}
			}
		}();

	auto delta = current - amount.current;
	amount.current = current;
	return delta;
}


real elapse_motion(motion &m, duration time, duration current_time, duration start_time) noexcept
{
	auto local_time = current_time - (start_time + m.start_time);

	if (auto reverse = time < 0.0_sec; reverse ?
		local_time <= m.total_duration && local_time - time > 0.0_sec :
		local_time >= 0.0_sec && local_time - time < m.total_duration)
	{
		auto percent = local_time / m.total_duration;
		return std::clamp(percent, 0.0_r, 1.0_r);
	}
	else
		return local_time < 0.0_sec ? 0.0_r : 1.0_r;
}

void elapse_motion(NodeAnimation &animation, rotating_motion &m, duration time, duration current_time, duration start_time) noexcept
{
	auto percent = elapse_motion(m, time, current_time, start_time);
		
	if (auto angle = move_amount(m.angle, percent);
		angle != 0.0_r)
	{
		if (auto owner = animation.Owner(); owner)
			owner->ParentNode().Rotate(angle);
	}
}

void elapse_motion(NodeAnimation &animation, scaling_motion &m, duration time, duration current_time, duration start_time) noexcept
{
	auto percent = elapse_motion(m, time, current_time, start_time);

	if (auto unit = Vector2{move_amount(m.x, percent), move_amount(m.y, percent)};
		unit != vector2::Zero)
	{
		if (auto owner = animation.Owner(); owner)
			owner->ParentNode().Scale(unit);
	}
}

void elapse_motion(NodeAnimation &animation, translating_motion &m, duration time, duration current_time, duration start_time) noexcept
{
	auto percent = elapse_motion(m, time, current_time, start_time);

	if (auto unit = Vector3{move_amount(m.x, percent), move_amount(m.y, percent), move_amount(m.z, percent)};
		unit != vector3::Zero)
	{
		if (auto owner = animation.Owner(); owner)
			owner->ParentNode().Translate(unit);
	}
}

void elapse_motion(NodeAnimation &animation, fading_motion &m, duration time, duration current_time, duration start_time) noexcept
{
	auto percent = elapse_motion(m, time, current_time, start_time);
		
	if (auto delta = move_amount(m.amount, percent);
		delta != 0.0_r)
	{
		if (auto owner = animation.Owner(); owner)
		{
			auto &node = owner->ParentNode();

			switch (m.type)
			{
				case FadingMotionType::Opacity:
				{
					auto targets = get_targets<DrawableObject>(m.target_name, node);

					for (auto &object : targets)
						object->Opacity(object->Opacity() + delta);

					break;
				}

				//Light
				case FadingMotionType::LightIntensity:
				case FadingMotionType::LightRadius:
				case FadingMotionType::LightConstantAttenuation:
				case FadingMotionType::LightLinearAttenuation:
				case FadingMotionType::LightQuadraticAttenuation:
				case FadingMotionType::LightCutoffInnerAngle:
				case FadingMotionType::LightCutoffOuterAngle:
				{
					auto targets = get_targets<Light>(m.target_name, node);

					switch (m.type)
					{
						case FadingMotionType::LightIntensity:
						{
							for (auto &light : targets)
								light->Intensity(light->Intensity() + delta);

							break;
						}

						case FadingMotionType::LightRadius:
						{
							for (auto &light : targets)
								light->Radius(light->Radius() + delta);

							break;
						}

						case FadingMotionType::LightConstantAttenuation:
						{
							for (auto &light : targets)
							{
								auto [constant, linear, quadratic] = light->Attenuation();
								light->Attenuation(constant + delta, linear, quadratic);
							}

							break;
						}

						case FadingMotionType::LightLinearAttenuation:
						{
							for (auto &light : targets)
							{
								auto [constant, linear, quadratic] = light->Attenuation();
								light->Attenuation(constant, linear + delta, quadratic);
							}

							break;
						}

						case FadingMotionType::LightQuadraticAttenuation:
						{
							for (auto &light : targets)
							{
								auto [constant, linear, quadratic] = light->Attenuation();
								light->Attenuation(constant, linear, quadratic + delta);
							}

							break;
						}

						case FadingMotionType::LightCutoffInnerAngle:
						{
							for (auto &light : targets)
							{
								auto [inner_angle, outer_angle] = light->Cutoff();
								light->Cutoff(inner_angle + math::ToRadians(delta), outer_angle);
							}

							break;
						}

						case FadingMotionType::LightCutoffOuterAngle:
						{
							for (auto &light : targets)
							{
								auto [inner_angle, outer_angle] = light->Cutoff();
								light->Cutoff(inner_angle, outer_angle + math::ToRadians(delta));
							}

							break;
						}
					}

					break;
				}

				//Model
				case FadingMotionType::ModelBaseOpacity:
				{
					auto [outer_target_name, inner_target_name] = split_target_name(m.target_name);
					auto targets = get_targets<Model>(outer_target_name, node);

					for (auto &model : targets)
					{
						for (auto &mesh : model->Meshes())
						{
							if (std::empty(inner_target_name) || //All
								mesh.Name() == inner_target_name) //Specific
							{
								if (auto shape = dynamic_cast<shapes::Shape*>(&mesh); shape)
									shape->FillOpacity(shape->FillOpacity() + delta);
								else
									mesh.BaseOpacity(mesh.BaseOpacity() + delta);

								if (!std::empty(inner_target_name))
									break;
							}
						}
					}

					break;
				}

				//Sound
				case FadingMotionType::SoundPitch:
				case FadingMotionType::SoundVolume:
				{
					auto targets = get_targets<MovableSound>(m.target_name, node);

					for (auto &target : targets)
					{
						if (auto &sound = target->Get(); sound)
						{
							switch (m.type)
							{
								case FadingMotionType::SoundPitch:
								sound->Pitch(sound->Pitch() + delta);
								break;

								case FadingMotionType::SoundVolume:
								sound->Volume(sound->Volume() + delta);
								break;
							}
						}
					}

					break;
				}

				//Text
				case FadingMotionType::TextBaseOpacity:
				{
					auto targets = get_targets<DrawableText>(m.target_name, node);

					for (auto &target : targets)
					{
						if (auto &text = target->Get(); text)
							text->DefaultBaseOpacity(text->DefaultBaseOpacity() + delta);
					}

					break;
				}
			}
		}
	}
}

void elapse_motion(NodeAnimation &animation, color_fading_motion &m, duration time, duration current_time, duration start_time) noexcept
{
	auto percent = elapse_motion(m, time, current_time, start_time);

	if (auto delta = std::tuple{move_amount(m.r, percent), move_amount(m.g, percent), move_amount(m.b, percent), move_amount(m.a, percent)};
		delta != std::tuple{0.0_r, 0.0_r, 0.0_r, 0.0_r})
	{
		auto [delta_r, delta_g, delta_b, delta_a] = delta;

		if (auto owner = animation.Owner(); owner)
		{
			auto &node = owner->ParentNode();

			switch (m.type)
			{
				//Light
				case ColorFadingMotionType::LightAmbient:
				case ColorFadingMotionType::LightDiffuse:
				case ColorFadingMotionType::LightSpecular:
				{
					auto targets = get_targets<Light>(m.target_name, node);

					for (auto &light : targets)
					{
						switch (m.type)
						{
							case ColorFadingMotionType::LightAmbient:
							{
								auto [r, g, b, a] = light->AmbientColor().RGBA();
								light->AmbientColor(Color{r + delta_r, g + delta_g, b + delta_b, a + delta_a});
								break;
							}

							case ColorFadingMotionType::LightDiffuse:
							{
								auto [r, g, b, a] = light->DiffuseColor().RGBA();
								light->DiffuseColor(Color{r + delta_r, g + delta_g, b + delta_b, a + delta_a});
								break;
							}

							case ColorFadingMotionType::LightSpecular:
							{
								auto [r, g, b, a] = light->SpecularColor().RGBA();
								light->SpecularColor(Color{r + delta_r, g + delta_g, b + delta_b, a + delta_a});
								break;
							}
						}
					}

					break;
				}

				//Model
				case ColorFadingMotionType::ModelBaseColor:
				{
					auto [outer_target_name, inner_target_name] = split_target_name(m.target_name);
					auto targets = get_targets<Model>(outer_target_name, node);

					for (auto &model : targets)
					{
						for (auto &mesh : model->Meshes())
						{
							if (std::empty(inner_target_name) || //All
								mesh.Name() == inner_target_name) //Specific
							{
								if (auto shape = dynamic_cast<shapes::Shape*>(&mesh); shape)
								{
									auto [r, g, b, a] = shape->FillColor().RGBA();
									shape->FillColor(Color{r + delta_r, g + delta_g, b + delta_b, a + delta_a});
								}
								else
								{
									auto [r, g, b, a] = mesh.BaseColor().RGBA();
									mesh.BaseColor(Color{r + delta_r, g + delta_g, b + delta_b, a + delta_a});
								}

								if (!std::empty(inner_target_name))
									break;
							}
						}
					}

					break;
				}

				//Text
				case ColorFadingMotionType::TextForegroundColor:
				case ColorFadingMotionType::TextBackgroundColor:
				case ColorFadingMotionType::TextDecorationColor:
				{
					auto targets = get_targets<DrawableText>(m.target_name, node);

					for (auto &target : targets)
					{
						if (auto &text = target->Get(); text)
						{
							switch (m.type)
							{
								case ColorFadingMotionType::TextForegroundColor:
								{
									auto [r, g, b, a] = text->DefaultForegroundColor().RGBA();
									text->DefaultForegroundColor(Color{r + delta_r, g + delta_g, b + delta_b, a + delta_a});
									break;
								}

								case ColorFadingMotionType::TextBackgroundColor:
								{
									if (text->DefaultBackgroundColor())
									{
										auto [r, g, b, a] = text->DefaultBackgroundColor()->RGBA();
										text->DefaultBackgroundColor(Color{r + delta_r, g + delta_g, b + delta_b, a + delta_a});
									}

									break;
								}

								case ColorFadingMotionType::TextDecorationColor:
								{
									if (text->DefaultDecorationColor())
									{
										auto [r, g, b, a] = text->DefaultDecorationColor()->RGBA();
										text->DefaultDecorationColor(Color{r + delta_r, g + delta_g, b + delta_b, a + delta_a});
									}

									break;
								}
							}
						}
					}

					break;
				}
			}
		}
	}
}

void elapse_motion(NodeAnimation &animation, user_motion &m, duration time, duration current_time, duration start_time) noexcept
{
	auto percent = elapse_motion(m, time, current_time, start_time);
	
	if (auto amount = move_amount(m.amount, percent);
		amount != 0.0_r)

		m.on_elapse(std::ref(animation), amount, std::ref(m.user_data));
}

void elapse_motion(NodeAnimation &animation, user_multi_motion &m, duration time, duration current_time, duration start_time) noexcept
{
	auto percent = elapse_motion(m, time, current_time, start_time);

	std::vector<real> amounts;
	amounts.reserve(std::size(m.amounts));

	auto non_zero = false;
	for (auto &amount : m.amounts)
	{
		amounts.push_back(move_amount(amount, percent));
		non_zero |= amounts.back() != 0.0_r;
	}

	if (non_zero)
		m.on_elapse(std::ref(animation), std::move(amounts), std::ref(m.user_data));
}


/*
	Targets
*/

std::pair<std::string_view, std::string_view> split_target_name(std::string_view name) noexcept
{
	if (auto off = name.find('.'); off != std::string_view::npos)
		return {name.substr(0, off), name.substr(off + 1)};
	else
		return {name, std::string_view{}};
}


std::vector<NodeAnimationTimeline*> get_timelines(std::string_view name, SceneNode &node)
{
	std::vector<NodeAnimationTimeline*> targets;

	//Self
	for (auto &timeline : node.Timelines())
	{
		if (timeline.Name() == name)
			targets.push_back(&timeline);
	}

	//Descendants
	for (auto &desc_node : node.DepthFirstSearch())
	{
		for (auto &timeline : desc_node.Timelines())
		{
			if (timeline.Name() == name)
				targets.push_back(&timeline);
		}
	}

	return targets;
}

std::vector<MovableObject*> get_movable_objects(std::string_view name, SceneNode &node)
{
	std::vector<MovableObject*> targets;

	//Self
	for (auto &object : node.AttachedObjects())
	{
		if (auto target =
			std::visit([](auto &&object) noexcept -> MovableObject* { return object; }, object);
			target && (target->Name() == name || target->Alias() == name))
				
			targets.push_back(target);
	}

	//Descendants
	for (auto &desc_node : node.DepthFirstSearch())
	{
		for (auto &object : desc_node.AttachedObjects())
		{
			if (auto target =
				std::visit([](auto &&object) noexcept -> MovableObject* { return object; }, object);
				target && (target->Name() == name || target->Alias() == name))
				
				targets.push_back(target);
		}
	}

	return targets;
}

} //node_animation::detail


//Private

duration NodeAnimation::RetrieveTotalDuration() const noexcept
{
	auto total_duration = 0.0_sec;

	if (!std::empty(actions_))
		total_duration = std::visit(
			[&](auto &&a) noexcept
			{
				return std::max(total_duration_, a.time);
			}, actions_.back());

	if (!std::empty(motions_))
		total_duration = std::visit(
			[&](auto &&m) noexcept
			{
				return std::max(total_duration_, m.start_time + m.total_duration);
			}, motions_.back());

	return total_duration;
}


//Public

/*
	Modifiers
*/

void NodeAnimation::Reset() noexcept
{
	for (auto &m : motions_)
		std::visit([&](auto &&m) noexcept { m.Reset(); }, m);
}


/*
	Elapse time
*/

void NodeAnimation::Elapse(duration time, duration current_time, duration start_time) noexcept
{
	Elapse(*this, time, current_time, start_time);
}

void NodeAnimation::Elapse(NodeAnimation &animation, duration time, duration current_time, duration start_time) noexcept
{
	auto local_time = current_time - start_time;

	if (auto reverse = time < 0.0_sec; reverse ?
		local_time <= total_duration_ && local_time - time > 0.0_sec :
		local_time >= 0.0_sec && local_time - time < total_duration_)
	{
		//Start
		if (on_start_ &&
			local_time >= 0.0_sec && local_time - time < 0.0_sec)
			(*on_start_)(animation);


		//Reverse
		if (reverse)
		{
			for (auto &a : adaptors::ranges::ReverseIterable<decltype(actions_)&>(actions_))
				std::visit([&](auto &&a) noexcept { elapse_action(animation, a, time, current_time, start_time); }, a);

			for (auto &m : adaptors::ranges::ReverseIterable<decltype(motions_)&>(motions_))
				std::visit([&](auto &&m) noexcept { elapse_motion(animation, m, time, current_time, start_time); }, m);
		}
		else //Forward
		{
			for (auto &a : actions_)
				std::visit([&](auto &&a) noexcept { elapse_action(animation, a, time, current_time, start_time); }, a);

			for (auto &m : motions_)
				std::visit([&](auto &&m) noexcept { elapse_motion(animation, m, time, current_time, start_time); }, m);
		}


		//Finish
		if (on_finish_ &&
			local_time >= total_duration_ && local_time - time < total_duration_)
			(*on_finish_)(animation);

		//Finish revert
		if (on_finish_revert_ &&
			local_time <= 0.0_sec && local_time - time > 0.0_sec)
			(*on_finish_revert_)(animation);
	}
}


/*
	Playback
*/

NonOwningPtr<NodeAnimationTimeline> NodeAnimation::Start(real playback_rate, bool running)
{
	if (auto owner = Owner(); owner)
	{
		auto timeline = owner->CreateTimeline({}, playback_rate, running);
		timeline->Attach(owner->GetAnimation(*Name()));
		return timeline;
	}
	else
		return nullptr;
}


/*
	Actions
*/

void NodeAnimation::AddAction(NodeActionType type, duration time)
{
	assert(time >= 0.0_sec);

	auto a = detail::node_action{
		{time}, type};

	//Insert sorted
	actions_.insert(
		std::upper_bound(std::begin(actions_), std::end(actions_), a,
			detail::action_types_comparator{}),
		a);

	total_duration_ = std::max(total_duration_, time);
}

void NodeAnimation::AddAction(NodeTimelineActionType type, std::string target_name, duration time)
{
	assert(time >= 0.0_sec);

	auto a = detail::node_timeline_action{
		{time}, type, std::move(target_name)};

	//Insert sorted
	actions_.insert(
		std::upper_bound(std::begin(actions_), std::end(actions_), a,
			detail::action_types_comparator{}),
		a);

	total_duration_ = std::max(total_duration_, time);
}

void NodeAnimation::AddAction(ObjectActionType type, std::string target_name, duration time)
{
	assert(time >= 0.0_sec);

	auto a = detail::object_action{
		{time}, type, std::move(target_name)};

	//Insert sorted
	actions_.insert(
		std::upper_bound(std::begin(actions_), std::end(actions_), a,
			detail::action_types_comparator{}),
		a);

	total_duration_ = std::max(total_duration_, time);
}

void NodeAnimation::AddAction(ModelActionType type, std::string target_name, duration time)
{
	assert(time >= 0.0_sec);

	auto a = detail::model_action{
		{time}, type, std::move(target_name)};

	//Insert sorted
	actions_.insert(
		std::upper_bound(std::begin(actions_), std::end(actions_), a,
			detail::action_types_comparator{}),
		a);

	total_duration_ = std::max(total_duration_, time);
}

void NodeAnimation::AddAction(ParticleSystemActionType type, std::string target_name, duration time)
{
	assert(time >= 0.0_sec);

	auto a = detail::particle_system_action{
		{time}, type, std::move(target_name)};

	//Insert sorted
	actions_.insert(
		std::upper_bound(std::begin(actions_), std::end(actions_), a,
			detail::action_types_comparator{}),
		a);

	total_duration_ = std::max(total_duration_, time);
}

void NodeAnimation::AddAction(SoundActionType type, std::string target_name, duration time)
{
	assert(time >= 0.0_sec);

	auto a = detail::sound_action{
		{time}, type, std::move(target_name)};

	//Insert sorted
	actions_.insert(
		std::upper_bound(std::begin(actions_), std::end(actions_), a,
			detail::action_types_comparator{}),
		a);

	total_duration_ = std::max(total_duration_, time);
}


void NodeAnimation::AddAction(events::Callback<void, NodeAnimation&, std::any&> on_execute,
	duration time, std::any user_data)
{
	assert(time >= 0.0_sec);

	auto a = detail::user_action{
		{time},
		std::move(user_data),
		on_execute};

	//Insert sorted
	actions_.insert(
		std::upper_bound(std::begin(actions_), std::end(actions_), a,
			detail::action_types_comparator{}),
		a);

	total_duration_ = std::max(total_duration_, time);
}

void NodeAnimation::AddAction(events::Callback<void, NodeAnimation&, std::any&> on_execute,
	events::Callback<void, NodeAnimation&, std::any&> on_execute_opposite,
	duration time, std::any user_data)
{
	assert(time >= 0.0_sec);

	auto a = detail::user_action{
		{time},
		std::move(user_data),
		on_execute, on_execute_opposite};

	//Insert sorted
	actions_.insert(
		std::upper_bound(std::begin(actions_), std::end(actions_), a,
			detail::action_types_comparator{}),
		a);

	total_duration_ = std::max(total_duration_, time);
}


void NodeAnimation::ClearActions() noexcept
{
	actions_.clear();
	actions_.shrink_to_fit();
	total_duration_ = RetrieveTotalDuration();
}


/*
	Motions
*/

void NodeAnimation::AddMotion(real target_amount, duration total_duration,
	events::Callback<void, NodeAnimation&, real, std::any&> on_elapse, duration start_time,
	MotionTechnique technique, std::any user_data)
{
	assert(total_duration > 0.0_sec);
	assert(start_time >= 0.0_sec);

	auto m = detail::user_motion{
		{start_time, total_duration},
		{0.0_r, target_amount, technique.type, technique.method},
		std::move(user_data),
		on_elapse};

	//Insert sorted
	motions_.insert(
		std::upper_bound(std::begin(motions_), std::end(motions_), m,
			detail::motion_types_comparator{}),
		m);

	total_duration_ = std::max(total_duration_, start_time + total_duration);
}

void NodeAnimation::AddMotion(std::vector<real> target_amounts, duration total_duration,
	events::Callback<void, NodeAnimation&, std::vector<real>, std::any&> on_elapse, duration start_time,
	MotionTechniques techniques, std::any user_data)
{
	assert(!std::empty(target_amounts));
	assert(total_duration > 0.0_sec);
	assert(start_time >= 0.0_sec);

	if (auto count = std::ssize(target_amounts) - std::ssize(techniques); count > 0)
		techniques.insert(std::end(techniques), count, MotionTechniqueType::Linear);

	detail::moving_amounts amounts;
	for (auto i = 0; auto &target_amount : target_amounts)
	{
		auto &technique = techniques[i++];
		amounts.emplace_back(0.0_r, target_amount, technique.type, technique.method);
	}

	auto m = detail::user_multi_motion{
		{start_time, total_duration},	
		std::move(amounts),
		std::move(user_data),
		on_elapse};

	//Insert sorted
	motions_.insert(
		std::upper_bound(std::begin(motions_), std::end(motions_), m,
			detail::motion_types_comparator{}),
		m);

	total_duration_ = std::max(total_duration_, start_time + total_duration);
}


void NodeAnimation::AddRotation(real angle, duration total_duration, duration start_time,
	MotionTechnique technique)
{
	assert(total_duration > 0.0_sec);
	assert(start_time >= 0.0_sec);

	auto m = detail::rotating_motion{
		{start_time, total_duration},
		{0.0_r, angle, technique.type, technique.method}};

	//Insert sorted
	motions_.insert(
		std::upper_bound(std::begin(motions_), std::end(motions_), m,
			detail::motion_types_comparator{}),
		m);

	total_duration_ = std::max(total_duration_, start_time + total_duration);
}


void NodeAnimation::AddScaling(const Vector2 &unit, duration total_duration, duration start_time,
	MotionTechnique technique)
{
	assert(total_duration > 0.0_sec);
	assert(start_time >= 0.0_sec);

	auto m = detail::scaling_motion{
		{start_time, total_duration},
		{0.0_r, unit.X(), technique.type, technique.method},
		{0.0_r, unit.Y(), technique.type, technique.method}};

	//Insert sorted
	motions_.insert(
		std::upper_bound(std::begin(motions_), std::end(motions_), m,
			detail::motion_types_comparator{}),
		m);

	total_duration_ = std::max(total_duration_, start_time + total_duration);
}

void NodeAnimation::AddScaling(const Vector2 &unit, duration total_duration, duration start_time,
	MotionTechnique technique_x, MotionTechnique technique_y)
{
	assert(total_duration > 0.0_sec);
	assert(start_time >= 0.0_sec);

	auto m = detail::scaling_motion{
		{start_time, total_duration},
		{0.0_r, unit.X(), technique_x.type, technique_x.method},
		{0.0_r, unit.Y(), technique_y.type, technique_y.method}};

	//Insert sorted
	motions_.insert(
		std::upper_bound(std::begin(motions_), std::end(motions_), m,
			detail::motion_types_comparator{}),
		m);

	total_duration_ = std::max(total_duration_, start_time + total_duration);
}


void NodeAnimation::AddTranslation(const Vector3 &unit, duration total_duration, duration start_time,
	MotionTechnique technique)
{
	assert(total_duration > 0.0_sec);
	assert(start_time >= 0.0_sec);

	auto m = detail::translating_motion{
		{start_time, total_duration},
		{0.0_r, unit.X(), technique.type, technique.method},
		{0.0_r, unit.Y(), technique.type, technique.method},
		{0.0_r, unit.Z(), technique.type, technique.method}};

	//Insert sorted
	motions_.insert(
		std::upper_bound(std::begin(motions_), std::end(motions_), m,
			detail::motion_types_comparator{}),
		m);

	total_duration_ = std::max(total_duration_, start_time + total_duration);
}

void NodeAnimation::AddTranslation(const Vector3 &unit, duration total_duration, duration start_time,
	MotionTechnique technique_x, MotionTechnique technique_y, MotionTechnique technique_z)
{
	assert(total_duration > 0.0_sec);
	assert(start_time >= 0.0_sec);

	auto m = detail::translating_motion{
		{start_time, total_duration},
		{0.0_r, unit.X(), technique_x.type, technique_x.method},
		{0.0_r, unit.Y(), technique_y.type, technique_y.method},
		{0.0_r, unit.Z(), technique_z.type, technique_z.method}};

	//Insert sorted
	motions_.insert(
		std::upper_bound(std::begin(motions_), std::end(motions_), m,
			detail::motion_types_comparator{}),
		m);

	total_duration_ = std::max(total_duration_, start_time + total_duration);
}


void NodeAnimation::AddFading(FadingMotionType type, std::string target_name,
	real amount, duration total_duration, duration start_time,
	MotionTechnique technique)
{
	assert(total_duration > 0.0_sec);
	assert(start_time >= 0.0_sec);

	auto m = detail::fading_motion{
		{start_time, total_duration},
		type, std::move(target_name),
		{0.0_r, amount, technique.type, technique.method}};

	//Insert sorted
	motions_.insert(
		std::upper_bound(std::begin(motions_), std::end(motions_), m,
			detail::motion_types_comparator{}),
		m);

	total_duration_ = std::max(total_duration_, start_time + total_duration);
}


void NodeAnimation::AddColorFading(ColorFadingMotionType type, std::string target_name,
	const Color &from_color, const Color &to_color, duration total_duration, duration start_time,
	MotionTechnique technique)
{
	assert(total_duration > 0.0_sec);
	assert(start_time >= 0.0_sec);

	auto m = detail::color_fading_motion{
		{start_time, total_duration},
		type, std::move(target_name),
		{0.0_r, to_color.R() - from_color.R(), technique.type, technique.method},
		{0.0_r, to_color.G() - from_color.G(), technique.type, technique.method},
		{0.0_r, to_color.B() - from_color.B(), technique.type, technique.method},
		{0.0_r, to_color.A() - from_color.A(), technique.type, technique.method}};

	//Insert sorted
	motions_.insert(
		std::upper_bound(std::begin(motions_), std::end(motions_), m,
			detail::motion_types_comparator{}),
		m);

	total_duration_ = std::max(total_duration_, start_time + total_duration);
}

void NodeAnimation::AddColorFading(ColorFadingMotionType type, std::string target_name,
	const Color &from_color, const Color &to_color, duration total_duration, duration start_time,
	MotionTechnique technique_r,
	MotionTechnique technique_g,
	MotionTechnique technique_b,
	MotionTechnique technique_a)
{
	assert(total_duration > 0.0_sec);
	assert(start_time >= 0.0_sec);

	auto m = detail::color_fading_motion{
		{start_time, total_duration},
		type, std::move(target_name),
		{0.0_r, to_color.R() - from_color.R(), technique_r.type, technique_r.method},
		{0.0_r, to_color.G() - from_color.G(), technique_g.type, technique_g.method},
		{0.0_r, to_color.B() - from_color.B(), technique_b.type, technique_b.method},
		{0.0_r, to_color.A() - from_color.A(), technique_a.type, technique_a.method}};

	//Insert sorted
	motions_.insert(
		std::upper_bound(std::begin(motions_), std::end(motions_), m,
			detail::motion_types_comparator{}),
		m);

	total_duration_ = std::max(total_duration_, start_time + total_duration);
}


void NodeAnimation::ClearMotions() noexcept
{
	motions_.clear();
	motions_.shrink_to_fit();
	total_duration_ = RetrieveTotalDuration();
}


/*
	Actions/motions
*/

void NodeAnimation::Clear() noexcept
{
	actions_.clear();
	actions_.shrink_to_fit();

	motions_.clear();
	motions_.shrink_to_fit();

	total_duration_ = 0.0_sec;
}

} //ion::graphics::scene::graph::animations