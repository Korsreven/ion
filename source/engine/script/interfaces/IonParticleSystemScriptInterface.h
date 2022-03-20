/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonParticleSystemScriptInterface.h
-------------------------------------------
*/

#ifndef ION_PARTICLE_SYSTEM_SCRIPT_INTERFACE_H
#define ION_PARTICLE_SYSTEM_SCRIPT_INTERFACE_H

#include <string_view>

#include "IonScriptInterface.h"
#include "assets/repositories/IonScriptRepository.h"
#include "script/IonScriptValidator.h"

//Forward declarations
namespace ion::graphics
{
	namespace materials
	{
		struct MaterialManager;
	}

	namespace particles
	{
		struct ParticleSystemManager;
	}
}

namespace ion::script::interfaces
{
	namespace particle_system_script_interface::detail
	{
		/*
			Classes
		*/

		script_validator::ClassDefinition get_affector_class();
		script_validator::ClassDefinition get_emitter_class();
		script_validator::ClassDefinition get_particle_system_class();

		script_validator::ClassDefinition get_color_fader_class();
		script_validator::ClassDefinition get_direction_randomizer_class();
		script_validator::ClassDefinition get_graviation_class();
		script_validator::ClassDefinition get_linear_force_class();
		script_validator::ClassDefinition get_scaler_class();
		script_validator::ClassDefinition get_sine_force_class();
		script_validator::ClassDefinition get_velocity_randomizer_class();


		/*
			Validators
		*/

		ScriptValidator get_particle_system_validator();
	} //particle_system_script_interface::detail


	class ParticleSystemScriptInterface final : public ScriptInterface
	{
		private:

			ScriptValidator GetValidator() const override;

		public:

			//Default constructor
			ParticleSystemScriptInterface() = default;


			/*
				Particle systems
				Creating from script
			*/

			//Create particle systems from a script (or object file) with the given asset name
			void CreateParticleSystems(std::string_view asset_name,
				graphics::particles::ParticleSystemManager &particle_system_manager, graphics::materials::MaterialManager &material_manager);
	};
} //ion::script::interfaces

#endif