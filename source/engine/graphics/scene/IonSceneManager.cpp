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


//Protected

/*
	Events
*/

void SceneManager::Created(Camera &camera) noexcept
{
	camera.Scene(this);
}


//Public

/*
	Cameras
	Creating
*/

Camera& SceneManager::CreateCamera(const Camera &camera)
{
	return CameraManagerBase::Create(camera);
}

Camera& SceneManager::CreateCamera(Camera &&camera)
{
	return CameraManagerBase::Create(std::move(camera));
}


/*
	Cameras
	Retrieving
*/

Camera* SceneManager::GetCamera(std::string_view name) noexcept
{
	return CameraManagerBase::Get(name);
}

const Camera* SceneManager::GetCamera(std::string_view name) const noexcept
{
	return CameraManagerBase::Get(name);
}


/*
	Cameras
	Removing
*/

void SceneManager::ClearCameras() noexcept
{
	CameraManagerBase::Clear();
}

bool SceneManager::RemoveCamera(Camera &camera) noexcept
{
	return CameraManagerBase::Remove(camera);
}

bool SceneManager::RemoveCamera(std::string_view name) noexcept
{
	return CameraManagerBase::Remove(name);
}

} //ion::graphics::scene