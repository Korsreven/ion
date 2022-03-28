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
#include "graphics/particles/IonEmitter.h"
#include "graphics/particles/IonEmitterManager.h"
#include "graphics/particles/IonParticleSystem.h"
#include "graphics/particles/IonParticleSystemManager.h"
#include "graphics/particles/affectors/IonAffectorManager.h"
#include "graphics/particles/affectors/IonColorFader.h"
#include "graphics/particles/affectors/IonDirectionRandomizer.h"
#include "graphics/particles/affectors/IonGravitation.h"
#include "graphics/particles/affectors/IonLinearForce.h"
#include "graphics/particles/affectors/IonScaler.h"
#include "graphics/particles/affectors/IonSineForce.h"
#include "graphics/particles/affectors/IonVelocityRandomizer.h"
#include "memory/IonNonOwningPtr.h"
#include "script/IonScriptTree.h"
#include "script/IonScriptValidator.h"

//Forward declarations
namespace ion::graphics
{
	namespace materials
	{
		struct MaterialManager;
	}
}

namespace ion::script::interfaces
{
	namespace particle_system_script_interface::detail
	{
		/*
			Validator classes
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

		ScriptValidator get_particle_system_validator();


		/*
			Tree parsing
		*/

		NonOwningPtr<graphics::particles::Emitter> create_emitter(const script_tree::ObjectNode &object,
			graphics::particles::ParticleSystem &particle_system,
			graphics::materials::MaterialManager &material_manager);
		NonOwningPtr<graphics::particles::ParticleSystem> create_particle_system(const script_tree::ObjectNode &object,
			graphics::particles::ParticleSystemManager &particle_system_manager,
			graphics::materials::MaterialManager &material_manager);

		NonOwningPtr<graphics::particles::affectors::ColorFader> create_color_fader(const script_tree::ObjectNode &object,
			graphics::particles::affectors::AffectorManager &affector_manager);
		NonOwningPtr<graphics::particles::affectors::DirectionRandomizer> create_direction_randomizer(const script_tree::ObjectNode &object,
			graphics::particles::affectors::AffectorManager &affector_manager);
		NonOwningPtr<graphics::particles::affectors::Gravitation> create_graviation(const script_tree::ObjectNode &object,
			graphics::particles::affectors::AffectorManager &affector_manager);
		NonOwningPtr<graphics::particles::affectors::LinearForce> create_linear_force(const script_tree::ObjectNode &object,
			graphics::particles::affectors::AffectorManager &affector_manager);
		NonOwningPtr<graphics::particles::affectors::Scaler> create_scaler(const script_tree::ObjectNode &object,
			graphics::particles::affectors::AffectorManager &affector_manager);
		NonOwningPtr<graphics::particles::affectors::SineForce> create_sine_force(const script_tree::ObjectNode &object,
			graphics::particles::affectors::AffectorManager &affector_manager);
		NonOwningPtr<graphics::particles::affectors::VelocityRandomizer> create_velocity_randomizer(const script_tree::ObjectNode &object,
			graphics::particles::affectors::AffectorManager &affector_manager);

		void create_particle_systems(const ScriptTree &tree,
			graphics::particles::ParticleSystemManager &particle_system_manager,
			graphics::materials::MaterialManager &material_manager);
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
				graphics::particles::ParticleSystemManager &particle_system_manager,
				graphics::materials::MaterialManager &material_manager);
	};
} //ion::script::interfaces

#endif