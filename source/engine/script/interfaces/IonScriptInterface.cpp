/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonScriptInterface.cpp
-------------------------------------------
*/

#include "IonScriptInterface.h"

namespace ion::script::interfaces
{

using namespace script_interface;

namespace script_interface::detail
{

} //script_interface::detail


/*
	Script repositories
	Creating
*/

NonOwningPtr<assets::repositories::ScriptRepository> ScriptInterface::CreateScriptRepository()
{
	return ScriptRepositoryBase::Create();
}

NonOwningPtr<assets::repositories::ScriptRepository> ScriptInterface::CreateScriptRepository(const assets::repositories::ScriptRepository &repository)
{
	return ScriptRepositoryBase::Create(repository);
}

NonOwningPtr<assets::repositories::ScriptRepository> ScriptInterface::CreateScriptRepository(assets::repositories::ScriptRepository &&repository)
{
	return ScriptRepositoryBase::Create(std::move(repository));
}


/*
	Script repositories
	Removing
*/

void ScriptInterface::ClearScriptRepositories() noexcept
{
	ScriptRepositoryBase::Clear();
}

bool ScriptInterface::RemoveScriptRepository(assets::repositories::ScriptRepository &repository) noexcept
{
	return ScriptRepositoryBase::Remove(repository);
}


/*
	File repositories
	Creating
*/

NonOwningPtr<assets::repositories::FileRepository> ScriptInterface::CreateFileRepository()
{
	return FileRepositoryBase::Create();
}

NonOwningPtr<assets::repositories::FileRepository> ScriptInterface::CreateFileRepository(const assets::repositories::FileRepository &repository)
{
	return FileRepositoryBase::Create(repository);
}

NonOwningPtr<assets::repositories::FileRepository> ScriptInterface::CreateFileRepository(assets::repositories::FileRepository &&repository)
{
	return FileRepositoryBase::Create(std::move(repository));
}


/*
	File repositories
	Removing
*/

void ScriptInterface::ClearFileRepositories() noexcept
{
	FileRepositoryBase::Clear();
}

bool ScriptInterface::RemoveFileRepository(assets::repositories::FileRepository &repository) noexcept
{
	return FileRepositoryBase::Remove(repository);
}

} //ion::script::interfaces