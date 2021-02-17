/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonSceneManager.cpp
-------------------------------------------
*/

#include "IonSceneManager.h"

namespace ion::graphics::scene
{

using namespace scene_manager;

namespace scene_manager::detail
{
} //scene_manager::detail


//Public

/*
	Cameras
	Creating
*/

NonOwningPtr<Camera> SceneManager::CreateCamera(std::string name)
{
	return CameraBase::Create(std::move(name));
}

NonOwningPtr<Camera> SceneManager::CreateCamera(std::string name, const render::Frustum &frustum)
{
	return CameraBase::Create(std::move(name), frustum);
}


NonOwningPtr<Camera> SceneManager::CreateCamera(const Camera &camera)
{
	return CameraBase::Create(camera);
}

NonOwningPtr<Camera> SceneManager::CreateCamera(Camera &&camera)
{
	return CameraBase::Create(std::move(camera));
}


/*
	Cameras
	Retrieving
*/

NonOwningPtr<Camera> SceneManager::GetCamera(std::string_view name) noexcept
{
	return CameraBase::Get(name);
}

NonOwningPtr<const Camera> SceneManager::GetCamera(std::string_view name) const noexcept
{
	return CameraBase::Get(name);
}


/*
	Cameras
	Removing
*/

void SceneManager::ClearCameras() noexcept
{
	CameraBase::Clear();
}

bool SceneManager::RemoveCamera(Camera &camera) noexcept
{
	return CameraBase::Remove(camera);
}

bool SceneManager::RemoveCamera(std::string_view name) noexcept
{
	return CameraBase::Remove(name);
}


/*
	Lights
	Creating
*/

NonOwningPtr<Light> SceneManager::CreateLight()
{
	return LightBase::Create();
}

NonOwningPtr<Light> SceneManager::CreateLight(light::LightType type,
	const Vector3 &position, const Vector3 &direction,
	const Color &ambient, const Color &diffuse, const Color &specular,
	real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
	real cutoff_angle, real outer_cutoff_angle,
	bool cast_shadows)
{
	return LightBase::Create(type,
		position, direction,
		ambient, diffuse, specular,
		attenuation_constant, attenuation_linear, attenuation_quadratic,
		cutoff_angle, outer_cutoff_angle,
		cast_shadows);
}


NonOwningPtr<Light> SceneManager::CreateLight(const Light &light)
{
	return LightBase::Create(light);
}

NonOwningPtr<Light> SceneManager::CreateLight(Light &&light)
{
	return LightBase::Create(std::move(light));
}


/*
	Lights
	Removing
*/

void SceneManager::ClearLights() noexcept
{
	return LightBase::Clear();
}

bool SceneManager::RemoveLight(Light &light) noexcept
{
	return LightBase::Remove(light);
}


/*
	Models
	Creating
*/

NonOwningPtr<Model> SceneManager::CreateModel()
{
	return ModelBase::Create();
}

NonOwningPtr<Model> SceneManager::CreateModel(bool visible)
{
	return ModelBase::Create(visible);
}


/*
	Models
	Removing
*/

void SceneManager::ClearModels() noexcept
{
	return ModelBase::Clear();
}

bool SceneManager::RemoveModel(Model &model) noexcept
{
	return ModelBase::Remove(model);
}

} //ion::graphics::scene