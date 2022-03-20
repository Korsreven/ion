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
#include "graphics/particles/IonParticleSystemManager.h"

namespace ion::script::interfaces
{

using namespace std::string_literals;
using namespace particle_system_script_interface;

namespace particle_system_script_interface::detail
{

/*
	Classes
*/

script_validator::ClassDefinition get_affector_class()
{
	return script_validator::ClassDefinition::Create("affector")
		.AddRequiredProperty("name", script_validator::ParameterType::String)
		.AddProperty("enabled", script_validator::ParameterType::Boolean);
}

script_validator::ClassDefinition get_emitter_class()
{
	return script_validator::ClassDefinition::Create("emitter")
		.AddClass("color-fader")
		.AddClass("direction-randomizer")
		.AddClass("gravitation")
		.AddClass("linear-force")
		.AddClass("scaler")
		.AddClass("sine-force")
		.AddClass("velocity-randomizer")

		.AddRequiredProperty("name", script_validator::ParameterType::String)
		.AddProperty("direction", script_validator::ParameterType::Vector2)
		.AddProperty("emission-angle", script_validator::ParameterType::FloatingPoint)
		.AddProperty("emission-duration", script_validator::ParameterType::FloatingPoint)
		.AddProperty("emission-rate", script_validator::ParameterType::FloatingPoint)	
		.AddProperty("inner-size", script_validator::ParameterType::Vector2)
		.AddProperty("particle-color", {script_validator::ParameterType::Color, script_validator::ParameterType::Color}, 1)
		.AddProperty("particle-lifetime", {script_validator::ParameterType::FloatingPoint, script_validator::ParameterType::FloatingPoint}, 1)
		.AddProperty("particle-mass", {script_validator::ParameterType::FloatingPoint, script_validator::ParameterType::FloatingPoint}, 1)
		.AddProperty("particle-material", script_validator::ParameterType::String)
		.AddProperty("particle-size", {script_validator::ParameterType::Vector2, script_validator::ParameterType::Vector2}, 1)
		.AddProperty("particle-velocity", {script_validator::ParameterType::FloatingPoint, script_validator::ParameterType::FloatingPoint}, 1)
		.AddProperty("particle-quota", script_validator::ParameterType::Integer)
		.AddProperty("position", script_validator::ParameterType::Vector2)
		.AddProperty("size", script_validator::ParameterType::Vector2)
		.AddProperty("type", {"point"s, "box"s, "ring"s});
}

script_validator::ClassDefinition get_particle_system_class()
{
	return script_validator::ClassDefinition::Create("particle-system")
		.AddClass(get_emitter_class())
		.AddClass(get_color_fader_class())
		.AddClass(get_direction_randomizer_class())
		.AddClass(get_graviation_class())
		.AddClass(get_linear_force_class())
		.AddClass(get_scaler_class())
		.AddClass(get_sine_force_class())
		.AddClass(get_velocity_randomizer_class())

		.AddRequiredProperty("name", script_validator::ParameterType::String)
		.AddProperty("render-primitive", {"point"s, "rectangle"s});
}


script_validator::ClassDefinition get_color_fader_class()
{
	auto step = script_validator::ClassDefinition::Create("step")
		.AddRequiredProperty("percent", script_validator::ParameterType::FloatingPoint)
		.AddProperty("color", script_validator::ParameterType::Color);

	return script_validator::ClassDefinition::Create("color-fader", "affector")
		.AddClass(std::move(step));
}

script_validator::ClassDefinition get_direction_randomizer_class()
{
	return script_validator::ClassDefinition::Create("direction-randomizer", "affector")
		.AddProperty("angle", script_validator::ParameterType::FloatingPoint)
		.AddProperty("scope", script_validator::ParameterType::FloatingPoint);
}

script_validator::ClassDefinition get_graviation_class()
{
	return script_validator::ClassDefinition::Create("gravitation", "affector")
		.AddProperty("gravity", script_validator::ParameterType::FloatingPoint)
		.AddProperty("mass", script_validator::ParameterType::FloatingPoint)
		.AddProperty("position", script_validator::ParameterType::Vector2);
}

script_validator::ClassDefinition get_linear_force_class()
{
	return script_validator::ClassDefinition::Create("linear-force", "affector")
		.AddProperty("force", script_validator::ParameterType::Vector2)
		.AddProperty("type", {"add"s, "average"s});
}

script_validator::ClassDefinition get_scaler_class()
{
	auto step = script_validator::ClassDefinition::Create("step")
		.AddRequiredProperty("percent", script_validator::ParameterType::FloatingPoint)
		.AddProperty("size", script_validator::ParameterType::Vector2);

	return script_validator::ClassDefinition::Create("scaler", "affector")
		.AddClass(std::move(step));
}

script_validator::ClassDefinition get_sine_force_class()
{
	return script_validator::ClassDefinition::Create("sine-force", "affector")
		.AddProperty("force", script_validator::ParameterType::Vector2)
		.AddProperty("type", {"add"s, "average"s});
}

script_validator::ClassDefinition get_velocity_randomizer_class()
{
	return script_validator::ClassDefinition::Create("velocity-randomizer", "affector")
		.AddProperty("scope", script_validator::ParameterType::FloatingPoint)
		.AddProperty("velocity", {script_validator::ParameterType::FloatingPoint, script_validator::ParameterType::FloatingPoint}, 1);
}


/*
	Validators
*/

ScriptValidator get_particle_system_validator()
{
	return ScriptValidator::Create()
		.AddAbstractClass(get_affector_class())
		.AddRequiredClass(get_particle_system_class());
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
	graphics::particles::ParticleSystemManager &particle_system_manager, graphics::materials::MaterialManager &material_manager)
{
	if (Execute(asset_name))
	{
		auto &tree = builder_.Tree();
		auto name =
			tree->Find("particle-system")
				.Property("name")[0]
				.Get<ScriptType::String>()
				.value_or(""s)
				.Get();

		particle_system_manager.CreateParticleSystem(std::move(name));
	}
}

} //ion::script::interfaces