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

ScriptValidator get_validator()
{
	return ScriptValidator::Create();
}

} //particle_system_script_interface::detail


//Private

ScriptValidator ParticleSystemScriptInterface::GetValidator() const
{
	return detail::get_validator();
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
			tree->Find("ParticleSystem")
				.Property("name")[0]
				.Get<ion::script::ScriptType::String>()
				.value_or(""s)
				.Get();

		particle_system_manager.CreateParticleSystem(std::move(name));
	}
}

} //ion::script::interfaces