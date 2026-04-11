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


AnimatedSprite::AnimatedSprite(std::optional<std::string> name,
	NonOwningPtr<materials::Material> material, bool visible) :

	AnimatedSprite{std::move(name), vector3::Zero, vector2::Zero, material, visible}
{
	//Empty
}

AnimatedSprite::AnimatedSprite(std::optional<std::string> name, const Vector2 &size,
	NonOwningPtr<materials::Material> material, bool visible) :

	AnimatedSprite{std::move(name), vector3::Zero, size, material, visible}
{
	//Empty
}

AnimatedSprite::AnimatedSprite(std::optional<std::string> name, const Vector3 &position, const Vector2 &size,
	NonOwningPtr<materials::Material> material, bool visible) :

	AnimatedSprite{std::move(name), position, 0.0_r, size, material, visible}
{
	//Empty
}

AnimatedSprite::AnimatedSprite(std::optional<std::string> name, const Vector3 &position, real rotation, const Vector2 &size,
	NonOwningPtr<materials::Material> material, bool visible) :

	AnimatedSprite{std::move(name), position, rotation, size, material, color::White, visible}
{
	//Empty
}


AnimatedSprite::AnimatedSprite(std::optional<std::string> name,
	NonOwningPtr<materials::Material> material, const Color &color, bool visible) :

	AnimatedSprite{std::move(name), vector3::Zero, vector2::Zero, material, color, visible}
{
	//Empty
}

AnimatedSprite::AnimatedSprite(std::optional<std::string> name, const Vector2 &size,
	NonOwningPtr<materials::Material> material, const Color &color, bool visible) :

	AnimatedSprite{std::move(name), vector3::Zero, size, material, color, visible}
{
	//Empty
}

AnimatedSprite::AnimatedSprite(std::optional<std::string> name, const Vector3 &position, const Vector2 &size,
	NonOwningPtr<materials::Material> material, const Color &color, bool visible) :

	AnimatedSprite{std::move(name), position, 0.0_r, size, material, color, visible}
{
	//Empty
}

AnimatedSprite::AnimatedSprite(std::optional<std::string> name, const Vector3 &position, real rotation, const Vector2 &size,
	NonOwningPtr<materials::Material> material, const Color &color, bool visible) :

	Sprite{std::move(name), position, rotation, size, nullptr, color, visible},

	diffuse_animation_{material && material->DiffuseMap().first ?
		make_owning<textures::Animation>(*material->DiffuseMap().first) : nullptr},
	normal_animation_{material && material->NormalMap().first ?
		make_owning<textures::Animation>(*material->NormalMap().first) : nullptr},
	specular_animation_{material && material->SpecularMap().first ?
		make_owning<textures::Animation>(*material->SpecularMap().first) : nullptr},
	emissive_animation_{material && material->EmissiveMap().first ?
		make_owning<textures::Animation>(*material->EmissiveMap().first) : nullptr},

	material_{material ? make_owning<materials::Material>(*material) : nullptr},
	initial_material_{material}
{
	material_->DiffuseMap(diffuse_animation_);
	material_->NormalMap(normal_animation_);
	material_->SpecularMap(specular_animation_);
	material_->EmissiveMap(emissive_animation_);
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

		if (normal_animation_ && initial_material_->NormalMap().first)
			*normal_animation_ = *initial_material_->NormalMap().first;

		if (specular_animation_ && initial_material_->SpecularMap().first)
			*specular_animation_ = *initial_material_->SpecularMap().first;

		if (emissive_animation_ && initial_material_->EmissiveMap().first)
			*emissive_animation_ = *initial_material_->EmissiveMap().first;

		*material_ = *initial_material_;
		material_->DiffuseMap(diffuse_animation_);
		material_->NormalMap(normal_animation_);
		material_->SpecularMap(specular_animation_);
		material_->EmissiveMap(emissive_animation_);
	}
}


/*
	Playback
*/

void AnimatedSprite::Start() noexcept
{
	if (diffuse_animation_)
		diffuse_animation_->Start();
	if (normal_animation_)
		normal_animation_->Start();
	if (specular_animation_)
		specular_animation_->Start();
	if (emissive_animation_)
		emissive_animation_->Start();
}

void AnimatedSprite::Stop() noexcept
{
	if (diffuse_animation_)
		diffuse_animation_->Stop();
	if (normal_animation_)
		normal_animation_->Stop();
	if (specular_animation_)
		specular_animation_->Stop();
	if (emissive_animation_)
		emissive_animation_->Stop();
}

void AnimatedSprite::Reset() noexcept
{
	if (diffuse_animation_)
		diffuse_animation_->Reset();
	if (normal_animation_)
		normal_animation_->Reset();
	if (specular_animation_)
		specular_animation_->Reset();
	if (emissive_animation_)
		emissive_animation_->Reset();
}

void AnimatedSprite::Restart() noexcept
{
	if (diffuse_animation_)
		diffuse_animation_->Restart();
	if (normal_animation_)
		normal_animation_->Restart();
	if (specular_animation_)
		specular_animation_->Restart();
	if (emissive_animation_)
		emissive_animation_->Restart();
}


void AnimatedSprite::JumpForward(duration time) noexcept
{
	if (diffuse_animation_)
		diffuse_animation_->JumpForward(time);
	if (normal_animation_)
		normal_animation_->JumpForward(time);
	if (specular_animation_)
		specular_animation_->JumpForward(time);
	if (emissive_animation_)
		emissive_animation_->JumpForward(time);
}

void AnimatedSprite::JumpBackward(duration time) noexcept
{
	if (diffuse_animation_)
		diffuse_animation_->JumpBackward(time);
	if (normal_animation_)
		normal_animation_->JumpBackward(time);
	if (specular_animation_)
		specular_animation_->JumpBackward(time);
	if (emissive_animation_)
		emissive_animation_->JumpBackward(time);
}


/*
	Elapse time
*/

void AnimatedSprite::Elapse(duration time) noexcept
{
	if (diffuse_animation_)
		diffuse_animation_->Elapse(time);
	if (normal_animation_)
		normal_animation_->Elapse(time);
	if (specular_animation_)
		specular_animation_->Elapse(time);
	if (emissive_animation_)
		emissive_animation_->Elapse(time);

	Sprite::Elapse(time);
}

} //ion::graphics::scene::shapes