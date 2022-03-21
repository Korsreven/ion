/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonParticleSystemScriptInterface.cpp
-------------------------------------------
*/

#include "IonParticleSystemScriptInterface.h"

#include <string>
#include "graphics/materials/IonMaterialManager.h"

namespace ion::script::interfaces
{

using namespace std::string_literals;
using namespace script_validator;
using namespace particle_system_script_interface;

namespace particle_system_script_interface::detail
{

/*
	Validator classes
*/

ClassDefinition get_affector_class()
{
	return ClassDefinition::Create("affector")
		.AddRequiredProperty("name", ParameterType::String)
		.AddProperty("enabled", ParameterType::Boolean);
}

ClassDefinition get_emitter_class()
{
	return ClassDefinition::Create("emitter")
		.AddClass("color-fader")
		.AddClass("direction-randomizer")
		.AddClass("gravitation")
		.AddClass("linear-force")
		.AddClass("scaler")
		.AddClass("sine-force")
		.AddClass("velocity-randomizer")

		.AddRequiredProperty("name", ParameterType::String)
		.AddProperty("direction", ParameterType::Vector2)
		.AddProperty("emission-angle", ParameterType::FloatingPoint)
		.AddProperty("emission-duration", ParameterType::FloatingPoint)
		.AddProperty("emission-rate", ParameterType::FloatingPoint)	
		.AddProperty("inner-size", ParameterType::Vector2)
		.AddProperty("particle-color", {ParameterType::Color, ParameterType::Color}, 1)
		.AddProperty("particle-lifetime", {ParameterType::FloatingPoint, ParameterType::FloatingPoint}, 1)
		.AddProperty("particle-mass", {ParameterType::FloatingPoint, ParameterType::FloatingPoint}, 1)
		.AddProperty("particle-material", ParameterType::String)
		.AddProperty("particle-size", {ParameterType::Vector2, ParameterType::Vector2}, 1)
		.AddProperty("particle-velocity", {ParameterType::FloatingPoint, ParameterType::FloatingPoint}, 1)
		.AddProperty("particle-quota", ParameterType::Integer)
		.AddProperty("position", ParameterType::Vector2)
		.AddProperty("size", ParameterType::Vector2)
		.AddProperty("type", {"point"s, "box"s, "ring"s});
}

ClassDefinition get_particle_system_class()
{
	return ClassDefinition::Create("particle-system")
		.AddClass(get_emitter_class())
		.AddClass(get_color_fader_class())
		.AddClass(get_direction_randomizer_class())
		.AddClass(get_graviation_class())
		.AddClass(get_linear_force_class())
		.AddClass(get_scaler_class())
		.AddClass(get_sine_force_class())
		.AddClass(get_velocity_randomizer_class())

		.AddRequiredProperty("name", ParameterType::String)
		.AddProperty("render-primitive", {"point"s, "rectangle"s});
}


ClassDefinition get_color_fader_class()
{
	auto step = ClassDefinition::Create("step")
		.AddRequiredProperty("percent", ParameterType::FloatingPoint)
		.AddProperty("color", ParameterType::Color);

	return ClassDefinition::Create("color-fader", "affector")
		.AddClass(std::move(step));
}

ClassDefinition get_direction_randomizer_class()
{
	return ClassDefinition::Create("direction-randomizer", "affector")
		.AddProperty("angle", ParameterType::FloatingPoint)
		.AddProperty("scope", ParameterType::FloatingPoint);
}

ClassDefinition get_graviation_class()
{
	return ClassDefinition::Create("gravitation", "affector")
		.AddProperty("gravity", ParameterType::FloatingPoint)
		.AddProperty("mass", ParameterType::FloatingPoint)
		.AddProperty("position", ParameterType::Vector2);
}

ClassDefinition get_linear_force_class()
{
	return ClassDefinition::Create("linear-force", "affector")
		.AddProperty("force", ParameterType::Vector2)
		.AddProperty("type", {"add"s, "average"s});
}

ClassDefinition get_scaler_class()
{
	auto step = ClassDefinition::Create("step")
		.AddRequiredProperty("percent", ParameterType::FloatingPoint)
		.AddProperty("size", ParameterType::Vector2);

	return ClassDefinition::Create("scaler", "affector")
		.AddClass(std::move(step));
}

ClassDefinition get_sine_force_class()
{
	return ClassDefinition::Create("sine-force", "affector")
		.AddProperty("force", ParameterType::Vector2)
		.AddProperty("type", {"add"s, "average"s});
}

ClassDefinition get_velocity_randomizer_class()
{
	return ClassDefinition::Create("velocity-randomizer", "affector")
		.AddProperty("scope", ParameterType::FloatingPoint)
		.AddProperty("velocity", {ParameterType::FloatingPoint, ParameterType::FloatingPoint}, 1);
}


ScriptValidator get_particle_system_validator()
{
	return ScriptValidator::Create()
		.AddAbstractClass(get_affector_class())
		.AddRequiredClass(get_particle_system_class());
}


/*
	Tree parsing
*/

NonOwningPtr<graphics::particles::Emitter> create_emitter(const script_tree::ObjectNode &object,
	graphics::particles::ParticleSystem &particle_system)
{
	auto &name =
		object
			.Property("name")[0]
			.Get<ScriptType::String>()
			.value_or(""s)
			.Get();

	auto emitter = particle_system.CreateEmitter(name);

	if (emitter)
	{
		for (auto &property : object.Properties())
		{
			if (property.Name() == "");
		}

		for (auto &obj : object.Objects())
		{
			if (obj.Name() == "color-fader")
				detail::create_color_fader(obj, *emitter);
			else if (obj.Name() == "direction-randomizer")
				detail::create_direction_randomizer(obj, *emitter);
			else if (obj.Name() == "graviation")
				detail::create_graviation(obj, *emitter);
			else if (obj.Name() == "linear-force")
				detail::create_linear_force(obj, *emitter);
			else if (obj.Name() == "scaler")
				detail::create_scaler(obj, *emitter);
			else if (obj.Name() == "sine-force")
				detail::create_sine_force(obj, *emitter);
			else if (obj.Name() == "velocity-randomizer")
				detail::create_velocity_randomizer(obj, *emitter);
		}
	}

	return emitter;
}

NonOwningPtr<graphics::particles::ParticleSystem> create_particle_system(const script_tree::ObjectNode &object,
	graphics::particles::ParticleSystemManager &particle_system_manager)
{
	auto &name = object
		.Property("name")[0]
		.Get<ScriptType::String>()
		.value_or(""s)
		.Get();

	auto particle_system = particle_system_manager.CreateParticleSystem(name);

	if (particle_system)
	{
		for (auto &property : object.Properties())
		{
			if (property.Name() == "");
		}

		for (auto &obj : object.Objects())
		{
			if (obj.Name() == "emitter")
				detail::create_emitter(obj, *particle_system);

			else if (obj.Name() == "color-fader")
				detail::create_color_fader(obj, *particle_system);
			else if (obj.Name() == "direction-randomizer")
				detail::create_direction_randomizer(obj, *particle_system);
			else if (obj.Name() == "graviation")
				detail::create_graviation(obj, *particle_system);
			else if (obj.Name() == "linear-force")
				detail::create_linear_force(obj, *particle_system);
			else if (obj.Name() == "scaler")
				detail::create_scaler(obj, *particle_system);
			else if (obj.Name() == "sine-force")
				detail::create_sine_force(obj, *particle_system);
			else if (obj.Name() == "velocity-randomizer")
				detail::create_velocity_randomizer(obj, *particle_system);
		}
	}

	return particle_system;
}


NonOwningPtr<graphics::particles::affectors::ColorFader> create_color_fader(const script_tree::ObjectNode &object,
	graphics::particles::affectors::AffectorManager &affector_manager)
{
	auto &name = object
		.Property("name")[0]
		.Get<ScriptType::String>()
		.value_or(""s)
		.Get();

	auto color_fader = affector_manager.CreateAffector<graphics::particles::affectors::ColorFader>(name);

	if (color_fader)
	{
		for (auto &property : object.Properties())
		{
			if (property.Name() == "");
		}

		for (auto &obj : object.Objects())
		{
			if (obj.Name() == "step")
			{
				for (auto &property : obj.Properties())
				{
					if (property.Name() == "");
				}
			}
		}
	}

	return color_fader;
}

NonOwningPtr<graphics::particles::affectors::DirectionRandomizer> create_direction_randomizer(const script_tree::ObjectNode &object,
	graphics::particles::affectors::AffectorManager &affector_manager)
{
	auto &name = object
		.Property("name")[0]
		.Get<ScriptType::String>()
		.value_or(""s)
		.Get();

	auto direction_randomizer = affector_manager.CreateAffector<graphics::particles::affectors::DirectionRandomizer>(name);

	if (direction_randomizer)
	{
		for (auto &property : object.Properties())
		{
			if (property.Name() == "");
		}
	}

	return direction_randomizer;
}

NonOwningPtr<graphics::particles::affectors::Gravitation> create_graviation(const script_tree::ObjectNode &object,
	graphics::particles::affectors::AffectorManager &affector_manager)
{
	auto &name = object
		.Property("name")[0]
		.Get<ScriptType::String>()
		.value_or(""s)
		.Get();

	auto gravitation = affector_manager.CreateAffector<graphics::particles::affectors::Gravitation>(name);

	if (gravitation)
	{
		for (auto &property : object.Properties())
		{
			if (property.Name() == "");
		}
	}

	return gravitation;
}

NonOwningPtr<graphics::particles::affectors::LinearForce> create_linear_force(const script_tree::ObjectNode &object,
	graphics::particles::affectors::AffectorManager &affector_manager)
{
	auto &name = object
		.Property("name")[0]
		.Get<ScriptType::String>()
		.value_or(""s)
		.Get();

	auto linear_force = affector_manager.CreateAffector<graphics::particles::affectors::LinearForce>(name);

	if (linear_force)
	{
		for (auto &property : object.Properties())
		{
			if (property.Name() == "");
		}
	}

	return linear_force;
}

NonOwningPtr<graphics::particles::affectors::Scaler> create_scaler(const script_tree::ObjectNode &object,
	graphics::particles::affectors::AffectorManager &affector_manager)
{
	auto &name = object
		.Property("name")[0]
		.Get<ScriptType::String>()
		.value_or(""s)
		.Get();

	auto scaler = affector_manager.CreateAffector<graphics::particles::affectors::Scaler>(name);

	if (scaler)
	{
		for (auto &property : object.Properties())
		{
			if (property.Name() == "");
		}

		for (auto &obj : object.Objects())
		{
			if (obj.Name() == "step")
			{
				for (auto &property : obj.Properties())
				{
					if (property.Name() == "");
				}
			}
		}
	}

	return scaler;
}

NonOwningPtr<graphics::particles::affectors::SineForce> create_sine_force(const script_tree::ObjectNode &object,
	graphics::particles::affectors::AffectorManager &affector_manager)
{
	auto &name = object
		.Property("name")[0]
		.Get<ScriptType::String>()
		.value_or(""s)
		.Get();

	auto sine_force = affector_manager.CreateAffector<graphics::particles::affectors::SineForce>(name);

	if (sine_force)
	{
		for (auto &property : object.Properties())
		{
			if (property.Name() == "");
		}
	}

	return sine_force;
}

NonOwningPtr<graphics::particles::affectors::VelocityRandomizer> create_velocity_randomizer(const script_tree::ObjectNode &object,
	graphics::particles::affectors::AffectorManager &affector_manager)
{
	auto &name = object
		.Property("name")[0]
		.Get<ScriptType::String>()
		.value_or(""s)
		.Get();

	auto velocity_randomizer = affector_manager.CreateAffector<graphics::particles::affectors::VelocityRandomizer>(name);

	if (velocity_randomizer)
	{
		for (auto &property : object.Properties())
		{
			if (property.Name() == "");
		}
	}

	return velocity_randomizer;
}

} //particle_system_script_interface::detail


//Private

ScriptValidator ParticleSystemScriptInterface::GetValidator() const
{
	return detail::get_particle_system_validator();
}


/*
	Particle systems
	Creating from script
*/

void ParticleSystemScriptInterface::CreateParticleSystems(std::string_view asset_name,
	graphics::particles::ParticleSystemManager &particle_system_manager,
	const graphics::materials::MaterialManager &material_manager)
{
	if (Load(asset_name))
	{
		for (auto &object : tree_->Objects())
			detail::create_particle_system(object, particle_system_manager);
	}
}

} //ion::script::interfaces