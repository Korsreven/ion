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


/*
	Cameras
	Creating
*/

Camera& SceneManager::CreateCamera(const Camera &camera)
{
	return Create(camera);
}

Camera& SceneManager::CreateCamera(Camera &&camera)
{
	return Create(std::move(camera));
}


/*
	Cameras
	Retrieving
*/

Camera* SceneManager::GetCamera(std::string_view name) noexcept
{
	return Get(name);
}

const Camera* SceneManager::GetCamera(std::string_view name) const noexcept
{
	return Get(name);
}


/*
	Cameras
	Removing
*/

void SceneManager::ClearCameras() noexcept
{
	Clear();
}

bool SceneManager::RemoveCamera(Camera &camera) noexcept
{
	return Remove(camera);
}

bool SceneManager::RemoveCamera(std::string_view name) noexcept
{
	return Remove(name);
}

} //ion::graphics::scene