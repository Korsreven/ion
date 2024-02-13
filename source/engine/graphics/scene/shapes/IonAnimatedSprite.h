/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/shapes
File:	IonAnimatedSprite.h
-------------------------------------------
*/

#ifndef ION_ANIMATED_SPRITE_H
#define ION_ANIMATED_SPRITE_H

#include <optional>
#include <string>

#include "IonSprite.h"
#include "graphics/materials/IonMaterial.h"
#include "graphics/textures/IonAnimation.h"
#include "memory/IonNonOwningPtr.h"
#include "memory/IonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene::shapes
{
	using namespace types::type_literals;

	namespace animated_sprite::detail
	{
	} //animated_sprite::detail


	///@brief A class representing a texturized rectangle that can animate
	class AnimatedSprite final : public Sprite
	{
		protected:

			OwningPtr<textures::Animation> diffuse_animation_;
			OwningPtr<textures::Animation> specular_animation_;
			OwningPtr<textures::Animation> normal_animation_;

			OwningPtr<materials::Material> material_;
			NonOwningPtr<materials::Material> initial_material_;

		public:

			///@brief Constructs a new animated sprite with the given name, material and visibility
			AnimatedSprite(std::optional<std::string> name,
				NonOwningPtr<materials::Material> material, bool visible = true);

			///@brief Constructs a new animated sprite with the given name, size, material and visibility
			AnimatedSprite(std::optional<std::string> name, const Vector2 &size,
				NonOwningPtr<materials::Material> material, bool visible = true);

			///@brief Constructs a new animated sprite with the given name, position, size, material and visibility
			AnimatedSprite(std::optional<std::string> name, const Vector3 &position, const Vector2 &size,
				NonOwningPtr<materials::Material> material, bool visible = true);

			///@brief Constructs a new animated sprite with the given name, position, rotation, size, material and visibility
			AnimatedSprite(std::optional<std::string> name, const Vector3 &position, real rotation, const Vector2 &size,
				NonOwningPtr<materials::Material> material, bool visible = true);


			///@brief Constructs a new animated sprite with the given name, material, color and visibility
			AnimatedSprite(std::optional<std::string> name,
				NonOwningPtr<materials::Material> material, const Color &color, bool visible = true);

			///@brief Constructs a new animated sprite with the given name, size, material, color and visibility
			AnimatedSprite(std::optional<std::string> name, const Vector2 &size,
				NonOwningPtr<materials::Material> material, const Color &color, bool visible = true);

			///@brief Constructs a new animated sprite with the given name, position, size, material, color and visibility
			AnimatedSprite(std::optional<std::string> name, const Vector3 &position, const Vector2 &size,
				NonOwningPtr<materials::Material> material, const Color &color, bool visible = true);

			///@brief Constructs a new animated sprite with the given name, position, rotation, size, material, color and visibility
			AnimatedSprite(std::optional<std::string> name, const Vector3 &position, real rotation, const Vector2 &size,
				NonOwningPtr<materials::Material> material, const Color &color, bool visible = true);


			/**
				@name Modifiers
				@{
			*/

			///@brief Reverts to the initial animated material
			void Revert();

			///@}

			/**
				@name Playback
				@{
			*/

			///@brief Starts or resumes, animated sprite playback
			void Start() noexcept;

			///@brief Stops animated sprite playback
			void Stop() noexcept;

			///@brief Stops animated sprite playback and reset elapsed time to zero
			void Reset() noexcept;

			///@brief Stops, resets and starts animated sprite playback
			void Restart() noexcept;


			///@brief Jumps forward by the given amount of time
			void JumpForward(duration time) noexcept;

			///@brief Jumps backward by the given amount of time
			void JumpBackward(duration time) noexcept;

			///@}

			/**
				@name Elapse time
				@{
			*/

			///@brief Elapses the total time for this animation by the given time in seconds
			///@details This function is typically called each frame, with the time in seconds since last frame
			virtual void Elapse(duration time) noexcept override;

			///@}
	};
} //ion::graphics::scene::shapes

#endif