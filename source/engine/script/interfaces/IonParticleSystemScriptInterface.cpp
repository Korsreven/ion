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
using namespace script_validator;
using namespace particle_system_script_interface;

namespace particle_system_script_interface::detail
{

/*
	Classes
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
	graphics::particles::ParticleSystemManager &particle_system_manager,
	const graphics::materials::MaterialManager &material_manager)
{
	if (Load(asset_name))
	{
		auto name =
			tree_->Find("particle-system")
				.Property("name")[0]
				.Get<ScriptType::String>()
				.value_or(""s)
				.Get();

		particle_system_manager.CreateParticleSystem(std::move(name));
	}
}

} //ion::script::interfaces