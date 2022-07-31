/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/shapes
File:	IonAnimatedSprite.cpp
-------------------------------------------
*/

#include "IonAnimatedSprite.h"

namespace ion::graphics::scene::shapes
{

using namespace animated_sprite;

namespace animated_sprite::detail
{
} //animated_sprite::detail


AnimatedSprite::AnimatedSprite(NonOwningPtr<materials::Material> material, bool visible) :
	AnimatedSprite{vector3::Zero, vector2::Zero, material, visible}
{
	//Empty
}

AnimatedSprite::AnimatedSprite(const Vector2 &size, NonOwningPtr<materials::Material> material, bool visible) :
	AnimatedSprite{vector3::Zero, size, material, visible}
{
	//Empty
}

AnimatedSprite::AnimatedSprite(const Vector3 &position, const Vector2 &size, NonOwningPtr<materials::Material> material, bool visible) :
	AnimatedSprite{position, 0.0_r, size, material, visible}
{
	//Empty
}

AnimatedSprite::AnimatedSprite(const Vector3 &position, real rotation, const Vector2 &size, NonOwningPtr<materials::Material> material, bool visible) :
	AnimatedSprite{position, rotation, size, material, color::White, visible}
{
	//Empty
}


AnimatedSprite::AnimatedSprite(NonOwningPtr<materials::Material> material, const Color &color, bool visible) :
	AnimatedSprite{vector3::Zero, vector2::Zero, material, color, visible}
{
	//Empty
}

AnimatedSprite::AnimatedSprite(const Vector2 &size, NonOwningPtr<materials::Material> material, const Color &color, bool visible) :
	AnimatedSprite{vector3::Zero, size, material, color, visible}
{
	//Empty
}

AnimatedSprite::AnimatedSprite(const Vector3 &position, const Vector2 &size, NonOwningPtr<materials::Material> material, const Color &color, bool visible) :
	AnimatedSprite{position, 0.0_r, size, material, color, visible}
{
	//Empty
}

AnimatedSprite::AnimatedSprite(const Vector3 &position, real rotation, const Vector2 &size, NonOwningPtr<materials::Material> material, const Color &color, bool visible) :

	Sprite{position, rotation, size, nullptr, color, visible},

	diffuse_animation_{material && material->DiffuseMap().first ?
		make_owning<textures::Animation>(*material->DiffuseMap().first) : nullptr},
	specular_animation_{material && material->SpecularMap().first ?
		make_owning<textures::Animation>(*material->SpecularMap().first) : nullptr},
	normal_animation_{material && material->NormalMap().first ?
		make_owning<textures::Animation>(*material->NormalMap().first) : nullptr},

	material_{material ? make_owning<materials::Material>(*material) : nullptr},
	initial_material_{material}
{
	material_->DiffuseMap(diffuse_animation_);
	material_->SpecularMap(specular_animation_);
	material_->NormalMap(normal_animation_);
	SurfaceMaterial(material_);
}


/*
	Modifiers
*/

void AnimatedSprite::Revert()
{
	if (material_ && initial_material_)
	{
		if (diffuse_animation_ && initial_material_->DiffuseMap().first)
			*diffuse_animation_ = *initial_material_->DiffuseMap().first;

		if (specular_animation_ && initial_material_->SpecularMap().first)
			*specular_animation_ = *initial_material_->SpecularMap().first;

		if (normal_animation_ && initial_material_->NormalMap().first)
			*normal_animation_ = *initial_material_->NormalMap().first;

		*material_ = *initial_material_;
		material_->DiffuseMap(diffuse_animation_);
		material_->SpecularMap(specular_animation_);
		material_->NormalMap(normal_animation_);
	}
}


/*
	Playback
*/

void AnimatedSprite::Start() noexcept
{
	if (diffuse_animation_)
		diffuse_animation_->Start();
	if (specular_animation_)
		specular_animation_->Start();
	if (normal_animation_)
		normal_animation_->Start();
}

void AnimatedSprite::Stop() noexcept
{
	if (diffuse_animation_)
		diffuse_animation_->Stop();
	if (specular_animation_)
		specular_animation_->Stop();
	if (normal_animation_)
		normal_animation_->Stop();
}

void AnimatedSprite::Reset() noexcept
{
	if (diffuse_animation_)
		diffuse_animation_->Reset();
	if (specular_animation_)
		specular_animation_->Reset();
	if (normal_animation_)
		normal_animation_->Reset();
}

void AnimatedSprite::Restart() noexcept
{
	if (diffuse_animation_)
		diffuse_animation_->Restart();
	if (specular_animation_)
		specular_animation_->Restart();
	if (normal_animation_)
		normal_animation_->Restart();
}


void AnimatedSprite::JumpForward(duration time) noexcept
{
	if (diffuse_animation_)
		diffuse_animation_->JumpForward(time);
	if (specular_animation_)
		specular_animation_->JumpForward(time);
	if (normal_animation_)
		normal_animation_->JumpForward(time);
}

void AnimatedSprite::JumpBackward(duration time) noexcept
{
	if (diffuse_animation_)
		diffuse_animation_->JumpBackward(time);
	if (specular_animation_)
		specular_animation_->JumpBackward(time);
	if (normal_animation_)
		normal_animation_->JumpBackward(time);
}


/*
	Elapse time
*/

void AnimatedSprite::Elapse(duration time) noexcept
{
	if (diffuse_animation_)
		diffuse_animation_->Elapse(time);
	if (specular_animation_)
		specular_animation_->Elapse(time);
	if (normal_animation_)
		normal_animation_->Elapse(time);

	Sprite::Elapse(time);
}

} //ion::graphics::scene::shapes