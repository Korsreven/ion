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

} //ion::graphics::scene