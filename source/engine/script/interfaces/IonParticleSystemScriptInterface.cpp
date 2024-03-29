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
#include "utilities/IonMath.h"

namespace ion::script::interfaces
{

using namespace std::string_literals;
using namespace script_validator;
using namespace particle_system_script_interface;
using namespace graphics::particles;

namespace particle_system_script_interface::detail
{

NonOwningPtr<graphics::materials::Material> get_material(std::string_view name, const ManagerRegister &managers) noexcept
{
	for (auto &material_manager : managers.ObjectsOf<graphics::materials::MaterialManager>())
	{
		if (material_manager)
		{
			if (auto material = material_manager->GetMaterial(name); material)
				return material;
		}
	}

	return nullptr;
}


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
		.AddProperty("emitting", ParameterType::Boolean)
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
		.AddProperty("emitting", ParameterType::Boolean)
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

void set_emitter_properties(const script_tree::ObjectNode &object, Emitter &emitter,
	const ManagerRegister &managers)
{
	for (auto &obj : object.Objects())
	{
		if (obj.Name() == "color-fader")
			create_color_fader(obj, emitter);
		else if (obj.Name() == "direction-randomizer")
			create_direction_randomizer(obj, emitter);
		else if (obj.Name() == "graviation")
			create_graviation(obj, emitter);
		else if (obj.Name() == "linear-force")
			create_linear_force(obj, emitter);
		else if (obj.Name() == "scaler")
			create_scaler(obj, emitter);
		else if (obj.Name() == "sine-force")
			create_sine_force(obj, emitter);
		else if (obj.Name() == "velocity-randomizer")
			create_velocity_randomizer(obj, emitter);
	}

	for (auto &property : object.Properties())
	{
		if (property.Name() == "direction")
			emitter.Direction(property[0].Get<ScriptType::Vector2>()->Get());
		else if (property.Name() == "emission-angle")
			emitter.EmissionAngle(utilities::math::ToRadians(property[0].Get<ScriptType::FloatingPoint>()->As<real>()));
		else if (property.Name() == "emission-duration")
			emitter.EmissionDuration(duration{property[0].Get<ScriptType::FloatingPoint>()->As<real>()});
		else if (property.Name() == "emission-rate")
			emitter.EmissionRate(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "emitting")
		{
			if (property[0].Get<ScriptType::Boolean>()->Get())
				emitter.Start();
			else
				emitter.Stop();
		}
		else if (property.Name() == "inner-size")
			emitter.InnerSize(property[0].Get<ScriptType::Vector2>()->Get());
		else if (property.Name() == "particle-color")
		{
			if (property.NumberOfArguments() == 2)
				emitter.ParticleColor(property[0].Get<ScriptType::Color>()->Get(),
									  property[1].Get<ScriptType::Color>()->Get());
			else
				emitter.ParticleColor(property[0].Get<ScriptType::Color>()->Get());
		}
		else if (property.Name() == "particle-lifetime")
		{
			if (property.NumberOfArguments() == 2)
				emitter.ParticleLifetime(duration{property[0].Get<ScriptType::FloatingPoint>()->As<real>()},
										 duration{property[1].Get<ScriptType::FloatingPoint>()->As<real>()});
			else
				emitter.ParticleLifetime(duration{property[0].Get<ScriptType::FloatingPoint>()->As<real>()});
		}
		else if (property.Name() == "particle-mass")
		{
			if (property.NumberOfArguments() == 2)
				emitter.ParticleMass(property[0].Get<ScriptType::FloatingPoint>()->As<real>(),
									 property[1].Get<ScriptType::FloatingPoint>()->As<real>());
			else
				emitter.ParticleMass(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		}
		else if (property.Name() == "particle-material")
			emitter.ParticleMaterial(get_material(property[0].Get<ScriptType::String>()->Get(), managers));
		else if (property.Name() == "particle-size")
		{
			if (property.NumberOfArguments() == 2)
				emitter.ParticleSize(property[0].Get<ScriptType::Vector2>()->Get(),
									 property[1].Get<ScriptType::Vector2>()->Get());
			else
				emitter.ParticleSize(property[0].Get<ScriptType::Vector2>()->Get());
		}
		else if (property.Name() == "particle-velocity")
		{
			if (property.NumberOfArguments() == 2)
				emitter.ParticleVelocity(property[0].Get<ScriptType::FloatingPoint>()->As<real>(),
										 property[1].Get<ScriptType::FloatingPoint>()->As<real>());
			else
				emitter.ParticleVelocity(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		}
		else if (property.Name() == "particle-quota")
			emitter.ParticleQuota(property[0].Get<ScriptType::Integer>()->As<int>());
		else if (property.Name() == "position")
			emitter.Position(property[0].Get<ScriptType::Vector2>()->Get());
		else if (property.Name() == "size")
			emitter.Size(property[0].Get<ScriptType::Vector2>()->Get());
		else if (property.Name() == "type")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "point")
				emitter.Type(emitter::EmitterType::Point);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "box")
				emitter.Type(emitter::EmitterType::Box);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "ring")
				emitter.Type(emitter::EmitterType::Ring);
		}
	}
}

void set_particle_system_properties(const script_tree::ObjectNode &object, ParticleSystem &particle_system,
	const ManagerRegister &managers)
{
	for (auto &obj : object.Objects())
	{
		if (obj.Name() == "emitter")
			create_emitter(obj, particle_system, managers);

		else if (obj.Name() == "color-fader")
			create_color_fader(obj, particle_system);
		else if (obj.Name() == "direction-randomizer")
			create_direction_randomizer(obj, particle_system);
		else if (obj.Name() == "graviation")
			create_graviation(obj, particle_system);
		else if (obj.Name() == "linear-force")
			create_linear_force(obj, particle_system);
		else if (obj.Name() == "scaler")
			create_scaler(obj, particle_system);
		else if (obj.Name() == "sine-force")
			create_sine_force(obj, particle_system);
		else if (obj.Name() == "velocity-randomizer")
			create_velocity_randomizer(obj, particle_system);
	}

	for (auto &property : object.Properties())
	{
		if (property.Name() == "emitting")
		{
			if (property[0].Get<ScriptType::Boolean>()->Get())
				particle_system.StartAll();
			else
				particle_system.StopAll();
		}
		else if (property.Name() == "render-primitive")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "point")
				particle_system.RenderPrimitive(particle_system::ParticlePrimitive::Point);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "rectangle")
				particle_system.RenderPrimitive(particle_system::ParticlePrimitive::Rectangle);
		}
	}
}


void set_affector_properties(const script_tree::ObjectNode &object, affectors::Affector &affector)
{
	for (auto &property : object.Properties())
	{
		if (property.Name() == "enabled")
			affector.Enabled(property[0].Get<ScriptType::Boolean>()->Get());
	}
}

void set_color_fader_properties(const script_tree::ObjectNode &object, affectors::ColorFader &color_fader)
{
	set_affector_properties(object, color_fader);

	for (auto &obj : object.Objects())
	{
		if (obj.Name() == "step")
		{
			auto percent = obj
				.Property("percent")[0]
				.Get<ScriptType::FloatingPoint>()->As<real>();
			auto color = std::optional<graphics::utilities::Color>{};

			for (auto &property : obj.Properties())
			{
				if (property.Name() == "color")
					color = property[0].Get<ScriptType::Color>()->Get();
			}

			color_fader.AddStep(percent, color);
		}
	}
}

void set_direction_randomizer_properties(const script_tree::ObjectNode &object, affectors::DirectionRandomizer &direction_randomizer)
{
	set_affector_properties(object, direction_randomizer);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "angle")
			direction_randomizer.Angle(utilities::math::ToRadians(property[0].Get<ScriptType::FloatingPoint>()->As<real>()));
		else if (property.Name() == "scope")
			direction_randomizer.Scope(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
	}
}

void set_gravitation_properties(const script_tree::ObjectNode &object, affectors::Gravitation &gravitation)
{
	set_affector_properties(object, gravitation);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "gravity")
			gravitation.Gravity(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "mass")
			gravitation.Mass(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "position")
			gravitation.Position(property[0].Get<ScriptType::Vector2>()->Get());
	}
}

void set_linear_force_properties(const script_tree::ObjectNode &object, affectors::LinearForce &linear_force)
{
	set_affector_properties(object, linear_force);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "force")
			linear_force.Force(property[0].Get<ScriptType::Vector2>()->Get());
		else if (property.Name() == "type")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "add")
				linear_force.Type(affectors::linear_force::ForceType::Add);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "average")
				linear_force.Type(affectors::linear_force::ForceType::Average);
		}
	}
}

void set_scaler_properties(const script_tree::ObjectNode &object, affectors::Scaler &scaler)
{
	set_affector_properties(object, scaler);

	for (auto &obj : object.Objects())
	{
		if (obj.Name() == "step")
		{
			auto percent = obj
				.Property("percent")[0]
				.Get<ScriptType::FloatingPoint>()->As<real>();
			auto size = std::optional<graphics::utilities::Vector2>{};

			for (auto &property : obj.Properties())
			{
				if (property.Name() == "size")
					size = property[0].Get<ScriptType::Vector2>()->Get();
			}

			scaler.AddStep(percent, size);
		}
	}
}

void set_sine_force_properties(const script_tree::ObjectNode &object, affectors::SineForce &sine_force)
{
	set_affector_properties(object, sine_force);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "force")
			sine_force.Force(property[0].Get<ScriptType::Vector2>()->Get());
		else if (property.Name() == "type")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "add")
				sine_force.Type(affectors::sine_force::ForceType::Add);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "average")
				sine_force.Type(affectors::sine_force::ForceType::Average);
		}
	}
}

void set_velocity_randomizer_properties(const script_tree::ObjectNode &object, affectors::VelocityRandomizer &velocity_randomizer)
{
	set_affector_properties(object, velocity_randomizer);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "scope")
			velocity_randomizer.Scope(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "velocity")
		{
			if (property.NumberOfArguments() == 2)
				velocity_randomizer.Velocity(property[0].Get<ScriptType::FloatingPoint>()->As<real>(),
											 property[1].Get<ScriptType::FloatingPoint>()->As<real>());
			else
				velocity_randomizer.Velocity(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		}
	}
}



NonOwningPtr<Emitter> create_emitter(const script_tree::ObjectNode &object,
	ParticleSystem &particle_system, const ManagerRegister &managers)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();

	auto emitter = particle_system.CreateEmitter(std::move(name));

	if (emitter)
		set_emitter_properties(object, *emitter, managers);

	return emitter;
}

NonOwningPtr<ParticleSystem> create_particle_system(const script_tree::ObjectNode &object,
	ParticleSystemManager &particle_system_manager, const ManagerRegister &managers)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();

	auto particle_system = particle_system_manager.CreateParticleSystem(std::move(name));

	if (particle_system)
		set_particle_system_properties(object, *particle_system, managers);

	return particle_system;
}


NonOwningPtr<affectors::ColorFader> create_color_fader(const script_tree::ObjectNode &object,
	affectors::AffectorManager &affector_manager)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();

	auto color_fader = affector_manager.CreateAffector<affectors::ColorFader>(std::move(name));

	if (color_fader)
		set_color_fader_properties(object, *color_fader);

	return color_fader;
}

NonOwningPtr<affectors::DirectionRandomizer> create_direction_randomizer(const script_tree::ObjectNode &object,
	affectors::AffectorManager &affector_manager)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();

	auto direction_randomizer = affector_manager.CreateAffector<affectors::DirectionRandomizer>(std::move(name));

	if (direction_randomizer)
		set_direction_randomizer_properties(object, *direction_randomizer);

	return direction_randomizer;
}

NonOwningPtr<affectors::Gravitation> create_graviation(const script_tree::ObjectNode &object,
	affectors::AffectorManager &affector_manager)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();

	auto gravitation = affector_manager.CreateAffector<affectors::Gravitation>(std::move(name));

	if (gravitation)
		set_gravitation_properties(object, *gravitation);

	return gravitation;
}

NonOwningPtr<affectors::LinearForce> create_linear_force(const script_tree::ObjectNode &object,
	affectors::AffectorManager &affector_manager)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();

	auto linear_force = affector_manager.CreateAffector<affectors::LinearForce>(std::move(name));

	if (linear_force)
		set_linear_force_properties(object, *linear_force);

	return linear_force;
}

NonOwningPtr<affectors::Scaler> create_scaler(const script_tree::ObjectNode &object,
	affectors::AffectorManager &affector_manager)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();

	auto scaler = affector_manager.CreateAffector<affectors::Scaler>(std::move(name));

	if (scaler)
		set_scaler_properties(object, *scaler);

	return scaler;
}

NonOwningPtr<affectors::SineForce> create_sine_force(const script_tree::ObjectNode &object,
	affectors::AffectorManager &affector_manager)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();

	auto sine_force = affector_manager.CreateAffector<affectors::SineForce>(std::move(name));

	if (sine_force)
		set_sine_force_properties(object, *sine_force);

	return sine_force;
}

NonOwningPtr<affectors::VelocityRandomizer> create_velocity_randomizer(const script_tree::ObjectNode &object,
	affectors::AffectorManager &affector_manager)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();

	auto velocity_randomizer = affector_manager.CreateAffector<affectors::VelocityRandomizer>(std::move(name));

	if (velocity_randomizer)
		set_velocity_randomizer_properties(object, *velocity_randomizer);

	return velocity_randomizer;
}


void create_particle_systems(const ScriptTree &tree,
	ParticleSystemManager &particle_system_manager, const ManagerRegister &managers)
{
	for (auto &object : tree.Objects())
	{
		if (object.Name() == "particle-system")
			create_particle_system(object, particle_system_manager, managers);
	}
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
	ParticleSystemManager &particle_system_manager)
{
	if (Load(asset_name))
		detail::create_particle_systems(*tree_, particle_system_manager, Managers());
}

void ParticleSystemScriptInterface::CreateParticleSystems(std::string_view asset_name,
	ParticleSystemManager &particle_system_manager, const ManagerRegister &managers)
{
	if (Load(asset_name))
		detail::create_particle_systems(*tree_, particle_system_manager, managers);
}

} //ion::script::interfaces