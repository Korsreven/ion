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


	//A class representing a texturized rectangle that can animate
	class AnimatedSprite final : public Sprite
	{
		protected:

			OwningPtr<textures::Animation> diffuse_animation_;
			OwningPtr<textures::Animation> specular_animation_;
			OwningPtr<textures::Animation> normal_animation_;

			OwningPtr<materials::Material> material_;
			NonOwningPtr<materials::Material> initial_material_;

		public:

			//Construct a new animated sprite with the given material and visibility
			explicit AnimatedSprite(NonOwningPtr<materials::Material> material, bool visible = true);

			//Construct a new animated sprite with the given size, material and visibility
			AnimatedSprite(const Vector2 &size, NonOwningPtr<materials::Material> material, bool visible = true);

			//Construct a new animated sprite with the given position, size, material and visibility
			AnimatedSprite(const Vector3 &position, const Vector2 &size, NonOwningPtr<materials::Material> material, bool visible = true);

			//Construct a new animated sprite with the given position, rotation, size, material and visibility
			AnimatedSprite(const Vector3 &position, real rotation, const Vector2 &size, NonOwningPtr<materials::Material> material, bool visible = true);


			//Construct a new animated sprite with the given material, color and visibility
			AnimatedSprite(NonOwningPtr<materials::Material> material, const Color &color, bool visible = true);

			//Construct a new animated sprite with the given size, material, color and visibility
			AnimatedSprite(const Vector2 &size, NonOwningPtr<materials::Material> material, const Color &color, bool visible = true);

			//Construct a new animated sprite with the given position, size, material, color and visibility
			AnimatedSprite(const Vector3 &position, const Vector2 &size, NonOwningPtr<materials::Material> material, const Color &color, bool visible = true);

			//Construct a new animated sprite with the given position, rotation, size, material, color and visibility
			AnimatedSprite(const Vector3 &position, real rotation, const Vector2 &size, NonOwningPtr<materials::Material> material, const Color &color, bool visible = true);


			/*
				Modifiers
			*/

			//Revert to the initial animated material
			void Revert();


			/*
				Playback
			*/

			//Starts or resumes, animated sprite playback
			void Start() noexcept;

			//Stops animated sprite playback
			void Stop() noexcept;

			//Stops animated sprite playback and reset elapsed time to zero
			void Reset() noexcept;

			//Stops, resets and starts animated sprite playback
			void Restart() noexcept;


			//Jump forward by the given amount of time
			void JumpForward(duration time) noexcept;

			//Jump backward by the given amount of time
			void JumpBackward(duration time) noexcept;


			/*
				Elapse time
			*/

			//Elapse the total time for this animation by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			virtual void Elapse(duration time) noexcept override;
	};
} //ion::graphics::scene::shapes

#endif