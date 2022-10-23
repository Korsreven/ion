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
#include "graphics/particles/affectors/IonAffector.h"
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

namespace ion::script::interfaces
{
	namespace particle_system_script_interface::detail
	{
		NonOwningPtr<graphics::materials::Material> get_material(std::string_view name, const ManagerRegister &managers) noexcept;


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

		void set_emitter_properties(const script_tree::ObjectNode &object, graphics::particles::Emitter &emitter,
			const ManagerRegister &managers);
		void set_particle_system_properties(const script_tree::ObjectNode &object, graphics::particles::ParticleSystem &particle_system,
			const ManagerRegister &managers);

		void set_affector_properties(const script_tree::ObjectNode &object, graphics::particles::affectors::Affector &affector);
		void set_color_fader_properties(const script_tree::ObjectNode &object, graphics::particles::affectors::ColorFader &color_fader);
		void set_direction_randomizer_properties(const script_tree::ObjectNode &object, graphics::particles::affectors::DirectionRandomizer &direction_randomizer);
		void set_gravitation_properties(const script_tree::ObjectNode &object, graphics::particles::affectors::Gravitation &gravitation);
		void set_linear_force_properties(const script_tree::ObjectNode &object, graphics::particles::affectors::LinearForce &linear_force);
		void set_scaler_properties(const script_tree::ObjectNode &object, graphics::particles::affectors::Scaler &scaler);
		void set_sine_force_properties(const script_tree::ObjectNode &object, graphics::particles::affectors::SineForce &sine_force);
		void set_velocity_randomizer_properties(const script_tree::ObjectNode &object, graphics::particles::affectors::VelocityRandomizer &velocity_randomizer);


		NonOwningPtr<graphics::particles::Emitter> create_emitter(const script_tree::ObjectNode &object,
			graphics::particles::ParticleSystem &particle_system, const ManagerRegister &managers);
		NonOwningPtr<graphics::particles::ParticleSystem> create_particle_system(const script_tree::ObjectNode &object,
			graphics::particles::ParticleSystemManager &particle_system_manager, const ManagerRegister &managers);

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
			graphics::particles::ParticleSystemManager &particle_system_manager, const ManagerRegister &managers);
	} //particle_system_script_interface::detail


	//A class representing an interface to a particle system script with a complete validation scheme
	//A particle system script can load particle systems from a script file into a particle system manager
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

			//Creates particle systems from a script (or object file) with the given asset name
			void CreateParticleSystems(std::string_view asset_name,
				graphics::particles::ParticleSystemManager &particle_system_manager);

			//Creates particle systems from a script (or object file) with the given asset name
			void CreateParticleSystems(std::string_view asset_name,
				graphics::particles::ParticleSystemManager &particle_system_manager, const ManagerRegister &managers);
	};
} //ion::script::interfaces

#endif